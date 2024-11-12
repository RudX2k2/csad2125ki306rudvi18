#pragma once
#include <map>
#include <string>
#include <vector>
#define SI_NO_CONVERSION
#include "SimpleIni.h"
#include "UartTxRx.h"
#include <QObject>

struct GameState {
    int isLoaded;
    std::string mode;           // PVP, PVE, EVE
    int player1Score;
    int player2Score;
    int maxRoundsAmount;
};

struct ClientGameTurn {
    int player;
    std::string choice;
};

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
        "Rock", "Paper", "Scissors"
    };

    // Parsing methods
    std::string generateGameStateMessage(const GameState& state);
    std::string generateClientGameTurn(const ClientGameTurn& turn);
    GameState parseGameState(const std::string& iniData);

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
