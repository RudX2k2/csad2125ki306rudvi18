#ifndef COMMONDATATYPES_H
#define COMMONDATATYPES_H

#include <string>

struct GameState {
    int isLoaded;
    std::string mode;
    int player1Score;
    int player2Score;
    int maxRoundsAmount;
    int winner;
};

struct ClientGameTurn {
    std::string choice;
};

#endif // COMMONDATATYPES_H
