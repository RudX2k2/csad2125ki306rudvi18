#include "testuarttxrx.h"

Q_DECLARE_METATYPE(GameState)

void registerCustomTypes()
{
    qRegisterMetaType<GameState>("GameState");
}


// TestUartTxRx member function implementations
bool TestUartTxRx::waitForResponse(QSignalSpy& spy, int timeout) {
    return spy.wait(timeout);
}

void TestUartTxRx::initTestCase() {
    try {
        registerCustomTypes();
        uart = new UartTxRx("/dev/ttyACM1");
        iniParser = IniByteParser::GetInstance();
        int err = 99;
        err = uart->InitConnection("/dev/ttyACM1");
        QVERIFY(err == 0);
        qDebug() << "UART initialized successfully";
    } catch (const std::exception& e) {
        QFAIL(qPrintable(QString("Exception in initTestCase: %1").arg(e.what())));
    }
}

void TestUartTxRx::testUartSendMessage() {
    int err = uart->GetInstance()->sendMessage("Amogus");

    QVERIFY(err == 0);
}

void TestUartTxRx::testUartIsConnected()
{
    bool err = uart->GetInstance()->isConnected();
    QVERIFY(err);
}

void TestUartTxRx::CleanGameStateRequest()
{
    std::string cleanGameMessage = iniParser->generateCleanGame();

    QSignalSpy cleanSpy(iniParser, &IniByteParser::ServerGoodClean);

    uart->sendMessage(QByteArray::fromStdString(cleanGameMessage));
}

void TestUartTxRx::cleanupTestCase() {
    if (uart) {
        uart->Disconnect();
        delete uart;
    }
}

void TestUartTxRx::testParseINIData_GetConfigResult()
{
    TestUartTxRx::CleanGameStateRequest();
    std::string cleanGameMessage = iniParser->generateCleanGame();

    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerGoodConfig);
    GameState game_state = {.isLoaded=0, .mode="EVE", .maxRoundsAmount=3};

    uart->sendMessage(QByteArray::fromStdString(iniParser->generateSetGameStateMessage(game_state)));

    qDebug() << "Wait for responce...";

    // Wait and verify signal value in one step
    QVERIFY(parserSpy.wait(5000) && parserSpy.first().at(0).toInt() == 1);
}


void TestUartTxRx::testParseINIData_GetGameState()
{
    TestUartTxRx::CleanGameStateRequest();

    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerSentGameState);

    uart->sendMessage(QByteArray::fromStdString(iniParser->generateGetGameStateMessage()));

    qDebug() << "Wait for responce...";

    QVERIFY(parserSpy.wait(5000));

    // Verify that the signal was emitted at least once
    QVERIFY(parserSpy.count() > 0);

    // Access the first signal's argument
    QVariantList arguments = parserSpy.takeFirst(); // Get the first signal's data
    QVERIFY(arguments.count() > 0);

    // Extract and cast the argument to GameState
    GameState gameState = qvariant_cast<GameState>(arguments.at(0));

    // Perform additional checks on gameState
    QVERIFY(!(gameState.mode == ""));
    // QCOMPARE(gameState.someOtherField, expectedValue); // Example comparison
}

void TestUartTxRx::testParseINIData_SetPlayerTurn()
{
    CleanGameStateRequest();
    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerSentTurnResult);
    ClientGameTurn turn = {.choice = "ROCK"};

    GameState game_state = {.isLoaded=0, .mode="PVE", .maxRoundsAmount=1};
    uart->sendMessage(QByteArray::fromStdString(iniParser->generateSetGameStateMessage(game_state)));
    qDebug() << "Sent set game";

    uart->sendMessage(QByteArray::fromStdString(iniParser->generateSetPlayerTurn(turn)));

    QVERIFY(parserSpy.wait(5000));

    // Verify that the signal was emitted at least once
    QVERIFY(parserSpy.count() > 0);

    // Access the first signal's argument
    QVariantList arguments = parserSpy.takeFirst(); // Get the first signal's data
    QVERIFY(arguments.count() > 0);

    // Extract and cast the argument to GameState
    GameState gameState = qvariant_cast<GameState>(arguments.at(0));

    // Perform additional checks on gameState
    QVERIFY(!(gameState.mode == ""));
    // QCOMPARE(gameState.someOtherField, expectedValue); // Example comparison
}





QTEST_MAIN(TestUartTxRx)
