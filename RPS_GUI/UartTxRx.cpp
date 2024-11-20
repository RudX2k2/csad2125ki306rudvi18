// UartTxRx.cpp
#include "UartTxRx.h"
#include <QDebug>
#include <QThread>

UartTxRx* UartTxRx::uartTxRx_ = nullptr;

UartTxRx::UartTxRx(const QString &portName)
{
    UartTxRx::InitConnection(portName);
}

UartTxRx::UartTxRx() : emptyReadsCount(0)
{
    // Configure and connect timer for checking additional data
    readTimer.setInterval(20);  // 20ms interval
    connect(&readTimer, &QTimer::timeout, this, &UartTxRx::checkForMoreData);
}

UartTxRx* UartTxRx::GetInstance()
{
    if (uartTxRx_ == nullptr)
    {
        uartTxRx_ = new UartTxRx();
    }
    return uartTxRx_;
}

int UartTxRx::InitConnection(const QString &portName)
{
    serial.setPortName(portName);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open port" << portName;
        return -1;
    } else {
        port_name = portName;
        connect(&serial, &QSerialPort::readyRead, this, &UartTxRx::handleReadyRead);
        return 0;
    }
}

void UartTxRx::handleReadyRead()
{
    QByteArray data = serial.read(CHUNK_SIZE);
    if (data.size() > 0) {
        if (rxBuffer.size() + data.size() < RX_BUF_SIZE) {
            rxBuffer.append(data);
            emit bufferChanged(rxBuffer);  // Emit signal for partial data

            // Start checking for more data
            emptyReadsCount = 0;
            readTimer.start();
        }
    }
}

void UartTxRx::checkForMoreData()
{
    if (serial.bytesAvailable() > 0) {
        QByteArray moreData = serial.read(CHUNK_SIZE);
        if (moreData.size() > 0) {
            if (rxBuffer.size() + moreData.size() < RX_BUF_SIZE) {
                rxBuffer.append(moreData);
                qDebug() << "Complete data received:" << QString(rxBuffer);
                emit bufferChanged(rxBuffer);
                emptyReadsCount = 0;  // Reset counter as we got data
                return;
            }
        }
    }

    emptyReadsCount++;
    if (emptyReadsCount >= 5) {  // No more data after 5 attempts
        readTimer.stop();
        processCompleteMessage();
    }
}

void UartTxRx::processCompleteMessage()
{
    if (rxBuffer.size() > 0) {
        rx_data = rxBuffer;  // Update rx_data to maintain compatibility
        qDebug() << "Emitting completeMessageReceived with data:" << QString(rxBuffer);
        emit completeMessageReceived(rxBuffer);
        resetBuffer();
    }
}

void UartTxRx::resetBuffer()
{
    rxBuffer.clear();
    emptyReadsCount = 0;
}

int UartTxRx::sendMessage(const QByteArray &message)
{
    if (serial.isOpen()) {
        for(int i = 0; i < message.size(); i += 32) {  // Send in 32-byte chunks
            QByteArray chunk = message.mid(i, 32);
            serial.write(chunk);
            serial.flush();
        }
        qDebug() << "Message sent";
        return 0;
    } else {
        qDebug() << "Serial port is not open!";
        return -1;
    }
}

bool UartTxRx::isConnected()
{
    return serial.isOpen();
}

int UartTxRx::Disconnect()
{
    readTimer.stop();  // Stop the timer before disconnecting
    serial.close();
    if(serial.isOpen()) {
        return -1;
    }
    return 0;
}
