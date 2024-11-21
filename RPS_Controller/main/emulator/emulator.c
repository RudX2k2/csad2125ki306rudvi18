#include "emulator_private.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "../uart_controller/uart_controller.h"
#include "../ini_handler/ini_handler_private.h"
#include "esp_log.h"
#include "esp_random.h"

#define TAG "EMULATOR"

#define NUMBER_OF_PLAYERS 2

emulator_t emulator = {.game_state = EMULATOR_GAMESTATE_WAIT_CONFIG};

SemaphoreHandle_t isTurnRetrieved;
SemaphoreHandle_t isStateRetrieved;

// xSemTake

void EMULATOR_Init(void)
{
    // srand(time(NULL));
    isTurnRetrieved = xSemaphoreCreateBinary();
    isStateRetrieved = xSemaphoreCreateBinary();

    xTaskCreate(EMULATOR_MainGameProccessThread, "game_emulator", 4096, NULL, configMAX_PRIORITIES - 2, NULL);
}

int EMULATOR_SetGameInfo(uint8_t isLoaded, char *mode, int player1Score, int player2Score, int curRound, int maxRounds)

{
    emulator.isLoaded = isLoaded;

    if (strcmp(mode, "PVP") == 0)
    {
        emulator.cur_battlemode = EMULATOR_BATTLEMODE_PVP;
    }
    else if (strcmp(mode, "PVE") == 0)
    {
        emulator.cur_battlemode = EMULATOR_BATTLEMODE_PVE;
    }
    else if (strcmp(mode, "EVE") == 0)
    {
        emulator.cur_battlemode = EMULATOR_BATTLEMODE_EVE;
    }
    else
    {
        ESP_LOGE(TAG, "Mode error");
        return -1;
    }

    if (maxRounds < 1 || maxRounds > 5)
    {
        ESP_LOGE(TAG, "maxRounds error");

        return -1;
    }
    else
    {
        emulator.max_rounds = maxRounds;
    }

    // load game data got
    if (isLoaded == 1)
    {
        if (curRound < 0)
        {
            ESP_LOGE(TAG, "curRound error: %d", curRound);

            return -1;
        }

        if (curRound > maxRounds)
        {
            ESP_LOGE(TAG, "curRound error: %d. bigger then max %d.", curRound, maxRounds);

            return -1;
        }

        if (player1Score + player2Score >= maxRounds)
        {
            ESP_LOGE(TAG, "playersScore error");

            return -1;
        }

        // save players score
        emulator.players_score[EMULATOR_PLAYER_1] = player1Score;
        emulator.players_score[EMULATOR_PLAYER_2] = player2Score;
        emulator.current_round = curRound;
    }

    return 0;
}

static void EMULATOR_MainGameProccessThread(void *a)
{
    while (true)
    {
        switch (emulator.game_state)
        {
        case EMULATOR_GAMESTATE_WAIT_CONFIG:
        {
            ESP_LOGW(TAG, "Wait for settings...");

            int err = EMULATOR_TakeSemIsStateRetrieved();
            if (err)
            {
                ESP_LOGE(TAG, "Didn't get the game configs");
            }
            else
            {
                // Game is loaded
                if (emulator.isLoaded == 1)
                {
                    ESP_LOGI(TAG, "GOT LAODED DATA\np1:%d\np2:%d\nmax:%d\ncur:%d\nMode:%s", emulator.players_score[EMULATOR_PLAYER_1],
                             emulator.players_score[EMULATOR_PLAYER_2],
                             emulator.max_rounds,
                             emulator.current_round,
                             EMULATOR_GetBattleModeString(emulator.cur_battlemode));
                }
                // New Game
                switch (emulator.cur_battlemode)
                {
                case EMULATOR_BATTLEMODE_PVP:
                case EMULATOR_BATTLEMODE_PVE:
                    emulator.game_state = EMULATOR_GAMESTATE_WAIT_PLAYER1_TURN;
                    break;
                case EMULATOR_BATTLEMODE_EVE:
                    emulator.game_state = EMULATOR_GAMESTATE_PROCCESS_BOTS_GAME;
                    break;
                default:
                    ESP_LOGE(TAG, "Unknown battle mode");
                    break;
                }
            }
            break;
        }
        case EMULATOR_GAMESTATE_WAIT_PLAYER1_TURN:
        {
            ESP_LOGW(TAG, "Wait for player 1 turn...");
            INIHANDLER_SendWaitTurn(EMULATOR_PLAYER_1);
            int err = EMULATOR_TakeSemIsTurnRetrieved();
            if (err)
            {
                ESP_LOGE(TAG, "Didn't get player1 turn :(");
            }
            else
            {
                emulator.players_choice[EMULATOR_PLAYER_1] = emulator.recent_turn_choice;

                switch (emulator.cur_battlemode)
                {
                case EMULATOR_BATTLEMODE_PVP:
                    emulator.game_state = EMULATOR_GAMESTATE_WAIT_PLAYER2_TURN;
                    break;
                case EMULATOR_BATTLEMODE_PVE:
                    emulator.game_state = EMULATOR_GAMESTATE_PROCCESS_PLAYERBOT_GAME;
                    break;
                case EMULATOR_BATTLEMODE_EVE:
                    ESP_LOGE(TAG, "Wrong battle mode");
                    break;
                default:
                    ESP_LOGE(TAG, "Unknown battle mode");
                    break;
                }
            }
            break;
        }
        case EMULATOR_GAMESTATE_WAIT_PLAYER2_TURN:
        {
            ESP_LOGW(TAG, "Wait for player 2 turn...");
            INIHANDLER_SendWaitTurn(EMULATOR_PLAYER_2);
            int err = EMULATOR_TakeSemIsTurnRetrieved();
            if (err)
            {
                ESP_LOGE(TAG, "Didn't get player2 turn :(");
            }
            else
            {
                emulator.players_choice[EMULATOR_PLAYER_2] = emulator.recent_turn_choice;

                switch (emulator.cur_battlemode)
                {
                case EMULATOR_BATTLEMODE_PVP:
                    emulator.game_state = EMULATOR_GAMESTATE_PROCCESS_PLAYERS_GAME;
                    break;
                default:
                    ESP_LOGE(TAG, "Wrong battle mode");
                    break;
                }
            }
            break;
        }
        case EMULATOR_GAMESTATE_PROCCESS_PLAYERS_GAME:
        {
            GetTurnResult_CommonData_t p_result = {.winner = 0};

            emulator_turnresult_winner_t turn_result = EMULATOR_ChoseWinner();

            p_result.player1 = emulator.players_score[EMULATOR_PLAYER_1];
            p_result.player2 = emulator.players_score[EMULATOR_PLAYER_2];
            memcpy(p_result.mode, EMULATOR_GetBattleModeString(emulator.cur_battlemode), 4);
            p_result.max_rounds = emulator.max_rounds;
            p_result.cur_round = emulator.current_round;
            memcpy(p_result.choice_p1, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_1]], EMULATOR_PLAYER_CHOICE_STRING_LEN);
            memcpy(p_result.choice_p2, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_2]], EMULATOR_PLAYER_CHOICE_STRING_LEN);

            ESP_LOGW(TAG, "EVE turn_result: %s", emulator_turnresult_winner_strings[turn_result]);
            switch (turn_result)
            {
            case EMULATOR_TURNRESULT_WINNER_GAME_FIRST:
            {
                p_result.winner = 1;
                emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;
                break;
            }
            case EMULATOR_TURNRESULT_WINNER_GAME_SECOND:
            {
                p_result.winner = 2;
                emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;
                break;
            }
            case EMULATOR_TURNRESULT_WINNER_DRAW:
            {
                p_result.winner = 3;
                emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;
                break;
            }
            case EMULATOR_TURNRESULT_WINNER_ROUND_FIRST:
            case EMULATOR_TURNRESULT_WINNER_ROUND_SECOND:
            case EMULATOR_TURNRESULT_WINNER_ROUND_DRAW:
            {
                emulator.game_state = EMULATOR_GAMESTATE_WAIT_PLAYER1_TURN;
                break;
            }
            default:
                break;
            }
            // In PVP send result after each turn
            vTaskDelay(500 / portTICK_PERIOD_MS);

            INIHANDLER_GetTurnResult(p_result);
            break;
        }
        case EMULATOR_GAMESTATE_PROCCESS_BOTS_GAME:
        {
            GetTurnResult_CommonData_t p_result = {.winner = 0};

            // Play `max_rounds` times.
            for (int i = 0; i < emulator.max_rounds; i++)
            {
                emulator.players_choice[EMULATOR_PLAYER_1] = esp_random() % 3;
                emulator.players_choice[EMULATOR_PLAYER_2] = esp_random() % 3;
                // Very interesting bug. Rand gets ram/heap at initialization and after when try access heap(malloc) - app crashes
                // rand() % (2 + 1 - 0) + 0;

                emulator_turnresult_winner_t turn_result = EMULATOR_ChoseWinner();

                ESP_LOGI(TAG, "Round %d - EVE turn Result is %s", emulator.current_round, emulator_turnresult_winner_strings[turn_result]);
                switch (turn_result)
                {
                case EMULATOR_TURNRESULT_WINNER_GAME_FIRST:
                {
                    p_result.winner = 1;
                    p_result.player1 = emulator.players_score[EMULATOR_PLAYER_1];
                    p_result.player2 = emulator.players_score[EMULATOR_PLAYER_2];

                    memcpy(p_result.mode, EMULATOR_GetBattleModeString(emulator.cur_battlemode), 4);
                    p_result.max_rounds = emulator.max_rounds;

                    p_result.cur_round = emulator.current_round;
                    memcpy(p_result.choice_p1, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_1]], EMULATOR_PLAYER_CHOICE_STRING_LEN);
                    memcpy(p_result.choice_p2, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_2]], EMULATOR_PLAYER_CHOICE_STRING_LEN);

                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    INIHANDLER_GetTurnResult(p_result);

                    emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;

                    break;
                }
                case EMULATOR_TURNRESULT_WINNER_GAME_SECOND:
                {

                    p_result.winner = 2;
                    p_result.player1 = emulator.players_score[EMULATOR_PLAYER_1];
                    p_result.player2 = emulator.players_score[EMULATOR_PLAYER_2];

                    memcpy(p_result.mode, EMULATOR_GetBattleModeString(emulator.cur_battlemode), 4);
                    p_result.max_rounds = emulator.max_rounds;
                    p_result.cur_round = emulator.current_round;
                    memcpy(p_result.choice_p1, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_1]], EMULATOR_PLAYER_CHOICE_STRING_LEN);
                    memcpy(p_result.choice_p2, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_2]], EMULATOR_PLAYER_CHOICE_STRING_LEN);

                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    INIHANDLER_GetTurnResult(p_result);
                    emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;

                    break;
                }
                case EMULATOR_TURNRESULT_WINNER_DRAW:
                {

                    p_result.winner = 3;
                    p_result.player1 = emulator.players_score[EMULATOR_PLAYER_1];
                    p_result.player2 = emulator.players_score[EMULATOR_PLAYER_2];

                    memcpy(p_result.mode, EMULATOR_GetBattleModeString(emulator.cur_battlemode), 4);
                    p_result.max_rounds = emulator.max_rounds;
                    p_result.cur_round = emulator.current_round;
                    memcpy(p_result.choice_p1, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_1]], EMULATOR_PLAYER_CHOICE_STRING_LEN);
                    memcpy(p_result.choice_p2, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_2]], EMULATOR_PLAYER_CHOICE_STRING_LEN);

                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    INIHANDLER_GetTurnResult(p_result);
                    emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;

                    break;
                }
                case EMULATOR_TURNRESULT_WINNER_ROUND_FIRST:
                case EMULATOR_TURNRESULT_WINNER_ROUND_SECOND:
                case EMULATOR_TURNRESULT_WINNER_ROUND_DRAW:
                {

                    break;
                }
                default:
                    break;
                }
            }
            break;
        }
        case EMULATOR_GAMESTATE_PROCCESS_PLAYERBOT_GAME:
        {
            GetTurnResult_CommonData_t p_result = {.winner = 0};
            // Since this is PVE - random bot result
            emulator.players_choice[EMULATOR_PLAYER_2] = esp_random() % 3;

            emulator_turnresult_winner_t turn_result = EMULATOR_ChoseWinner();

            // ESP_LOGW(TAG, "PVE turn_result: %s", emulator_turnresult_winner_strings[turn_result]);
            switch (turn_result)
            {
            case EMULATOR_TURNRESULT_WINNER_GAME_FIRST:
            {
                p_result.winner = 1;
                emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;
                break;
            }
            case EMULATOR_TURNRESULT_WINNER_GAME_SECOND:
            {
                p_result.winner = 2;
                emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;
                break;
            }
            case EMULATOR_TURNRESULT_WINNER_DRAW:
            {
                p_result.winner = 3;
                emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;
                break;
            }
            case EMULATOR_TURNRESULT_WINNER_ROUND_FIRST:
            case EMULATOR_TURNRESULT_WINNER_ROUND_SECOND:
            case EMULATOR_TURNRESULT_WINNER_ROUND_DRAW:
            {
                emulator.game_state = EMULATOR_GAMESTATE_WAIT_PLAYER1_TURN;
                break;
            }
            default:
                break;
            }

            p_result.player1 = emulator.players_score[EMULATOR_PLAYER_1];
            p_result.player2 = emulator.players_score[EMULATOR_PLAYER_2];
            memcpy(p_result.mode, EMULATOR_GetBattleModeString(emulator.cur_battlemode), 4);
            p_result.max_rounds = emulator.max_rounds;
            p_result.cur_round = emulator.current_round;
            memcpy(p_result.choice_p1, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_1]], EMULATOR_PLAYER_CHOICE_STRING_LEN);
            memcpy(p_result.choice_p2, emulator_player_choice_string[emulator.players_choice[EMULATOR_PLAYER_2]], EMULATOR_PLAYER_CHOICE_STRING_LEN);
            
            // In PVP and PVE send result after each turn
            vTaskDelay(500 / portTICK_PERIOD_MS);
            INIHANDLER_GetTurnResult(p_result);
            break;
        }
        case EMULATOR_GAMESTATE_CLEAN_GAME:
        {
            emulator.players_score[EMULATOR_PLAYER_1] = 0;
            emulator.players_score[EMULATOR_PLAYER_2] = 0;
            emulator.winner = 0;
            emulator.isLoaded = false;
            emulator.max_rounds = 0;
            emulator.current_round = 0;
            emulator.cur_battlemode = EMULATOR_BATTLEMODE_IDLE;

            emulator.game_state = EMULATOR_GAMESTATE_WAIT_CONFIG;
            break;
        }
        default:
            break;
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void EMULATOR_SetNewRecievedTurn(char *turn)
{
    if (strcmp(turn, "ROCK") == 0)
    {
        emulator.recent_turn_choice = EMULATOR_CHOICE_ROCK;
    }
    else if (strcmp(turn, "PAPER") == 0)
    {
        emulator.recent_turn_choice = EMULATOR_CHOICE_PAPER;
    }
    else if (strcmp(turn, "SCISSORS") == 0)
    {
        emulator.recent_turn_choice = EMULATOR_CHOICE_SCISSORS;
    }
    else
    {
        ESP_LOGE(TAG, "NO SUCH TURN");
    }
}

void EMULATOR_StartGame(void)
{
    ESP_LOGW(TAG, "LETS START THE GAME!");
}

void EMULATOR_GiveSemIsTurnRetrieved(void)
{
    xSemaphoreGive(isTurnRetrieved);
}

int EMULATOR_TakeSemIsTurnRetrieved(void)
{
    if (xSemaphoreTake(isTurnRetrieved, portMAX_DELAY) == pdTRUE)
    {
        return 0;
    }
    return -1;
}

void EMULATOR_GiveSemIsStateRetrieved(void)
{
    if (xSemaphoreGive(isStateRetrieved) == pdTRUE)
    {
        ESP_LOGD(TAG, "Gave state sem!");
    }
    else
    {
        ESP_LOGE(TAG, "Can't give state sem!");
    }
}

int EMULATOR_TakeSemIsStateRetrieved(void)
{
    if (xSemaphoreTake(isStateRetrieved, portMAX_DELAY) == pdTRUE)
    {
        return 0;
    }
    return -1;
}

char *EMULATOR_GetBattleModeString(emulator_battlemode_enum_t mode)
{
    switch (mode)
    {
    case EMULATOR_BATTLEMODE_PVP:
        return "PVP";
        break;
    case EMULATOR_BATTLEMODE_PVE:
        return "PVE";
        break;
    case EMULATOR_BATTLEMODE_EVE:
        return "EVE";
        break;
    case EMULATOR_BATTLEMODE_IDLE:
        return "NOO";
        break;
    default:
        break;
    }
    return NULL;
}

GameState_CommonData_t EMULATOR_GetGameState(void)
{
    GameState_CommonData_t gamestate = {
        .Player1 = emulator.players_score[EMULATOR_PLAYER_1],
        .Player2 = emulator.players_score[EMULATOR_PLAYER_2],
        .maxRounds = emulator.max_rounds,
        .winner = emulator.winner,
    };
    memcpy(gamestate.mode, EMULATOR_GetBattleModeString(emulator.cur_battlemode), 4);
    return gamestate;
}

emulator_turnresult_winner_t EMULATOR_ChoseWinner()
{
    emulator_player_choice_enum_t p1 = emulator.players_choice[EMULATOR_PLAYER_1];
    emulator_player_choice_enum_t p2 = emulator.players_choice[EMULATOR_PLAYER_2];
    emulator_turnresult_winner_t game_turn_result = {0};

    bool is_last_round = false;
    if ((emulator.current_round += 1) >= emulator.max_rounds)
    {
        // if not draw then someone will win
        is_last_round = true;
        ESP_LOGW(TAG, "This is last round!");
    }

    switch (p1)
    {
    case EMULATOR_CHOICE_ROCK:
        switch (p2)
        {
        case EMULATOR_CHOICE_ROCK:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            break;
        case EMULATOR_CHOICE_PAPER:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_SECOND;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            break;
        case EMULATOR_CHOICE_SCISSORS:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_FIRST;
            emulator.players_score[EMULATOR_PLAYER_1]++;
            break;
        default:
            // Handle invalid p2 input here if necessary
            break;
        }
        break;

    case EMULATOR_CHOICE_PAPER:
        switch (p2)
        {
        case EMULATOR_CHOICE_ROCK:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_FIRST;
            emulator.players_score[EMULATOR_PLAYER_1]++;
            break;
        case EMULATOR_CHOICE_PAPER:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            break;
        case EMULATOR_CHOICE_SCISSORS:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_SECOND;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            break;
        default:
            // Handle invalid p2 input here if necessary
            break;
        }
        break;

    case EMULATOR_CHOICE_SCISSORS:
        switch (p2)
        {
        case EMULATOR_CHOICE_ROCK:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_SECOND;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            break;
        case EMULATOR_CHOICE_PAPER:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_FIRST;
            emulator.players_score[EMULATOR_PLAYER_1]++;
            break;
        case EMULATOR_CHOICE_SCISSORS:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            break;
        default:
            // Handle invalid p2 input here if necessary
            break;
        }
        break;

    default:
        // Handle invalid p1 input here if necessary
        break;
    }

    // Choose the winner
    if (is_last_round)
    {
        if (emulator.players_score[EMULATOR_PLAYER_1] > emulator.players_score[EMULATOR_PLAYER_2])
        {
            game_turn_result = EMULATOR_TURNRESULT_WINNER_GAME_FIRST;
        }
        else if (emulator.players_score[EMULATOR_PLAYER_1] < emulator.players_score[EMULATOR_PLAYER_2])
        {
            game_turn_result = EMULATOR_TURNRESULT_WINNER_GAME_SECOND;
        }
        else
        {
            game_turn_result = EMULATOR_TURNRESULT_WINNER_DRAW;
        }
    }

    return game_turn_result;
}