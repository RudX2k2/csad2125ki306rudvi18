#ifndef EMULATOR_COMM_DATA_H
#define EMULATOR_COMM_DATA_H


/// @brief `GameState_CommonData_t` is common data for sending turn result from the server ti client
typedef struct
{
    char mode[4];
    int Player1;
    int Player2;
    int maxRounds;
    int currentRound;
    int winner;
} GameState_CommonData_t;


/// @brief GetTurnResult_CommonData_t is common data for sending turn result from the server ti client
typedef struct
{
    char mode[4];
    int player1;
    int player2;
    int cur_round;
    int max_rounds;
    int winner;
    char choice_p1[9];
    char choice_p2[9];
} GetTurnResult_CommonData_t;


/// @brief SetPlayerTurn_CommonData_t is commonn turn result sent by the client
typedef struct{
    char turn[10];
} SetPlayerTurn_CommonData_t;

#endif // EMULATOR_COMM_DATA_H