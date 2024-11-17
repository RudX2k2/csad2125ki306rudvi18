#ifndef EMULATOR_COMM_DATA_H
#define EMULATOR_COMM_DATA_H

typedef struct
{
    char mode[4];
    int Player1;
    int Player2;
    int maxRounds;
    int winner;
} GameState_CommonData_t;

typedef struct
{
    char mode[4];
    int player1;
    int player2;
    int max_rounds;
    int winner;
} GetTurnResult_CommonData_t;

typedef struct{
    char mode[4];
} SetPlayerTurn_CommonData_t;

#endif // EMULATOR_COMM_DATA_H