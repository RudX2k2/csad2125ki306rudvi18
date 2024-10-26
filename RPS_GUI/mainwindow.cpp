#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "UartTxRx.h"
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QLabel>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>

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
{
    ui->setupUi(this);

    uart_obj = UartTxRx::GetInstance();
    connect(uart_obj, &UartTxRx::bufferChanged, this, &MainWindow::updateTerminal);
    connect(uart_obj, &UartTxRx::disconnected, this, &MainWindow::writeDisconnectedInTerminal);

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
}



MainWindow::~MainWindow()
{
    delete ui;
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

