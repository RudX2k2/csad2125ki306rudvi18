#ifndef UARTTXRX_H
#define UARTTXRX_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <QTimer>

class UartTxRx : public QThread
{
    Q_OBJECT

private:
    static UartTxRx* uartTxRx_;
    QSerialPort serial;
    QTimer readTimer;
    QByteArray rx_data;
    QByteArray rxBuffer;
    QString port_name;
    int emptyReadsCount;

    // Prevent copying
    UartTxRx(UartTxRx &other) = delete;
    void operator=(const UartTxRx &) = delete;

    // Private methods for handling chunked data
    void processCompleteMessage();
    void resetBuffer();

public:
    // Constants for buffer management
    static const int RX_BUF_SIZE = 4096;
    static const int CHUNK_SIZE = 1024;

    // Constructors
    explicit UartTxRx(const QString &portName);
    UartTxRx();

    // Singleton access
    static UartTxRx *GetInstance();

    // Connection management
    int InitConnection(const QString &portName);
    int Disconnect();
    bool isConnected();

signals:
    // Signal for partial data updates
    void bufferChanged(const QByteArray &newBuffer);
    // Signal for complete message received
    void completeMessageReceived(const QByteArray &completeData);
    // Signal for disconnection
    void disconnected();

public slots:
    // Data handling slots
    void handleReadyRead();
    void checkForMoreData();
    int sendMessage(const QByteArray &message);
};

#endif // UARTTXRX_H
