#include "testuarttxrx.h"

void TestUartTxRx::testSendMessage()
{
    UartTxRx uart;
    QByteArray message = "Hello, ESP32";
    int result = uart.sendMessage(message);

    // Verify that the message was sent successfully
    QVERIFY(result == 0);
}

void TestUartTxRx::testConnection()
{
    UartTxRx uart;
    QString portName = "/dev/ttyACM1";
    int result = uart.InitConnection(portName);

    // Verify that the connection was initialized successfully
    QVERIFY(result == 0);
}

QTEST_MAIN(TestUartTxRx)
#include "testuarttxrx.moc"
