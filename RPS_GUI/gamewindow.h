#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "inibyteparser.h"

/**
 * @file gamewindow.h
 * @brief Declaration of the GameWindow class, which manages the game UI.
 */

/**
 * @namespace Ui
 * @brief Contains the UI class for GameWindow.
 */
namespace Ui {
class GameWindow;
}

/**
 * @class GameWindow
 * @brief Represents the main game window for the Rock-Paper-Scissors game.
 *
 * This class manages the game UI and provides methods to handle user interactions,
 * update the game state, and process game turns.
 */
class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a GameWindow object.
     * @param mainWindow A pointer to the main application window.
     * @param parent The parent widget (optional).
     */
    explicit GameWindow(QMainWindow* mainWindow, QWidget *parent = nullptr);

    /**
     * @brief Destroys the GameWindow object.
     */
    ~GameWindow();

    /**
     * @brief Sends the player's turn to the game logic.
     * @param turn_string The string representing the player's turn (e.g., "Rock").
     */
    void GameWindow_SendTurn(std::string turn_string);

    /**
     * @brief Updates the status label based on the current game state.
     * @param gamestate The current state of the game.
     */
    void GameWindow_UpdateStatusLabel(GameState gamestate);

private slots:
    /**
     * @brief Handles the "Go Back" button click event.
     */
    void on_btnGoBack_clicked();

    /**
     * @brief Handles the "Rock" button click event.
     */
    void on_btnRock_clicked();

    /**
     * @brief Handles the "Paper" button click event.
     */
    void on_btnPaper_clicked();

    /**
     * @brief Handles the "Scissors" button click event.
     */
    void on_btnScissors_clicked();

    /**
     * @brief Handles the "Get Info" button click event.
     */
    void on_btnGetInfo_clicked();

    /**
     * @brief Handles the "Save Game" button click event.
     */
    void on_btnSaveGame_clicked();

private:
    Ui::GameWindow *ui; ///< Pointer to the UI for this window.
    QWidget *mainWindow; ///< Pointer to the main application window.
    GameState current_game; ///< The current game state.

    /**
     * @brief Processes the result of a player's turn.
     * @param turn_result The result of the turn.
     */
    void GameWindow_ProccessTurnResult(GameState turn_result);

    /**
     * @brief Allows a player to enter their turn.
     * @param player The ID of the player whose turn it is.
     */
    void GameWindow_LetEnterTurn(int player);
};

#endif // GAMEWINDOW_H
