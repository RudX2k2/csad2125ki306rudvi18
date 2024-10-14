#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "UartTxRx.h"
#include <QDebug>
#include <QMessageBox>

UartTxRx * uart_obj;
QString port_name = "/dev/ttyACM1";


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    uart_obj = UartTxRx::GetInstance();
    connect(uart_obj, &UartTxRx::bufferChanged, this, &MainWindow::updateTerminal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnConnect_clicked()
{
    QMessageBox msgBox;

    int err = uart_obj->InitConnection(port_name);

    if(err)
    {
        qDebug() << "Error to connect\n";
        // msgBox.setText("Error to connect " + port_name);
    }
    else{
        qDebug() << "Connected\n";
        // msgBox.setText("Connected to " + port_name);
    }

    // msgBox.exec();
}


void MainWindow::on_btnSendHost_clicked()
{
    QString send_text(ui->pTxtSendHost->toPlainText());

    int ret = uart_obj->sendMessage(send_text.toUtf8());
    if(ret)
    {
        qDebug() << "Can't send message\n";
    }
    else{
        send_text.prepend("Client: ");
        send_text.append("\n");

        ui->pTxtTerminal->append(send_text);

        qDebug() << "Message sent\n";
    }
}


void MainWindow::updateTerminal(const QByteArray &newBuffer)
{
    // Update the UI element with the new buffer (e.g., a QTextEdit)
    QString text(newBuffer);

    text.prepend("Server: ");
    text.append("\n");
    ui->pTxtTerminal->append(text);  // Update QTextEdit or other UI element
}
