#include "emulator_private.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "../uart_controller/uart_controller.h"
#include "../ini_handler/ini_handler_private.h"
#include "esp_log.h"

#define TAG "EMULATOR"

#define NUMBER_OF_PLAYERS 2

emulator_t emulator = {.game_state = EMULATOR_GAMESTATE_WAIT_CONFIG};

SemaphoreHandle_t isTurnRetrieved;
SemaphoreHandle_t isStateRetrieved;

// xSemTake

void EMULATOR_Init(void)
{
    srand(time(NULL));
    isTurnRetrieved = xSemaphoreCreateBinary();
    isStateRetrieved = xSemaphoreCreateBinary();

    xTaskCreate(EMULATOR_MainGameProccessThread, "game_emulator", 4096, NULL, configMAX_PRIORITIES - 2, NULL);
}

void EMULATOR_SetGameInfo(uint8_t isLoaded, char *mode, uint16_t player1Score, uint16_t player2Score, uint16_t maxRounds)
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
        // error
    }

    emulator.max_rounds = maxRounds;

    // load game data got
    if (isLoaded == 1)
    {
        // save players score
    }
    // new game
    else
    {
    }
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
                ESP_LOGI(TAG, "GameConfig retrieved");
                // Game is loaded
                if (emulator.isLoaded == 1)
                {
                }
                else
                {
                    // New Game
                    switch (emulator.cur_battlemode)
                    {
                    case EMULATOR_BATTLEMODE_PVP:
                    case EMULATOR_BATTLEMODE_PVE:
                        emulator.game_state = EMULATOR_GAMESTATE_WAIT_PLAYER1_TURN;
                        INIHANDLER_SendClientGoodConfig();
                        break;
                    case EMULATOR_BATTLEMODE_EVE:
                        emulator.game_state = EMULATOR_GAMESTATE_PROCCESS_BOTS_GAME;
                        INIHANDLER_SendClientGoodConfig();
                        break;
                    default:
                        ESP_LOGE(TAG, "Unknown battle mode");
                        break;
                    }
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

            ESP_LOGW(TAG, "turn_result: %s", emulator_turnresult_winner_strings[turn_result]);
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
            INIHANDLER_GetTurnResult(p_result);
            emulator.current_round++;
            break;
        }
        case EMULATOR_GAMESTATE_PROCCESS_BOTS_GAME:
        {
            GetTurnResult_CommonData_t p_result = {.winner = 0};

            // Play `max_rounds` times.
            for (int i = 0; i < emulator.max_rounds; i++)
            {
                ESP_LOGI(TAG, "Bots round %d", emulator.current_round);
                emulator.players_choice[EMULATOR_PLAYER_1] = rand() % (2 + 1 - 0) + 0;
                emulator.players_choice[EMULATOR_PLAYER_2] = rand() % (2 + 1 - 0) + 0;

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

                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    INIHANDLER_GetTurnResult(p_result);
                    emulator.game_state = EMULATOR_GAMESTATE_CLEAN_GAME;
                    break;
                }
                case EMULATOR_TURNRESULT_WINNER_ROUND_FIRST:
                case EMULATOR_TURNRESULT_WINNER_ROUND_SECOND:
                case EMULATOR_TURNRESULT_WINNER_ROUND_DRAW:
                {
                    emulator.current_round++;

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
            emulator.players_choice[EMULATOR_PLAYER_2] = rand() % (2 + 1 - 0) + 0;
            ;
            emulator_turnresult_winner_t turn_result = EMULATOR_ChoseWinner();
            switch (turn_result)
            {
            case EMULATOR_TURNRESULT_WINNER_GAME_FIRST:
            {
                p_result.winner = 1;

                emulator.game_state = EMULATOR_GAMESTATE_WAIT_CONFIG;
                INIHANDLER_GetTurnResult(p_result);

                break;
            }
            case EMULATOR_TURNRESULT_WINNER_GAME_SECOND:
            {
                p_result.winner = 2;

                emulator.game_state = EMULATOR_GAMESTATE_WAIT_CONFIG;
                INIHANDLER_GetTurnResult(p_result);

                break;
            }
            case EMULATOR_TURNRESULT_WINNER_DRAW:
            {
                p_result.winner = 3;

                emulator.game_state = EMULATOR_GAMESTATE_WAIT_CONFIG;
                INIHANDLER_GetTurnResult(p_result);

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
        emulator.recent_turn_choice = EMULATOR_ROCK;
    }
    else if (strcmp(turn, "PAPER") == 0)
    {
        emulator.recent_turn_choice = EMULATOR_PAPER;
    }
    else if (strcmp(turn, "SCISSORS") == 0)
    {
        emulator.recent_turn_choice = EMULATOR_SCISSORS;
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
        return "NO";
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
    if (emulator.current_round + 1 >= emulator.max_rounds)
    {
        // if not draw then someone will win
        is_last_round = true;
        ESP_LOGW(TAG, "This is last round!");
    }

    switch (p1)
    {
    case EMULATOR_ROCK:
    {
        switch (p2)
        {
        case EMULATOR_ROCK:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            emulator.players_score[EMULATOR_PLAYER_1]++;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            break;
        case EMULATOR_PAPER:
            emulator.players_score[EMULATOR_PLAYER_2]++;
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_SECOND;
            break;
        case EMULATOR_SCISSORS:
            emulator.players_score[EMULATOR_PLAYER_1]++;
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_FIRST;
            break;
        default:
            emulator.players_score[EMULATOR_PLAYER_1]++;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            break;
        }
        break;
    }
    case EMULATOR_PAPER:
    {
        switch (p2)
        {
        case EMULATOR_ROCK:
            emulator.players_score[EMULATOR_PLAYER_1]++;
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_FIRST;
            break;
        case EMULATOR_PAPER:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            emulator.players_score[EMULATOR_PLAYER_1]++;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            break;
        case EMULATOR_SCISSORS:
            emulator.players_score[EMULATOR_PLAYER_2]++;
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_SECOND;
            break;
        default:
            emulator.players_score[EMULATOR_PLAYER_1]++;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            break;
        }
        break;
    }
    case EMULATOR_SCISSORS:
    {
        switch (p2)
        {
        case EMULATOR_ROCK:
            emulator.players_score[EMULATOR_PLAYER_2]++;
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_SECOND;
            break;
        case EMULATOR_PAPER:
            emulator.players_score[EMULATOR_PLAYER_1]++;
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_FIRST;
            break;
        case EMULATOR_SCISSORS:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            emulator.players_score[EMULATOR_PLAYER_1]++;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            break;
        default:
            game_turn_result = EMULATOR_TURNRESULT_WINNER_ROUND_DRAW;
            emulator.players_score[EMULATOR_PLAYER_1]++;
            emulator.players_score[EMULATOR_PLAYER_2]++;
            break;
        }
        break;
    }
    default:
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