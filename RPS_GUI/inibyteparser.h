#pragma once
#include <map>
#include <string>
#include <vector>
#define SI_NO_CONVERSION
#include "SimpleIni.h"
#include "UartTxRx.h"
#include <QObject>

#include "CommonDataTypes.h"



class IniByteParser : public QObject {
    Q_OBJECT

public:
    static IniByteParser* GetInstance() {
        static IniByteParser instance;
        return &instance;
    }

    ~IniByteParser() = default;

    // Game constants
    const std::map<std::string, std::string> GameModes = {
        {"PVP", "Player vs Player"},
        {"PVE", "Player vs AI"},
        {"EVE", "AI vs AI"}
    };

    const std::vector<std::string> ValidTurns = {
        "ROCK", "PAPER", "SCISSORS"
    };

    // Parsing methods
    std::string generateSetGameStateMessage(const GameState& state);
    std::string generateSetPlayerTurn(const ClientGameTurn& turn);
    GameState parseGameState(const std::string& iniData);
    std::string generateGetGameStateMessage();
signals:
    void ServerGoodConfig();
    void ServerSentGameState(GameState state);
    void ServerSentTurnResult(TurnResult turn_result);
    void ServerWaitTurn();

public slots:
    void INIBYTEPARSER_ParseINIData(const QByteArray &message);

private:
    IniByteParser();
    IniByteParser(const IniByteParser&) = delete;
    IniByteParser& operator=(const IniByteParser&) = delete;

    bool parseFromString(const std::string& data);
    std::string getValue(const std::string& section, const std::string& key, const std::string& defaultValue = "");
    int getValueInt(const std::string& section, const std::string& key, int defaultValue = 0);

    CSimpleIniA ini;
    UartTxRx* uart_ini_obj;
};
