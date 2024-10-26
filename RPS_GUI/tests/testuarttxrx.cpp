#include "testuarttxrx.h"
#include "../UartTxRx.h"

void TestUartTxRx::testSendMessage()
{
    try {
        UartTxRx uart;
        QByteArray message = "Hello, ESP32";
        qDebug() << "Attempting to send message:" << message;
        int result = uart.sendMessage(message);
        qDebug() << "Send message result:" << result;

        // In CI environment, we expect -1 because the port isn't open
        QCOMPARE(result, -1);
    } catch (const std::exception& e) {
        QFAIL(qPrintable(QString("Exception caught in testSendMessage: %1").arg(e.what())));
    } catch (...) {
        QFAIL("Unknown exception caught in testSendMessage");
    }
}

void TestUartTxRx::testConnection()
{
    try {
        UartTxRx uart;
        QString portName = "/dev/ttyACM1";
        qDebug() << "Attempting to initialize connection on port:" << portName;
        int result = uart.InitConnection(portName);
        qDebug() << "Initialize connection result:" << result;

        // In CI environment, we expect -1 because the port isn't available
        QCOMPARE(result, -1);
    } catch (const std::exception& e) {
        QFAIL(qPrintable(QString("Exception caught in testConnection: %1").arg(e.what())));
    } catch (...) {
        QFAIL("Unknown exception caught in testConnection");
    }
}


QTEST_MAIN(TestUartTxRx)
