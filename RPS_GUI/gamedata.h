// gamedata.h
#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>

#include "CommonDataTypes.h"

class GameData : public QObject {
    Q_OBJECT
private:
    static GameData* instance;
    explicit GameData(QObject* parent = nullptr);  // explicit constructor
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    GameState current_gamestate;

signals:


public:
    static void GameData_SetGameState(GameState set_gamestate);
    static GameData* getInstance(QObject* parent = nullptr);
    GameState getCurrentGameState();
    void setCurrentGameState(GameState gamestate);
    // Add your game data methods and properties here
};


#endif // GAMEDATA_H
