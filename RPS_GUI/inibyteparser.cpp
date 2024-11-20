// inibyteparser.cpp
#include "inibyteparser.h"
#include <sstream>
#include <QDebug>


IniByteParser::IniByteParser() {
    ini.SetUnicode();
    uart_ini_obj = UartTxRx::GetInstance();
    QObject::connect(uart_ini_obj, &UartTxRx::completeMessageReceived, this, &IniByteParser::INIBYTEPARSER_ParseINIData);
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



std::string IniByteParser::generateSetGameStateMessage(const GameState& state) {
    std::stringstream ss;
    ss << "[SetGameConfig]\n"
       << "Client=1\n"
       << "IsLoaded=" << state.isLoaded << "\n\n"
       << "Mode=" << state.mode << "\n"
       << "Player1=" << state.player1Score << "\n"
       << "Player2=" << state.player2Score << "\n"
       << "MaxRounds=" << state.maxRoundsAmount << "\n";
    return ss.str();
}


std::string IniByteParser::generateGetGameStateMessage() {
    std::stringstream ss;
    ss << "[GetGameState]\n"
       << "Client=1\n";
    return ss.str();
}


std::string IniByteParser::generateSetPlayerTurn(const ClientGameTurn& turn) {
    std::stringstream ss;
    ss << "[SetPlayerTurn]\n"
       << "Client=1\n"
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
    // QMutexLocker locker(&iniMutex); // Lock the mutex for the duration of this function
    // qDebug() << "INI parse:\n" << QString(message) << "\n";
    parseFromString(message.toStdString());
    // CSimpleIniA::TNamesDepend sections;
    // ini.GetAllSections(sections);
    // for (const auto& section : sections) {
    //     qDebug() << "[Section:" << section.pItem << "]";

    //     // Iterate through all keys in the section
    //     CSimpleIniA::TNamesDepend keys;
    //     ini.GetAllKeys(section.pItem, keys);
    //     for (const auto& key : keys) {
    //         const char* value = ini.GetValue(section.pItem, key.pItem, "Default");
    //         qDebug() << " " << key.pItem << "=" << value;
    //     }
    // }

    // if (ini.GetSectionSize("GetTurnResult") > 0) {
    //     qDebug() << "Section 'GetTurnResult' found";
    //     qDebug() << "Server:" << QString::fromUtf8(ini.GetValue("GetTurnResult", "Server", 0));
    // } else {
    //     qDebug() << "Section 'GetTurnResult' not found in message.";
    // }

    // Parse the INI data
    std::string mode, player1, player2, maxRounds, gameMode, isLoaded, player, turn;
    int retrieve, playerTurn;

    if(ini.GetSectionSize("GetConfigResult") > 0)
    {
        int getConfigResult_serverIncluded = (QString::fromUtf8(ini.GetValue("GetConfigResult", "Server", "0"))).toInt();
        if(getConfigResult_serverIncluded == 1)
        {
            int getConfigResult_result = (QString::fromUtf8(ini.GetValue("GetConfigResult", "Result", "0"))).toInt();

            if(getConfigResult_result == 1)
            {
                emit ServerGoodConfig();
            }
        }
    }
    if(ini.GetSectionSize("GetGameState") > 0)
    {
        int isServerMsg = (QString::fromUtf8(ini.GetValue("GetGameState","Server", 0))).toInt();
        if(isServerMsg == 1)
        {
            GameState result_gamestate;
            result_gamestate.player1Score = (QString::fromUtf8(ini.GetValue("GetGameState","Player1", 0))).toInt();
            result_gamestate.player2Score = (QString::fromUtf8(ini.GetValue("GetGameState","Player2", 0))).toInt();
            result_gamestate.mode = ini.GetValue("GetGameState","Mode", 0);
            result_gamestate.maxRoundsAmount = (QString::fromUtf8(ini.GetValue("GetGameState","MaxRounds", 0))).toInt();
            result_gamestate.winner = (QString::fromUtf8(ini.GetValue("GetGameState","Winner", 0))).toInt();
            emit ServerSentGameState(result_gamestate);
        }
    }
    if(ini.GetSectionSize("GetTurnResult") > 0)
    {
        qDebug() << "TURN RESULT";
        int isServerMsg = (QString::fromUtf8(ini.GetValue("GetTurnResult","Server", 0))).toInt();
        if(isServerMsg == 1)
        {
            GameState get_turnresult;
            get_turnresult.player1Score = (QString::fromUtf8(ini.GetValue("GetTurnResult","Player1", 0))).toInt();
            get_turnresult.player2Score = (QString::fromUtf8(ini.GetValue("GetTurnResult","Player2", 0))).toInt();
            get_turnresult.mode = ini.GetValue("GetTurnResult","Mode", "");
            get_turnresult.maxRoundsAmount = (QString::fromUtf8(ini.GetValue("GetTurnResult","MaxRounds", 0))).toInt();
            get_turnresult.winner = (QString::fromUtf8(ini.GetValue("GetTurnResult","Winner", 0))).toInt();
            get_turnresult.curRound = (QString::fromUtf8(ini.GetValue("GetTurnResult","CurrentRound", 0))).toInt();
            get_turnresult.choiceP1 = ini.GetValue("GetTurnResult", "ChoiceP1", ""); // Corrected key name
            get_turnresult.choiceP2 = ini.GetValue("GetTurnResult", "ChoiceP2", ""); // Corrected key name
            get_turnresult.isLoaded = 99;
            emit ServerSentTurnResult(get_turnresult);
        }
    }
    if (ini.GetSectionSize("WaitClientTurn") > 0)
    {
        int isServerMsg = (QString::fromUtf8(ini.GetValue("WaitClientTurn", "Server", 0))).toInt();
        if (isServerMsg == 1)
        {
            int player_to_wait_turn = (QString::fromUtf8(ini.GetValue("WaitClientTurn", "Player", 0))).toInt();
            qDebug() << "Emit server wait turn";
            emit ServerWaitTurn(player_to_wait_turn);
        }
    }
    ini.Reset();
}
