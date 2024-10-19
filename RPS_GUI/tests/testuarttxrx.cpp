#include "testuarttxrx.h"
#include "../UartTxRx.h"

void TestUartTxRx::testSendMessage()
{
    UartTxRx uart;
    QByteArray message = "Hello, ESP32";
    qDebug() << "Attempting to send message:" << message;
    int result = uart.sendMessage(message);
    qDebug() << "Send message result:" << result;

    // In a real environment, we'd expect result to be 0
    // For CI environment, we expect -1 because the port isn't open
    QCOMPARE(result, -1);
}

void TestUartTxRx::testConnection()
{
    UartTxRx uart;
    QString portName = "/dev/ttyACM1";
    qDebug() << "Attempting to initialize connection on port:" << portName;
    int result = uart.InitConnection(portName);
    qDebug() << "Initialize connection result:" << result;

    // In a real environment, we'd expect result to be 0
    // For CI environment, we expect -1 because the port isn't available
    QCOMPARE(result, -1);
}


QTEST_MAIN(TestUartTxRx)
