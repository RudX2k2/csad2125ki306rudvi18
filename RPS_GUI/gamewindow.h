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
    void setRetrievedGamestate(GameState gamestate);

private slots:
    void on_btnGoBack_clicked();

    void on_btnRock_clicked();

    void on_brnPaper_clicked();

    void on_btnScissors_clicked();

    void on_btnGetInfo_clicked();

private:
    Ui::GameWindow *ui;
    QWidget *mainWindow;
    void GameWindow_ProccessTurnResult(TurnResult turn_result);
};

#endif // GAMEWINDOW_H
