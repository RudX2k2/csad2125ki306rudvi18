// inibyteparser.cpp
#include "inibyteparser.h"
#include <sstream>
#include <QDebug>


IniByteParser::IniByteParser() {
    ini.SetUnicode();
    uart_ini_obj = UartTxRx::GetInstance();
    QObject::connect(uart_ini_obj, &UartTxRx::bufferChanged, this, &IniByteParser::INIBYTEPARSER_ParseINIData);
}

bool IniByteParser::parseFromString(const std::string& data) {
    SI_Error rc = ini.LoadData(data.c_str(), data.length());
    return (rc >= 0);
}

std::string IniByteParser::getValue(const std::string& section, const std::string& key, const std::string& defaultValue) {
    return ini.GetValue(section.c_str(), key.c_str(), defaultValue.c_str());
}

int IniByteParser::getValueInt(const std::string& section, const std::string& key, int defaultValue) {
    const char* value = ini.GetValue(section.c_str(), key.c_str());
    return value ? std::stoi(value) : defaultValue;
}



std::string IniByteParser::generateGameStateMessage(const GameState& state) {
    std::stringstream ss;
    ss << "[GameState]\n"
       << "Client=1\n"
       << "IsLoaded=" << state.isLoaded << "\n\n"
       << "Mode=" << state.mode << "\n"
       << "Player1=" << state.player1Score << "\n"
       << "Player2=" << state.player2Score << "\n"
       << "MaxRounds=" << state.maxRoundsAmount << "\n";
    return ss.str();
}


std::string IniByteParser::generateClientGameTurn(const ClientGameTurn& turn) {
    std::stringstream ss;
    ss << "[PlayerTurn]\n"
       << "Client=1\n"
       << "Player=" << turn.player << "\n"
       << "Turn=" << turn.choice << "\n";
    return ss.str();
}


GameState IniByteParser::parseGameState(const std::string& iniData) {
    parseFromString(iniData);
    GameState state;
    state.mode = getValue("GameState", "Mode", "PVP");
    state.player1Score = getValueInt("GameState", "Player1Score", 0);
    state.player2Score = getValueInt("GameState", "Player2Score", 0);
    state.maxRoundsAmount = getValueInt("GameState", "MaxRoundsAmount", 3);
    return state;
}


void IniByteParser::INIBYTEPARSER_ParseINIData(const QByteArray &message){
    parseFromString(message.toStdString());

    // Parse the INI data
    std::string mode, player1, player2, maxRounds, gameMode, isLoaded, player, turn;
    int retrieve, playerTurn;

    if(ini.GetSectionSize("ClientTurn") > 0)
    {
        qDebug() << "Found ClientTurn!";
    }

    // if (ini.GetSectionSize("Server") > 0) {
    //     qDebug() << "Found server!";
    //     // [GameState] section
    //     retrieve = ini.GetLongValue("GameState", "Retrieve", 0);
    //     mode = ini.GetValue("GameState", "Mode", "");
    //     player1 = ini.GetValue("GameState", "Player1", "");
    //     player2 = ini.GetValue("GameState", "Player2", "");
    //     maxRounds = ini.GetValue("GameState", "MaxRounds", "");

    //     // [PlayerTurn] section
    //     playerTurn = ini.GetLongValue("PlayerTurn", "Player", 0);
    //     turn = ini.GetValue("PlayerTurn", "Turn", "");

    //     qDebug() << "GameState:";
    //     qDebug() << "  Retrieve:" << retrieve;
    //     qDebug() << "  Mode:" << mode.c_str();
    //     qDebug() << "  Player1:" << player1.c_str();
    //     qDebug() << "  Player2:" << player2.c_str();
    //     qDebug() << "  MaxRounds:" << maxRounds.c_str();

    //     qDebug() << "PlayerTurn:";
    //     qDebug() << "  Player:" << playerTurn;
    //     qDebug() << "  Turn:" << turn.c_str();
    // }


}
