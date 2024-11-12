#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QMessageBox>
#include "UartTxRx.h"
#include <QDebug>

UartTxRx * uart_gamewindow_obj;

GameWindow::GameWindow(QMainWindow* mainWindow, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    mainWindow(mainWindow)  // Initialize the mainWindow pointer
{
    ui->setupUi(this);
    uart_gamewindow_obj = uart_gamewindow_obj->GetInstance();

}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::on_btnGoBack_clicked()
{
    if (mainWindow) {
        mainWindow->show();
    }
    this->close();
}


void GameWindow::GameWindow_SendGameStateRetrieve()
{
    qDebug() << "Ruja mavpa\n";
    // GameState game_state = {.isClientRequest=1};
    // // Use the parser to generate INI message
    // // std::string iniMessage = parser.generateGameStateMessage(game_state);

    // // Send the message via UART
    // int ret = uart_gamewindow_obj->sendMessage(QByteArray::fromStdString(iniMessage));
    // if(ret) {
    //     QMessageBox::critical(this, "Error", "Failed to send game settings to server");
    //     return;
    // }
}


