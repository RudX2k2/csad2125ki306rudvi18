#include "emulator_private.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "../uart_controller/uart_controller.h"
#include "../ini_handler/ini_handler_private.h"
#include "esp_log.h"

#define TAG "EMULATOR"

#define NUMBER_OF_PLAYERS 2

typedef struct
{
    uint8_t player_no;

} emulator_player_t;

emulator_t emulator = {.game_state = EMULATOR_GAMESTATE_WAIT_CONFIG};

SemaphoreHandle_t isTurnRetrieved;
SemaphoreHandle_t isStateRetrieved;

// xSemTake

void EMULATOR_Init(void)
{
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

                if (emulator.isLoaded == 1)
                {
                }
                else
                {
                    // new game
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
            }
            break;
        }
        case EMULATOR_GAMESTATE_WAIT_PLAYER1_TURN:
        {
            ESP_LOGW(TAG, "Wait for player 1 turn...");
            INIHANDLER_SendWaitTurn();
            int err = EMULATOR_TakeSemIsTurnRetrieved();
            if (err)
            {
                ESP_LOGE(TAG, "Didn't get player1 turn :(");
            }
            else
            {
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
            INIHANDLER_SendWaitTurn();
            int err = EMULATOR_TakeSemIsTurnRetrieved();
            if (err)
            {
                ESP_LOGE(TAG, "Didn't get player2 turn :(");
            }

            switch (emulator.cur_battlemode)
            {
            case EMULATOR_BATTLEMODE_PVP:
                emulator.game_state = EMULATOR_GAMESTATE_PROCCESS_PLAYERS_GAME;
                break;
            case EMULATOR_BATTLEMODE_PVE:
            case EMULATOR_BATTLEMODE_EVE:
                ESP_LOGE(TAG, "Wrong battle mode");
                break;
            default:
                ESP_LOGE(TAG, "Unknown battle mode");
                break;
            }
            break;
        }
        case EMULATOR_GAMESTATE_PROCCESS_PLAYERS_GAME:
        {

            ESP_LOGI(TAG, "Proccess game! Turns are %d and %d",
                     emulator.players_choice[EMULATOR_PLAYER_1],
                     emulator.players_choice[EMULATOR_PLAYER_2]);
        }
        default:
            break;
        case EMULATOR_GAMESTATE_PROCCESS_PLAYERBOT_GAME:
            break;
        case EMULATOR_GAMESTATE_PROCCESS_BOTS_GAME:
            break;
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

// void EMULATOR_UpdateGameSettings(ClientMsg_GameSettings_t game_settings)
// {
//     // Start new game
//     if (game_settings.isGameToLoad == 0)
//     {
//         if (strcmp(game_settings.mode, "PVP") == 0)
//         {
//             emulator.players_type[0] = EMULATOR_PLAYER_TYPE_HUMAN;
//             emulator.players_type[1] = EMULATOR_PLAYER_TYPE_HUMAN;
//         }
//         else if (strcmp(game_settings.mode, "PVE") == 0)
//         {
//             emulator.players_type[0] = EMULATOR_PLAYER_TYPE_HUMAN;
//             emulator.players_type[1] = EMULATOR_PLAYER_TYPE_BOT;
//         }
//         else if (strcmp(game_settings.mode, "EVE") == 0)
//         {
//             emulator.players_type[0] = EMULATOR_PLAYER_TYPE_BOT;
//             emulator.players_type[1] = EMULATOR_PLAYER_TYPE_BOT;
//         }
//         else
//         {
//             ESP_LOGE(TAG, "Wrong mode");
//         }
//         emulator.max_rounds = game_settings.maxRoundsAmount;

//         emulator.game_state = EMULATOR_GAMESTATE_WAIT_TURN;
//     }
//     // Continue game
//     else
//     {
//     }

//     // launch task to update
// }

// void EMULATOR_UpdateEmulationTask(void *a)
// {
//     switch (emulator.game_state)
//     {
//     case EMULATOR_GAMESTATE_WAIT_CONFIG:
//         // inihandler wait
//         break;
//     case EMULATOR_GAMESTATE_WAIT_TURN:
//         for (uint8_t i = 0; i < EMULATOR_PLAYERS_AMOUNT; i++)
//         {
//             if (emulator.players_type[i] == EMULATOR_PLAYER_TYPE_HUMAN)
//             {
//                 // inihandler - wait for choice
//                 bool got_responce = false;
//                 while (!got_responce)
//                 {
//                     // wait
//                     vTaskDelay(5 / portTICK_PERIOD_MS);
//                 }

//                 INIHANDLER_SendWaitTurn();

//                 int err = EMULATOR_TakeSemIsTurnRetrieved();
//                 if (err)
//                 {
//                     ESP_LOGE(TAG, "Player turn wasn't retrieved");
//                 }
//                 else
//                 {
//                     // Turn retrieved - proccess
//                     ESP_LOGE(TAG, "GOT TURN");
//                     ESP_LOGE(TAG, "%d", emulator.recent_turn_choice);
//                 }
//             }
//         }

//         break;
//     default:
//         break;
//     }
// }

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
        // error
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
    ESP_LOGW(TAG, "Try to give sem...");
    if (xSemaphoreGive(isStateRetrieved) == pdTRUE)
    {
        ESP_LOGI(TAG, "Gave state sem!");
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
        ESP_LOGW(TAG, "Semaphore taken!");
        return 0;
    }
    return -1;
}
