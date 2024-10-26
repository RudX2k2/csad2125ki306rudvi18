#ifndef UARTTXRX_H
#define UARTTXRX_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QThread>

class UartTxRx : public QThread
{
    Q_OBJECT

private:
    QSerialPort serial;
    static UartTxRx* uartTxRx_;
    QByteArray rx_data;
    QString port_name;


public:
    // Constructor to initialize the serial port with the specified port name
    explicit UartTxRx(const QString &portName);

    UartTxRx();

    UartTxRx(UartTxRx &other) = delete;

    void operator=(const UartTxRx &) = delete;


    int InitConnection(const QString &portName);

    int Disconnect();

    static UartTxRx *GetInstance();

    bool isConnected();

signals:
    void bufferChanged(const QByteArray &newBuffer);  // Signal to notify buffer change
    void disconnected();

public slots:
    // Slot to handle receiving data
    void handleReadyRead();

    // Method to send data over UART
    int sendMessage(const QByteArray &message);

};

#endif // UARTTXRX_H
