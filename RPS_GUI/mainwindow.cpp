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
    ui->setupUi(this);

    uart_obj = UartTxRx::GetInstance();
    connect(uart_obj, &UartTxRx::completeMessageReceived    , this, &MainWindow::updateTerminal);
    connect(uart_obj, &UartTxRx::disconnected, this, &MainWindow::writeDisconnectedInTerminal);
    connect(IniByteParser::GetInstance(), &IniByteParser::ServerGoodConfig, this, &MainWindow::readyToGame);

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
        .maxRoundsAmount = ui->cBoxRoundAmount->currentText().toInt(),
    };

    // Use the parser to generate INI message
    std::string iniMessage = IniByteParser::GetInstance()->generateSetGameStateMessage(game_state);

    // Send the message via UART
    int ret = uart_obj->sendMessage(QByteArray::fromStdString(iniMessage));
    if(ret) {
        QMessageBox::critical(this, "Error", "Failed to send game settings to server");
        return;
    }
}

void MainWindow::readyToGame(){
    // Create and show game window only if message was sent successfully
    if (!gameWindow) {
        gameWindow = new GameWindow(this, nullptr);
        gameWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(gameWindow, &GameWindow::destroyed, [this]() {
            gameWindow = nullptr;
        });
    }

    // Hide main window and show game window
    qDebug() << "ready to game\n";
    this->hide();
    gameWindow->show();
}

void MainWindow::on_btnLoadGame_clicked()
{

}

