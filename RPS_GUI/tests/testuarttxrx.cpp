#include "testuarttxrx.h"

// TestUartTxRx member function implementations
bool TestUartTxRx::waitForResponse(QSignalSpy& spy, int timeout) {
    return spy.wait(timeout);
}

QByteArray TestUartTxRx::simulateResponse(const QByteArray& message) {
    if (message.contains("[CleanGame]")) {
        return QByteArray("[CleanGameResult]\nSuccess=1\n");
    } else if (message.contains("[SetGameConfig]")) {
        return QByteArray("[GetConfigResult]\nResult=1\n");
    } else if (message.contains("[InvalidConfig]")) {
        return QByteArray("[Error]\nInvalidData=1\n");
    }
    return QByteArray(); // Default empty response
}

void TestUartTxRx::initTestCase() {
    try {
        uart = new UartTxRx(); // Use the real class
        iniParser = IniByteParser::GetInstance();

        QVERIFY(uart->InitConnection("mock-port"));
        qDebug() << "UART initialized successfully";
    } catch (const std::exception& e) {
        QFAIL(qPrintable(QString("Exception in initTestCase: %1").arg(e.what())));
    }
}

void TestUartTxRx::cleanupTestCase() {
    if (uart) {
        uart->Disconnect();
        delete uart;
    }
}

void TestUartTxRx::testUartConnectivity() {
    QVERIFY(uart != nullptr);
    QVERIFY(uart->isConnected());
}

void TestUartTxRx::testSendConfigMessage() {
    QSignalSpy cleanGameOkSpy(uart, &UartTxRx::completeMessageReceived);

    QByteArray cleanGameMessage("[CleanGame]\nClient=1\n");

    // Inject the mock response directly into the signal
    connect(uart, &UartTxRx::sendMessage, this, [=](const QByteArray& message) {
        emit uart->completeMessageReceived(simulateResponse(message));
    });

    uart->sendMessage(cleanGameMessage);

    QVERIFY(waitForResponse(cleanGameOkSpy, 1));

    QByteArray response = cleanGameOkSpy.takeFirst().at(0).toByteArray();
    QVERIFY(response.contains("Success=1"));
}

void TestUartTxRx::testParserSignals() {
    QSignalSpy uartSpy(uart, &UartTxRx::completeMessageReceived);
    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerGoodConfig);

    QByteArray message("[SetGameConfig]\nClient=1\nData1=1\nData2=2\n");

    // Inject the mock response directly into the signal
    connect(uart, &UartTxRx::sendMessage, this, [=](const QByteArray& message) {
        emit uart->completeMessageReceived(simulateResponse(message));
    });

    uart->sendMessage(message);

    QVERIFY(waitForResponse(uartSpy));
    QVERIFY(waitForResponse(parserSpy));

    QList<QVariant> args = parserSpy.takeFirst();
    QCOMPARE(args.at(0).toInt(), 1); // Expecting Result=1
}

void TestUartTxRx::testEndToEndConfigFlow() {
    QSignalSpy uartSpy(uart, &UartTxRx::completeMessageReceived);
    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerGoodConfig);

    QByteArray invalidMessage("[InvalidConfig]\nBadData=1\n");

    // Inject the mock response directly into the signal
    connect(uart, &UartTxRx::sendMessage, this, [=](const QByteArray& message) {
        emit uart->completeMessageReceived(simulateResponse(message));
    });

    uart->sendMessage(invalidMessage);

    if (waitForResponse(uartSpy)) {
        QByteArray response = uartSpy.takeFirst().at(0).toByteArray();
        qDebug() << "Response to invalid message:" << response;
    }

    QByteArray validMessage("[SetGameConfig]\nClient=1\nData1=1\nData2=2\n");
    uart->sendMessage(validMessage);

    QVERIFY(waitForResponse(uartSpy));
    QVERIFY(waitForResponse(parserSpy));

    QByteArray finalResponse = uartSpy.takeFirst().at(0).toByteArray();
    QVERIFY(finalResponse.contains("[GetConfigResult]"));
    QVERIFY(finalResponse.contains("Result=1"));
}

QTEST_MAIN(TestUartTxRx)
