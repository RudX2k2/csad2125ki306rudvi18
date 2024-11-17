#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QMessageBox>
#include "UartTxRx.h"
#include <QDebug>
#include "gamedata.h"


GameWindow::GameWindow(QMainWindow* mainWindow, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    mainWindow(mainWindow)  // Initialize the mainWindow pointer
{
    connect(IniByteParser::GetInstance(), &IniByteParser::ServerSentGameState, this, &GameWindow::setRetrievedGamestate);
    connect(IniByteParser::GetInstance(), &IniByteParser::ServerSentTurnResult, this, &GameWindow::GameWindow_ProccessTurnResult);

    std::string send_get_gamestate = IniByteParser::GetInstance()->generateGetGameStateMessage();

    UartTxRx::GetInstance()->sendMessage(QByteArray::fromStdString(send_get_gamestate));
    ui->setupUi(this);

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


void GameWindow::GameWindow_SendTurn(std::string turn_string)
{
    ClientGameTurn turn = {.choice = turn_string};
    std::string result = IniByteParser::GetInstance()->generateSetPlayerTurn(turn);

    UartTxRx::GetInstance()->sendMessage(QByteArray::fromStdString(result));
}


void GameWindow::on_btnRock_clicked()
{
    GameWindow_SendTurn("ROCK");
}


void GameWindow::on_brnPaper_clicked()
{
    GameWindow_SendTurn("PAPER");
}


void GameWindow::on_btnScissors_clicked()
{
    GameWindow_SendTurn("SCISSORS");
}


void GameWindow::setRetrievedGamestate(GameState gamestate)
{
    QString gamestate_text = QString("Player 1 score: %1\n"
                                     "Player 2 score: %2\n"
                                     "Max rounds: %3\n")
                                 .arg(gamestate.player1Score)
                                 .arg(gamestate.player2Score)
                                 .arg(gamestate.maxRoundsAmount);

    GameData::GameData_SetGameState(gamestate);
    ui->lblGameInfo->setText(gamestate_text);
}



void GameWindow::on_btnGetInfo_clicked()
{
    std::string ss = IniByteParser::GetInstance()->generateGetGameStateMessage();
    UartTxRx::GetInstance()->sendMessage(QByteArray::fromStdString(ss));
}

void GameWindow::GameWindow_ProccessTurnResult(TurnResult turn_result)
{
    if(turn_result.winner == 0)
    {
        // Someone had won
        QMessageBox msgBox;

        // Set the message box title and text
        msgBox.setWindowTitle("Make turn again!");
        msgBox.setText("Yes Yes");
        msgBox.setInformativeText("Click yes to yes!");

        // Add a button to the message box
        QPushButton *button = msgBox.addButton("Yes!", QMessageBox::AcceptRole);

        // Show the message box
        msgBox.exec();
    }
    else if(turn_result.winner == 1 || turn_result.winner == 2)
    {
        // Someone had won
        QMessageBox msgBox;

        // Set the message box title and text
        msgBox.setWindowTitle("Winner!");
        msgBox.setText("Player 1 or 2 won");
        msgBox.setInformativeText("Click to back to main menu");

        // Add a button to the message box
        QPushButton *button = msgBox.addButton("Great!", QMessageBox::AcceptRole);

        // Show the message box
        msgBox.exec();

    }
    else if(turn_result.winner == 3){
        // Draw!
        // Someone had won
        QMessageBox msgBox;

        // Set the message box title and text
        msgBox.setWindowTitle("Draw!");
        msgBox.setText("No one won");
        msgBox.setInformativeText("Click to back to main menu");

        // Add a button to the message box
        QPushButton *button = msgBox.addButton("Peace, bublegum", QMessageBox::AcceptRole);

        // Show the message box
        msgBox.exec();
    }

    GameState gamestate = {.player1Score = turn_result.player1Score,
                           .player2Score = turn_result.player2Score,
                           .maxRoundsAmount = turn_result.maxRoundsAmount,
                           .winner = 0};
    setRetrievedGamestate(gamestate);
}
