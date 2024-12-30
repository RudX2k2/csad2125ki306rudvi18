#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>
#include "CommonDataTypes.h"

/**
 * @file gamedata.h
 * @brief Declaration of the GameData class for managing game state.
 */

/**
 * @class GameData
 * @brief Singleton class for managing the game's state and related data.
 *
 * The GameData class provides methods for setting and retrieving the game's current state.
 * It uses the QObject system to emit signals when the game state changes.
 */
class GameData : public QObject
{
    Q_OBJECT

private:
    static GameData* instance; ///< Singleton instance.
    explicit GameData(QObject* parent = nullptr); ///< Private constructor for singleton pattern.

    // Prevent copying
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    GameState current_gamestate; ///< Current state of the game.

signals:
    /**
     * @brief Emitted when the game state is updated.
     * @param gamestate The updated game state.
     */
    void GameData_GameStateUpdated(GameState gamestate);

public:
    /**
     * @brief Sets the current game state.
     * @param set_gamestate The game state to set.
     */
    void GameData_SetGameState(GameState set_gamestate);

    /**
     * @brief Returns the singleton instance of GameData.
     * @param parent Optional parent object.
     * @return Pointer to the GameData instance.
     */
    static GameData* getInstance(QObject* parent = nullptr);

    /**
     * @brief Retrieves the current game state.
     * @return The current game state.
     */
    GameState getCurrentGameState();
};

#endif // GAMEDATA_H
