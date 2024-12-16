#include "testuarttxrx.h"

bool TestUartTxRx::waitForResponse(QSignalSpy& spy, int timeout)
{
    if (!spy.wait(timeout)) {
        qWarning() << "Timeout waiting for response";
        return false;
    }
    return true;
}

void TestUartTxRx::initTestCase()
{
    try {
        uart = UartTxRx::GetInstance();
        iniParser = IniByteParser::GetInstance();

        // Initialize UART with retry mechanism
        int retries = 3;
        int initResult = -1;

        while (retries > 0 && initResult != 0) {
#ifdef TEST_PORT
            QString portName = TEST_PORT; // Default port name from CMake
#else
            QString portName = "/dev/ttyACM0";
#endif
            initResult = uart->InitConnection(portName);
            if (initResult != 0) {
                qDebug() << "Failed to initialize UART, retrying..." << retries;
                QTest::qWait(1000); // Wait 1 second before retry
                retries--;
            }
        }

        if (initResult != 0) {
            QFAIL("Failed to initialize UART connection after multiple attempts");
        }

        qDebug() << "UART initialized successfully";
        qDebug() << "Pause to wait for board initialization for 2 sec...";
        QTest::qWait(2000);
    } catch (const std::exception& e) {
        QFAIL(qPrintable(QString("Exception in initTestCase: %1").arg(e.what())));
    }
}

void TestUartTxRx::cleanupTestCase()
{
    if (uart) {
        uart->Disconnect();
        delete uart;
    }
}

void TestUartTxRx::testUartConnectivity()
{
    QVERIFY(uart != nullptr);
    QVERIFY(uart->isConnected());
}

void TestUartTxRx::testSendConfigMessage()
{
    std::string cleanGame_message = IniByteParser::GetInstance()->generateCleanGame();

    // Set up spy for complete message received signal
    QSignalSpy cleanGameOkSpy(iniParser, &IniByteParser::ServerGoodClean);

    // Send message and verify it was sent successfully
    qDebug() << "Send " + QString::fromStdString(cleanGame_message);
    uart->sendMessage(QByteArray::fromStdString(cleanGame_message));

    // Wait for response from board
    QVERIFY(waitForResponse(cleanGameOkSpy, 10000));

    GameState gamestate = {.isLoaded = 0, .mode="EVE", .maxRoundsAmount=3};
    std::string sendConfig_message = IniByteParser::GetInstance()->generateSetGameStateMessage(gamestate);

    // Set up spy for complete message received signal
    QSignalSpy getGameStateSpy(iniParser, &IniByteParser::ServerGoodConfig);

    // Send message and verify it was sent successfully
    uart->sendMessage(QByteArray::fromStdString(sendConfig_message));
    // QCOMPARE(result, 0);

    // Wait for response from board
    QVERIFY(waitForResponse(getGameStateSpy));

    // Verify response format
    QByteArray response = getGameStateSpy.takeFirst().at(0).toByteArray();

    // Print the response as both hex and ASCII
    qDebug() << "Response (ASCII):" << QString::fromUtf8(response);
    qDebug() << "Response (Hex):" << response.toHex();

    // Verify response contains expected content
    QVERIFY(response.contains("[GetConfigResult]"));
}


void TestUartTxRx::testParserSignals()
{
    // Set up spies for both the UART and parser signals
    QSignalSpy uartSpy(uart, &UartTxRx::completeMessageReceived);
    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerGoodConfig);


    GameState gs = {.isLoaded = 0, .mode = "PVP", .maxRoundsAmount = 5};
    // Send configuration message
    QByteArray message = QByteArray::fromStdString(    IniByteParser::GetInstance()->generateSetGameStateMessage(gs));
    uart->sendMessage(message);

    // Wait for UART response
    QVERIFY(waitForResponse(uartSpy));

    // Wait for parser signal
    QVERIFY(waitForResponse(parserSpy));

    // Verify parser received correct value
    QList<QVariant> args = parserSpy.takeFirst();
    QCOMPARE(args.at(0).toInt(), 1); // Expecting Result=1
}

void TestUartTxRx::testEndToEndConfigFlow()
{
    // Set up spies
    QSignalSpy uartSpy(uart, &UartTxRx::completeMessageReceived);
    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerGoodConfig);

    // Send invalid message to test error handling
    QByteArray invalidMessage = "[InvalidConfig]\nBadData=1\n";
    uart->sendMessage(invalidMessage);

    // Wait for response
    if (waitForResponse(uartSpy)) {
        QByteArray response = uartSpy.takeFirst().at(0).toByteArray();
        qDebug() << "Response to invalid message:" << response;
    }

    // Send valid message
    QByteArray validMessage = "[SetGameConfig]\nClient=1\nData1=1\nData2=2\n";
    uart->sendMessage(validMessage);

    // Wait for both UART and parser responses
    QVERIFY(waitForResponse(uartSpy));
    QVERIFY(waitForResponse(parserSpy));

    // Verify final state
    QByteArray finalResponse = uartSpy.takeFirst().at(0).toByteArray();
    QVERIFY(finalResponse.contains("[GetConfigResult]"));
    QVERIFY(finalResponse.contains("Result=1"));
}

QTEST_MAIN(TestUartTxRx)
