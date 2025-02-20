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

void TestUartTxRx::SetConnectionUART()
{
    uart = UartTxRx::GetInstance();

    if(uart->isConnected())
    {
        qDebug() << "UART already initialized";

    }else{
        int err = 99;
        err = uart->InitConnection("/dev/ttyACM1");
        QVERIFY(err == 0);
        qDebug() << "UART initialized successfully";
    }
}

void TestUartTxRx::initTestCase() {
    try {
        registerCustomTypes();
        // uart = UartTxRx::GetInstance();
        iniParser = IniByteParser::GetInstance();

    } catch (const std::exception& e) {
        QFAIL(qPrintable(QString("Exception in initTestCase: %1").arg(e.what())));
    }
}


void TestUartTxRx::testIniByteParserGeneratorMsg_GenerateSetGameStateMessage()
{
    iniParser = IniByteParser::GetInstance();

    GameState game_state = {.isLoaded=0, .mode="EVE", .maxRoundsAmount=3};
    std::string setGameMessage = iniParser->generateSetGameStateMessage(game_state);

    QVERIFY(!setGameMessage.empty());
}


void TestUartTxRx::testIniByteParserGeneratorMsg_GenerateSetPlayerTurn(){
    iniParser = IniByteParser::GetInstance();

    ClientGameTurn turn = {.choice = "ROCK"};
    std::string setPlayerTurnMessage = iniParser->generateSetPlayerTurn(turn);

    QVERIFY(!setPlayerTurnMessage.empty());
}


void TestUartTxRx::testIniByteParserGeneratorMsg_GenerateGetGameStateMessage(){
    iniParser = IniByteParser::GetInstance();

    ClientGameTurn turn = {.choice = "ROCK"};
    std::string getGameStateMessage = iniParser->generateGetGameStateMessage();

    QVERIFY(!getGameStateMessage.empty());
}


void TestUartTxRx::testUartSendMessage() {
    TestUartTxRx::SetConnectionUART();
    int err = uart->GetInstance()->sendMessage("Amogus");

    QVERIFY(err == 0);
}

void TestUartTxRx::testUartIsConnected()
{
    TestUartTxRx::SetConnectionUART();
    bool err = uart->GetInstance()->isConnected();
    QVERIFY(err == true);
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
    TestUartTxRx::SetConnectionUART();
    TestUartTxRx::CleanGameStateRequest();
    std::string cleanGameMessage = iniParser->generateCleanGame();

    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerGoodConfig);
    GameState game_state = {.isLoaded=0, .mode="EVE", .maxRoundsAmount=3};

    uart->sendMessage(QByteArray::fromStdString(iniParser->generateSetGameStateMessage(game_state)));

    qDebug() << "Wait for responce...";

    // Wait and verify signal value in one step
    QVERIFY(parserSpy.wait(1000) && parserSpy.first().at(0).toInt() == 1);
}


void TestUartTxRx::testParseINIData_GetGameState()
{
    TestUartTxRx::SetConnectionUART();
    TestUartTxRx::CleanGameStateRequest();

    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerSentGameState);

    uart->sendMessage(QByteArray::fromStdString(iniParser->generateGetGameStateMessage()));

    qDebug() << "Wait for responce...";

    QVERIFY(parserSpy.wait(1000));

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
    TestUartTxRx::SetConnectionUART();
    CleanGameStateRequest();

    QSignalSpy parserSpy(iniParser, &IniByteParser::ServerSentTurnResult);

    GameState game_state = {.isLoaded=0, .mode="PVE", .maxRoundsAmount=1};
    uart->sendMessage(QByteArray::fromStdString(iniParser->generateSetGameStateMessage(game_state)));
    qDebug() << "Sent set game";


    ClientGameTurn turn = {.choice = "ROCK"};
    uart->sendMessage(QByteArray::fromStdString(iniParser->generateSetPlayerTurn(turn)));

    QVERIFY(parserSpy.wait(1000));

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
