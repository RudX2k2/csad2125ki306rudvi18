#include "UartTxRx.h"
#include <QDebug>

UartTxRx* UartTxRx::uartTxRx_ = nullptr;


// Constructor: initializes the serial port
UartTxRx::UartTxRx(const QString &portName)
{
    UartTxRx::InitConnection(portName);
}


UartTxRx::UartTxRx()
{

}


int UartTxRx::InitConnection(const QString &portName)
{
    serial.setPortName(portName);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.open(QIODevice::ReadWrite)) {  // Open for both read and write
        qDebug() << "Failed to open port" << portName;
        return -1;
    } else {
        connect(&serial, &QSerialPort::readyRead, this, &UartTxRx::handleReadyRead);
        return 0;
    }
}


UartTxRx* UartTxRx::GetInstance()
{
    if (uartTxRx_ == nullptr)
    {
        uartTxRx_ = new UartTxRx();
    }
    return uartTxRx_;
}



// Slot: Handles incoming data when available
void UartTxRx::handleReadyRead()
{
    QByteArray data = serial.readAll();
    qDebug() << "Data received:" << data;
    rx_data = data;

    emit bufferChanged(rx_data);
}

// Method: Sends data over UART
int UartTxRx::sendMessage(const QByteArray &message)
{
    if (serial.isOpen()) {
        serial.write(message);
        if (serial.waitForBytesWritten(1000)) {  // Wait for the data to be written
            qDebug() << "Message sent:" << message;
            serial.clear();
            return 0;
        } else {
            qDebug() << "Failed to send message: " << message;
            serial.clear();
            return -1;
        }
    } else {
        qDebug() << "Serial port is not open!";
        return -1;
    }
}
