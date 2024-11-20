#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "inibyteparser.h"

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(QMainWindow* mainWindow, QWidget *parent = nullptr);  // Modified constructor
    ~GameWindow();
    void GameWindow_SendTurn(std::string turn_string);
    void GameWindow_UpdateStatusLabel(GameState gamestate);

private slots:
    void on_btnGoBack_clicked();

    void on_btnRock_clicked();

    void on_btnPaper_clicked();

    void on_btnScissors_clicked();

    void on_btnGetInfo_clicked();

private:
    Ui::GameWindow *ui;
    QWidget *mainWindow;
    GameState current_game;
    void GameWindow_ProccessTurnResult(GameState turn_result);
    void GameWindow_LetEnterTurn(int player);
};

#endif // GAMEWINDOW_H
