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

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testParseINIData_GetConfigResult();
    void CleanGameStateRequest();
    void testParseINIData_GetGameState();
    void testParseINIData_SetPlayerTurn();
    void testUartSendMessage();
    void testUartIsConnected();

};

#endif // TESTUARTTXRX_H
