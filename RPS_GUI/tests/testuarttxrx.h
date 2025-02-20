#ifndef TESTUARTTXRX_H
#define TESTUARTTXRX_H

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include "../UartTxRx.h"
#include "../inibyteparser.h"

class TestUartTxRx : public QObject
{
    Q_OBJECT

private:
    UartTxRx* uart;
    IniByteParser* iniParser;

    // Helper function to wait for response
    bool waitForResponse(QSignalSpy& spy, int timeout = 1);
    void CleanGameStateRequest();

private slots:
    void initTestCase();
    void testIniByteParserGeneratorMsg_GenerateSetGameStateMessage();
    void testIniByteParserGeneratorMsg_GenerateSetPlayerTurn();
    void testIniByteParserGeneratorMsg_GenerateGetGameStateMessage();
    void testParseINIData_GetConfigResult();
    void testParseINIData_GetGameState();
    void testParseINIData_SetPlayerTurn();
    void testUartSendMessage();
    void testUartIsConnected();
    void cleanupTestCase();
    void SetConnectionUART();
};

#endif // TESTUARTTXRX_H
