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
    void GameWindow_SendGameStateRetrieve();

private slots:
    void on_btnGoBack_clicked();

private:
    Ui::GameWindow *ui;
    QWidget *mainWindow;
};

#endif // GAMEWINDOW_H
