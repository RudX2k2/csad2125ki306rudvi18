#ifndef UARTTXRX_H
#define UARTTXRX_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <QTimer>

/**
 * @file uarttxrx.h
 * @brief Declaration of the UartTxRx class for UART communication.
 */

/**
 * @class UartTxRx
 * @brief Provides UART communication functionality with support for buffer management.
 *
 * The UartTxRx class implements a singleton design pattern to ensure a single instance
 * for managing UART communication. It includes methods for sending and receiving data
 * as well as handling disconnections.
 */
class UartTxRx : public QThread
{
    Q_OBJECT

private:
    static UartTxRx* uartTxRx_; ///< Singleton instance.
    QSerialPort serial; ///< Serial port for UART communication.
    QTimer readTimer; ///< Timer for handling periodic reads.
    QByteArray rx_data; ///< Incoming data buffer.
    QByteArray rxBuffer; ///< Buffer for storing received data.
    QString port_name; ///< Name of the serial port.
    int emptyReadsCount; ///< Counter for empty reads.

    // Prevent copying
    UartTxRx(UartTxRx &other) = delete;
    void operator=(const UartTxRx &) = delete;

    /**
     * @brief Processes a complete message from the buffer.
     */
    void processCompleteMessage();

    /**
     * @brief Resets the receive buffer.
     */
    void resetBuffer();

public:
    static const int RX_BUF_SIZE = 4096; ///< Size of the receive buffer.
    static const int CHUNK_SIZE = 1024; ///< Size of data chunks for processing.

    /**
     * @brief Constructs a UartTxRx object with a specified port name.
     * @param portName The name of the serial port.
     */
    explicit UartTxRx(const QString &portName);

    /**
     * @brief Constructs a UartTxRx object with no specified port name.
     */
    UartTxRx();

    /**
     * @brief Returns the singleton instance of UartTxRx.
     * @return Pointer to the UartTxRx instance.
     */
    static UartTxRx *GetInstance();

    /**
     * @brief Initializes a connection to the specified serial port.
     * @param portName The name of the port to connect to.
     * @return Status code of the operation.
     */
    int InitConnection(const QString &portName);

    /**
     * @brief Disconnects from the serial port.
     * @return Status code of the operation.
     */
    int Disconnect();

    /**
     * @brief Checks if the UART connection is active.
     * @return True if connected, false otherwise.
     */
    bool isConnected();

signals:
    /**
     * @brief Emitted when the buffer is updated with partial data.
     * @param newBuffer The updated buffer.
     */
    void bufferChanged(const QByteArray &newBuffer);

    /**
     * @brief Emitted when a complete message is received.
     * @param completeData The complete message data.
     */
    void completeMessageReceived(const QByteArray &completeData);

    /**
     * @brief Emitted when the serial port is disconnected.
     */
    void disconnected();

public slots:
    /**
     * @brief Handles data ready for reading from the serial port.
     */
    void handleReadyRead();

    /**
     * @brief Checks for additional data in the buffer.
     */
    void checkForMoreData();

    /**
     * @brief Sends a message via the serial port.
     * @param message The data to be sent.
     * @return Status code of the operation.
     */
    int sendMessage(const QByteArray &message);
};

#endif // UARTTXRX_H
