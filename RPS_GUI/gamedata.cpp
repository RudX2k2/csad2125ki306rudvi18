#include "gamedata.h"
#include "inibyteparser.h"

#include "CommonDataTypes.h"


// Make instance definition local to this translation unit
GameData* GameData::instance = nullptr;

GameData::GameData(QObject* parent) : QObject(parent) {
    // Initialize your game data here
    connect(IniByteParser::GetInstance(), &IniByteParser::ServerSentGameState, this, &GameData::setCurrentGameState);

}

void GameData::GameData_SetGameState(GameState set_gamestate)
{
    getInstance()->current_gamestate = set_gamestate;
}

GameData* GameData::getInstance(QObject* parent) {
    if (!instance) {
        instance = new GameData(parent);
    }
    return instance;
}

GameState GameData::getCurrentGameState()
{
    return current_gamestate;
}
void GameData::setCurrentGameState(GameState gamestate){
    current_gamestate = gamestate;
}
