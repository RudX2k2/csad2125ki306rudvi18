#ifndef COMMONDATATYPES_H
#define COMMONDATATYPES_H

#include <string>

struct GameState {
    int isLoaded;
    std::string mode;
    int player1Score;
    int player2Score;
    int curRound;
    int maxRoundsAmount;
    int winner;
    std::string choiceP1;
    std::string choiceP2;
};

struct ClientGameTurn {
    std::string choice;
};

#endif // COMMONDATATYPES_H
