#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <uart_controller/uart_controller.h>

#define NUMBER_OF_PLAYERS 2

typedef enum{
    EMULATOR_ROCK = 0,
    EMULATOR_PAPER,
    EMULATOR_SCISSORS,
} emulator_player_choice_enum_t;

typedef enum{
    EMULATOR_MODE_PVP = 0,
    EMULATOR_MODE_PVE,
    EMULATOR_MODE_EVE,
    EMULATOR_MODE_IDLE,
} emulator_mode_enum_t;

typedef enum{
    EMULATOR_PLAYER_1,
    EMULATOR_PLAYER_2,
    EMULATOR_PLAYERS_AMOUNT,
} emulator_players_enum_t;

typedef enum{
    EMULATOR_PLAYER_TYPE_HUMAN = 0,
    EMULATOR_PLAYER_TYPE_BOT,
} emulator_player_type_enum_t;

typedef enum{
    EMULATOR_GAMESTATE_WAIT_TURN,
    EMULATOR_GAMESTATE_IDLE,
} emulator_gamestate_enum_t;

typedef struct{
    volatile bool stop_game;
    emulator_mode_enum_t cur_mode;
    emulator_player_choice_enum_t players_choice[EMULATOR_PLAYERS_AMOUNT];
} emulator_t;

typedef struct{
    uint8_t player_no;
    
} emulator_player_t;

emulator_t emulator = {.stop_game = false, .cur_mode = EMULATOR_MODE_IDLE};

void EMULATOR_MainGameProccessThread(void)
{
    while(emulator.stop_game)
    {
        switch (emulator.cur_mode)
        {
        case EMULATOR_MODE_PVP:
        // 1. Wait player 1 move
        // 2. Wait player 2 move
        // 3. Check who win

            
        break;
        case EMULATOR_MODE_PVE:
            
        break;
        case EMULATOR_MODE_EVE:
            
        break;
        
        default:
            break;
        }
    }
}
