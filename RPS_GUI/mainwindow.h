#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cstring>
#include "gamewindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @file mainwindow.h
 * @brief Declaration of the MainWindow class for the main application interface.
 */

/**
 * @class MainWindow
 * @brief Represents the main application window for managing the game interface.
 *
 * The MainWindow class provides methods and slots for user interactions, game state management,
 * and transitioning to the GameWindow.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a MainWindow object.
     * @param parent The parent widget (optional).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the MainWindow object.
     */
    ~MainWindow();

private slots:
    void on_btnConnect_clicked();
    void on_btnSendHost_clicked();
    void updateTerminal(const QByteArray &newBuffer);
    void on_btnRefreshPorts_clicked();
    void writeDisconnectedInTerminal();
    void on_btnDisconnected_clicked();
    void on_btnGoPlay_clicked();
    void readyToGame(int configRes);
    void on_btnLoadGame_clicked();
    void readyToGameLoaded(bool res, GameState result_gamestate, const QByteArray &messag);
    void on_btnCleanGame_clicked();

private:
    Ui::MainWindow *ui; ///< Pointer to the UI for this window.
    GameWindow *gameWindow; ///< Pointer to the game window.
};

#endif // MAINWINDOW_H
