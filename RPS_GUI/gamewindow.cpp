#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QMessageBox>
#include "UartTxRx.h"
#include <QDebug>
#include "gamedata.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>


GameWindow::GameWindow(QMainWindow* mainWindow, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    mainWindow(mainWindow)  // Initialize the mainWindow pointer
{
    // std::string ss = IniByteParser::GetInstance()->generateGetGameStateMessage();
    // UartTxRx::GetInstance()->sendMessage(QByteArray::fromStdString(ss));


    connect(IniByteParser::GetInstance(), &IniByteParser::ServerWaitTurn, this, &GameWindow::GameWindow_LetEnterTurn);
    // connect(IniByteParser::GetInstance(), &IniByteParser::ServerSentGameState, this, &GameWindow::GameWindow_UpdateStatusLabel);
    connect(IniByteParser::GetInstance(), &IniByteParser::ServerSentTurnResult, this, &GameWindow::GameWindow_ProccessTurnResult);
    connect(GameData::getInstance(), &GameData::GameData_GameStateUpdated, this, &GameWindow::GameWindow_UpdateStatusLabel);
    ui->setupUi(this);

    GameWindow_UpdateStatusLabel(GameData::getInstance()->getCurrentGameState());
    // ui->btnRock->setDisabled(true);
    // ui->btnScissors->setDisabled(true);
    // ui->btnPaper->setDisabled(true);
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
    ui->btnRock->setDisabled(true);
    ui->btnScissors->setDisabled(true);
    ui->btnPaper->setDisabled(true);
}


void GameWindow::on_btnPaper_clicked()
{
    GameWindow_SendTurn("PAPER");
    ui->btnRock->setDisabled(true);
    ui->btnScissors->setDisabled(true);
    ui->btnPaper->setDisabled(true);
}


void GameWindow::on_btnScissors_clicked()
{
    GameWindow_SendTurn("SCISSORS");
    ui->btnRock->setDisabled(true);
    ui->btnScissors->setDisabled(true);
    ui->btnPaper->setDisabled(true);
}


void GameWindow::GameWindow_UpdateStatusLabel(GameState gamestate)
{
    qDebug() << "P1 choice: " << QString::fromStdString(gamestate.choiceP1);
    qDebug() << "P2 choice: " << QString::fromStdString(gamestate.choiceP2);

    QString winner_string;
    if(gamestate.winner == 1)
    {winner_string = "Player 1";}
    else if(gamestate.winner == 2){
        winner_string = "Player 1";
    }
    else if(gamestate.winner == 3)
    {
        winner_string = "Draw";
    }
    else{
        winner_string = "...";
    }


    QString gamestate_text = QString("Mode: %1\n"
                                     "Player 1 score: %2\n"
                                     "Player 2 score: %3\n"
                                     "Current round: %4\n"
                                     "Max rounds: %5\n"
                                     "Player 1 choice: %6\n"
                                     "Player 2 choice: %7\n"
                                     "Winner is Player %8")
                                 .arg(QString::fromStdString(gamestate.mode))  // Convert std::string to QString
                                 .arg(gamestate.player1Score)
                                 .arg(gamestate.player2Score)
                                 .arg(gamestate.curRound)
                                 .arg(gamestate.maxRoundsAmount)
                                 .arg(QString::fromStdString(gamestate.choiceP1))
                                 .arg(QString::fromStdString(gamestate.choiceP2))
                                 .arg(winner_string);

    ui->lblGameInfo->setText(gamestate_text);
}


void GameWindow::on_btnGetInfo_clicked()
{
    // std::string ss = IniByteParser::GetInstance()->generateGetGameStateMessage();
    // UartTxRx::GetInstance()->sendMessage(QByteArray::fromStdString(ss));

    GameWindow_UpdateStatusLabel(GameData::getInstance()->getCurrentGameState());
}

void GameWindow::GameWindow_ProccessTurnResult(GameState turn_result)
{
    qDebug() << "Proccess turn result!";
    // Update gamestate
    GameData::getInstance()->GameData_SetGameState(turn_result);
    GameWindow_UpdateStatusLabel(turn_result);

    GameState cur_gamestate = GameData::getInstance()->getCurrentGameState();
    qDebug() << "turn_result("
             << "isLoaded=" << turn_result.isLoaded << ", "
             << "mode=" << QString::fromStdString(turn_result.mode) << ", "
             << "player1Score=" << turn_result.player1Score << ", "
             << "player2Score=" << turn_result.player2Score << ", "
             << "maxRoundsAmount=" << turn_result.maxRoundsAmount << ", "
             << "winner=" << turn_result.winner
             << ')';
    QString insert_text;

    if(cur_gamestate.mode == "PVP")
    {
        if(ui->lblCurrentPlayer->text() == "PLAYER 1")
        {
            insert_text = QString("PLAYER 2");
        }
        else if(ui->lblCurrentPlayer->text() == "PLAYER 2"){
            insert_text = QString("PLAYER 1");
        }

        ui->lblCurrentPlayer->setText(insert_text);
        ui->btnRock->setDisabled(false);
        ui->btnScissors->setDisabled(false);
        ui->btnPaper->setDisabled(false);
    }
    else if(cur_gamestate.mode == "EVE")
    {
        insert_text = QString("BOTS");
        ui->lblCurrentPlayer->setText(insert_text);
    }
    else{
        ui->btnRock->setDisabled(false);
        ui->btnScissors->setDisabled(false);
        ui->btnPaper->setDisabled(false);
    }

    if(turn_result.winner == 0)
    {

    }
    else if(turn_result.winner == 1 || turn_result.winner == 2)
    {
        // Someone had won
        QMessageBox msgBox;

        // Set the message box title and text
        msgBox.setWindowTitle("Winner!");
        if(turn_result.winner == 1)
        {
            msgBox.setText("Player 1 won!");
        }
        else{
            msgBox.setText("Player 2 won!");
        }
        msgBox.setInformativeText("Click to back to main menu");

        // Add a button to the message box
        QPushButton *button = msgBox.addButton("Congrats!", QMessageBox::AcceptRole);

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
}


void GameWindow::GameWindow_LetEnterTurn(int player)
{
    QString insert_text;


    GameState local_waitturn_gamedata = GameData::getInstance()->getCurrentGameState();
    // qDebug() << "GameState {"
    //          << "\n  isLoaded: " << local_waitturn_gamedata.isLoaded
    //          << "\n  mode: " << QString::fromStdString(local_waitturn_gamedata.mode)
    //          << "\n  player1Score: " << local_waitturn_gamedata.player1Score
    //          << "\n  player2Score: " << local_waitturn_gamedata.player2Score
    //          << "\n  maxRoundsAmount: " << local_waitturn_gamedata.maxRoundsAmount
    //          << "\n  winner: " << local_waitturn_gamedata.winner
    //          << "\n}";

    if(local_waitturn_gamedata.mode == "PVP")
    {
        if(ui->lblCurrentPlayer->text() == "PLAYER 1")
        {
            insert_text = QString("PLAYER 2");
        }
        else if(ui->lblCurrentPlayer->text() == "PLAYER 2"){
            insert_text = QString("PLAYER 1");
        }

        ui->lblCurrentPlayer->setText(insert_text);
        ui->btnRock->setDisabled(false);
        ui->btnScissors->setDisabled(false);
        ui->btnPaper->setDisabled(false);
    }
    else if(local_waitturn_gamedata.mode == "EVE")
    {

    }
    else{
        ui->btnRock->setDisabled(false);
        ui->btnScissors->setDisabled(false);
        ui->btnPaper->setDisabled(false);
    }
}


void GameWindow::on_btnSaveGame_clicked()
{
    GameState gameState = GameData::getInstance()->getCurrentGameState();
    // Open a file save dialog to select the location to save the file
    QString fileName = QFileDialog::getSaveFileName(this, "Save Game", "", "INI Files (*.ini)");
    if (fileName.isEmpty()) {
        return; // User canceled the dialog
    }

    // Open the file for writing
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Could not open file for writing.");
        return;
    }

    // Write data in .ini format
    QTextStream out(&file);
    out << "[GameState]\n";
    out << "Mode=" << QString::fromStdString(gameState.mode) << "\n";
    out << "IsLoaded=" << 1 << "\n";
    out << "Player1Score=" << gameState.player1Score << "\n";
    out << "Player2Score=" << gameState.player2Score << "\n";
    out << "CurrentRound=" << gameState.curRound << "\n";
    out << "MaxRoundsAmount=" << gameState.maxRoundsAmount << "\n";
    out << "Winner=" << gameState.winner << "\n";
    out << "ChoiceP1=" << QString::fromStdString(gameState.choiceP1) << "\n";
    out << "ChoiceP2=" << QString::fromStdString(gameState.choiceP2) << "\n";

    file.close();

    // Notify the user that the game has been saved
    QMessageBox::information(this, "Save Game", "Game state saved successfully!");
}

