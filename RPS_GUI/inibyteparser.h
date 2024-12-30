/*!
 * \file IniByteParser.h
 * \brief Header file for the IniByteParser class.
 *
 * This file contains the declaration of the IniByteParser class, which is used
 * for parsing and generating INI-style messages.
 */

#pragma once
#include <map>
#include <string>
#include <vector>
#define SI_NO_CONVERSION
#include "SimpleIni.h"
#include "UartTxRx.h"
#include <QObject>
#include <QMutex>

#include "CommonDataTypes.h"

/*!
 * \brief The IniByteParser class is used for parsing and creating INI messages.
 *
 * This singleton class provides various methods to handle the generation of
 * INI-style messages for game state, player turns, and other game-related data.
 */
class IniByteParser : public QObject {
    Q_OBJECT

public:
    /*!
     * \brief GetInstance Returns the singleton instance of the IniByteParser class.
     * \return A pointer to the singleton instance.
     */
    static IniByteParser* GetInstance() {
        static IniByteParser instance;
        return &instance;
    }

    /*!
     * \brief Destructor for the IniByteParser class.
     */
    ~IniByteParser() = default;

    /*!
     * \brief generateSetGameStateMessage Generates an INI-style message to set the game state.
     * \param state The GameState structure to use for generating the message.
     * \return A string containing the INI-style set message for the game state.
     */
    std::string generateSetGameStateMessage(const GameState& state);

    /*!
     * \brief generateSetPlayerTurn Generates an INI-style message for a player's turn.
     * \param turn The ClientGameTurn structure representing the player's turn.
     * \return A string containing the INI-style message for the player's turn.
     */
    std::string generateSetPlayerTurn(const ClientGameTurn& turn);

    /*!
     * \brief generateGetGameStateMessage Generates a request message to retrieve the current game state.
     * \return A string containing the INI-style request message for the game state.
     */
    std::string generateGetGameStateMessage();

    /*!
     * \brief generateCleanGame Generates a request message to clean the game state on the server.
     * \return A string containing the INI-style clean game request message.
     */
    std::string generateCleanGame();

    // Game constants
    const std::map<std::string, std::string> GameModes = {
        {"PVP", "Player vs Player"},
        {"PVE", "Player vs AI"},
        {"EVE", "AI vs AI"}
    };

    const std::vector<std::string> ValidTurns = {
        "ROCK", "PAPER", "SCISSORS"
    };

signals:
    /*!
     * \brief Emitted when the server sends a good configuration response.
     * \param res The result code.
     */
    void ServerGoodConfig(int res);

    /*!
     * \brief Emitted when the server sends the current game state.
     * \param state The game state.
     */
    void ServerSentGameState(GameState state);

    /*!
     * \brief Emitted when the server sends the turn result.
     * \param turn_result The game state with the turn result.
     */
    void ServerSentTurnResult(GameState turn_result);

    /*!
     * \brief Emitted when the server waits for a player's turn.
     * \param player The player number.
     */
    void ServerWaitTurn(int player);

    /*!
     * \brief Emitted to load the game state to the controller.
     * \param initParse Whether the initialization parsing was successful.
     * \param gamestate The game state.
     * \param message The original message data.
     */
    void LoadGameToController(bool initParse, GameState gamestate, const QByteArray &message);

    /*!
     * \brief Emitted when the server sends a successful clean game response.
     */
    void ServerGoodClean(int result);

public slots:
    /*!
     * \brief Parses INI data received from the server.
     * \param message The message containing the INI data.
     */
    void INIBYTEPARSER_ParseINIData(const QByteArray &message);

private:
    QMutex iniMutex; /*!< Mutex for thread-safe operations on the INI parser. */

    /*!
     * \brief Private constructor for the IniByteParser class.
     */
    IniByteParser();

    /*!
     * \brief Deleted copy constructor to prevent copying of the singleton.
     */
    IniByteParser(const IniByteParser&) = delete;

    /*!
     * \brief Deleted assignment operator to prevent assignment of the singleton.
     */
    IniByteParser& operator=(const IniByteParser&) = delete;

    /*!
     * \brief Parses data from a string in INI format.
     * \param data The string containing the INI data.
     * \return true if parsing was successful; false otherwise.
     */
    bool parseFromString(const std::string& data);

    /*!
     * \brief Retrieves a value from the INI data.
     * \param section The section name in the INI data.
     * \param key The key name in the section.
     * \param defaultValue The default value if the key is not found.
     * \return The retrieved value as a string.
     */
    std::string getValue(const std::string& section, const std::string& key, const std::string& defaultValue = "");

    /*!
     * \brief Retrieves an integer value from the INI data.
     * \param section The section name in the INI data.
     * \param key The key name in the section.
     * \param defaultValue The default value if the key is not found.
     * \return The retrieved value as an integer.
     */
    int getValueInt(const std::string& section, const std::string& key, int defaultValue = 0);

    CSimpleIniA ini; /*!< Instance of the CSimpleIniA parser. */
    UartTxRx* uart_ini_obj; /*!< Pointer to the UART transmission/reception object. */
};
