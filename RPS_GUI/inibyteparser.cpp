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


std::string IniByteParser::generateCleanGame() {
    std::stringstream ss;
    ss << "[CleanGame]\n"
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


void IniByteParser::INIBYTEPARSER_ParseINIData(const QByteArray &message){
    parseFromString(message.toStdString());

    // Parse the INI data
    std::string mode, player1, player2, maxRounds, gameMode, isLoaded, player, turn;
    int retrieve, playerTurn;

    if(ini.GetSectionSize("GetConfigResult") > 0)
    {
        int getConfigResult_serverIncluded = (QString::fromUtf8(ini.GetValue("GetConfigResult", "Server", "0"))).toInt();
        if(getConfigResult_serverIncluded == 1)
        {
            int getConfigResult_result = (QString::fromUtf8(ini.GetValue("GetConfigResult", "Result", "99"))).toInt();

            if(getConfigResult_result == 1 || getConfigResult_result == 0)
            {
                emit ServerGoodConfig(getConfigResult_result);
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
    if(ini.GetSectionSize("SetGameConfig")>0)
    {
        int isLoaded = (QString::fromUtf8(ini.GetValue("SetGameConfig", "IsLoaded", 0))).toInt();

        GameState result_gamestate;
        result_gamestate.mode = ini.GetValue("SetGameConfig","Mode", 0);
        result_gamestate.player1Score = (QString::fromUtf8(ini.GetValue("SetGameConfig","Player1", 0))).toInt();
        result_gamestate.player2Score = (QString::fromUtf8(ini.GetValue("SetGameConfig","Player2", 0))).toInt();
        result_gamestate.maxRoundsAmount = (QString::fromUtf8(ini.GetValue("SetGameConfig","MaxRounds", 0))).toInt();
        result_gamestate.curRound = (QString::fromUtf8(ini.GetValue("SetGameConfig","CurrentRound", 0))).toInt();


        if(isLoaded == 1)
        {
            qDebug("Set game loaded!");
            emit LoadGameToController(true, result_gamestate, message);
        }
        else{
            qDebug("Not set game loaded!");

            emit LoadGameToController(false, result_gamestate, message);
        }
    }
    if(ini.GetSectionSize("CleanResult") > 0)
    {
        int getCleanResult_serverIncluded = (QString::fromUtf8(ini.GetValue("CleanResult", "Server", "0"))).toInt();
        if(getCleanResult_serverIncluded == 1)
        {
            int getCleanResult_result = (QString::fromUtf8(ini.GetValue("CleanResult", "Result", "99"))).toInt();

            if(getCleanResult_result == 1)
            {
                qDebug() << "Good clean result";
                emit ServerGoodClean();
            }
        }
    }
    ini.Reset();
}
