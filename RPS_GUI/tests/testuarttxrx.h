#ifndef TESTUARTTXRX_H
#define TESTUARTTXRX_H

#include <QtTest/QtTest>
#include "../UartTxRx.h"

class TestUartTxRx : public QObject
{
    Q_OBJECT

private slots:
    void testSendMessage();
    void testConnection();
};

#endif // TESTUARTTXRX_H
