#include "testuarttxrx.h"
#include "../UartTxRx.h"

void TestUartTxRx::testSendMessage()
{
    UartTxRx uart;
    QByteArray message = "Hello, ESP32";
    int result = uart.sendMessage(message);

    // Verify that the message was sent successfully
    QVERIFY(result == 0);
    QCOMPARE(result, 0);   // Provides detailed output in case of failure
}

void TestUartTxRx::testConnection()
{
    UartTxRx uart;
    QString portName = "/dev/ttyACM1";
    int result = uart.InitConnection(portName);

    // Verify that the connection was initialized successfully
    QVERIFY(result == 0);
    QCOMPARE(result, 0);  // Provides detailed output if the test fails
}

QTEST_MAIN(TestUartTxRx)
