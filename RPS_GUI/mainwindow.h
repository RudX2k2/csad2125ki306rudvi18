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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnConnect_clicked();

    void on_btnSendHost_clicked();

    void updateTerminal(const QByteArray &newBuffer);  // Slot to handle buffer changes

    void on_btnRefreshPorts_clicked();

    void writeDisconnectedInTerminal();

    void on_btnDisconnected_clicked();

    void on_btnGoPlay_clicked();

    void readyToGame(int configRes);

    void on_btnLoadGame_clicked();

    void readyToGameLoaded(bool res, GameState result_gamestate, const QByteArray & messag);

private:
    Ui::MainWindow *ui;
    GameWindow *gameWindow;
};
#endif // MAINWINDOW_H
