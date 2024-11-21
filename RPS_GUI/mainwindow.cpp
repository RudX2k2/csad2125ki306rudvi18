#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "UartTxRx.h"
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QLabel>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include "gamewindow.h"
#include "inibyteparser.h"
#include "gamedata.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

UartTxRx * uart_obj;
QString port_name;

void MainWindow::writeDisconnectedInTerminal()
{
    qDebug() << "Disconnected";
    ui->pTxtTerminal->append("Disconnected");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gameWindow(nullptr)
{
    // Create get_instance singleton
    GameData::getInstance()->getCurrentGameState();
    uart_obj = UartTxRx::GetInstance();

    ui->setupUi(this);

    connect(uart_obj, &UartTxRx::completeMessageReceived, this, &MainWindow::updateTerminal);
    connect(uart_obj, &UartTxRx::disconnected, this, &MainWindow::writeDisconnectedInTerminal);
    connect(IniByteParser::GetInstance(), &IniByteParser::ServerGoodConfig, this, &MainWindow::readyToGame);
    connect(IniByteParser::GetInstance(), &IniByteParser::LoadGameToController, this, &MainWindow::readyToGameLoaded);

        ui->btnGoPlay->setDisabled(true);

    const auto availablePorts = QSerialPortInfo::availablePorts();

    if (availablePorts.isEmpty()) {
        qDebug() << "No serial ports found!";
        return;
    }

    // Print the list of files
    foreach (const QSerialPortInfo &port, availablePorts) {
        QString devManufacturer = port.manufacturer();
        qDebug() << port.manufacturer();
        if(devManufacturer == "1a86"){
            ui->cBoxPorts->addItem(port.portName());
        }
    }

    for (int i = 1; i <= 5; i++) {
        QString add_item = QString::number(i);
        ui->cBoxRoundAmount->addItem(add_item);
    }

    for(auto it = IniByteParser::GetInstance()->GameModes.begin(); it != IniByteParser::GetInstance()->GameModes.end(); it++)
    {
        ui->cBoxGameMode->addItem(QString::fromStdString(it->first));
    }
}


MainWindow::~MainWindow()
{
    delete ui;
    if (gameWindow) {
        delete gameWindow;
    }
}

void MainWindow::on_btnConnect_clicked()
{
    port_name = ui->cBoxPorts->currentText();
    port_name.prepend("/dev/");
    int err = uart_obj->InitConnection(port_name);
    QMessageBox msgbox_error;

    if(err)
    {
        qDebug() << "Error to connect\n";
        msgbox_error.setText("Error to connect " + port_name);
        msgbox_error.exec();
    }
    else{
        qDebug() << "Connected to the port " << port_name;
        ui->pTxtTerminal->append("Connected\n");
        ui->btnGoPlay->setDisabled(false);
    }
}


void MainWindow::on_btnSendHost_clicked()
{
    QString send_text(ui->pTxtSendHost->toPlainText());

    int ret = uart_obj->sendMessage(send_text.toUtf8());
    if(ret)
    {
        QString err_string = "Can't send message: " + send_text;
        qDebug() << err_string;
        ui->pTxtTerminal->append(err_string);
    }
    else{
        send_text.prepend("Client: ");
        send_text.append("\n");

        ui->pTxtTerminal->append(send_text);

        qDebug() << "Message sent\n";
    }
    ui->pTxtSendHost->clear();
}


void MainWindow::updateTerminal(const QByteArray &newBuffer)
{
    // Update the UI element with the new buffer (e.g., a QTextEdit)
    QString text(newBuffer);

    text.prepend("Server: ");
    text.append("\n");
    ui->pTxtTerminal->append(text);  // Update QTextEdit or other UI element
}

void MainWindow::on_btnRefreshPorts_clicked()
{
    const auto availablePorts = QSerialPortInfo::availablePorts();

    ui->cBoxPorts->clear();

    if (availablePorts.isEmpty()) {
        qDebug() << "No serial ports found!";
        return;
    }

    // Print the list of files
    foreach (const QSerialPortInfo &port, availablePorts) {
        QString devManufacturer = port.manufacturer();
        qDebug() << port.manufacturer();
        if(devManufacturer == "1a86"){
            ui->cBoxPorts->addItem(port.portName());
        }
    }
}


void MainWindow::on_btnDisconnected_clicked()
{
    int err = uart_obj->Disconnect();
    if(err)
    {
        ui->pTxtTerminal->append("Can't disconnect");
    }
    else{
        ui->pTxtTerminal->append("Disconnected");
    }
}


void MainWindow::on_btnGoPlay_clicked()
{
    // Create game settings from UI input
    GameState game_state = {
        .isLoaded = false,  // Since this is a new game
        .mode = ui->cBoxGameMode->currentText().toStdString(),
        .player1Score = 0,
        .player2Score = 0,
        .curRound = 0,
        .maxRoundsAmount = ui->cBoxRoundAmount->currentText().toInt(),
        .winner = 0,
    };

    GameData::getInstance()->GameData_SetGameState(game_state);
    // Use the parser to generate INI message
    std::string iniMessage = IniByteParser::GetInstance()->generateSetGameStateMessage(game_state);

    // Send the message via UART
    int ret = uart_obj->sendMessage(QByteArray::fromStdString(iniMessage));
    if(ret) {
        QMessageBox::critical(this, "Error", "Failed to send game settings to server");
        return;
    }
}

void MainWindow::readyToGameLoaded(bool res, GameState result_gamestate, const QByteArray & message)
{
    if(res)
    {
        GameData::getInstance()->GameData_SetGameState(result_gamestate);
        UartTxRx::GetInstance()->sendMessage(message);
    }
    else{
        // Someone had won
        QMessageBox msgBox;

        // Set the message box title and text
        msgBox.setWindowTitle("Error!");

        msgBox.setInformativeText("cannot config the game");

        // Add a button to the message box
        QPushButton *button = msgBox.addButton("Ok", QMessageBox::AcceptRole);

        // Show the message box
        msgBox.exec();
    }
}


void MainWindow::readyToGame(int configRes){
    if(configRes == 1)
    {
        // Create and show game window only if message was sent successfully
        if (!gameWindow) {
            gameWindow = new GameWindow(this, nullptr);
            gameWindow->setAttribute(Qt::WA_DeleteOnClose);
            connect(gameWindow, &GameWindow::destroyed, [this]() {
                gameWindow = nullptr;
            });
        }
        // Hide main window and show game window
        this->hide();
        gameWindow->show();
    }
    else{
        // Someone had won
        QMessageBox msgBox;

        // Set the message box title and text
        msgBox.setWindowTitle("Error!");

        msgBox.setInformativeText("cannot config the game");

        // Add a button to the message box
        QPushButton *button = msgBox.addButton("Ok", QMessageBox::AcceptRole);

        // Show the message box
        msgBox.exec();

    }



}

void MainWindow::on_btnLoadGame_clicked() {
    // Open a file dialog to select the .ini file
    QString fileName = QFileDialog::getOpenFileName(this, "Load Game", "", "INI Files (*.ini)");
    if (fileName.isEmpty()) {
        return; // User canceled the dialog
    }

    // Open the file for reading
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Could not open file for reading.");
        QMessageBox::warning(this, "Load Game", "Failed to open the selected file.");
        return;
    }

    // Read the entire file content into a QByteArray
    QByteArray fileContent = file.readAll();
    file.close();

    // Pass the data to the parsing function
    IniByteParser::GetInstance()->INIBYTEPARSER_ParseINIData(fileContent);

    // Notify the user of success
    QMessageBox::information(this, "Load Game", "Game state loaded successfully!");
}


