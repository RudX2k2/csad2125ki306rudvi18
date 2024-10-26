#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cstring>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnConnect_clicked();

    void on_btnSendHost_clicked();

    void updateTerminal(const QByteArray &newBuffer);  // Slot to handle buffer changes

    void on_btnRefreshPorts_clicked();

    void writeDisconnectedInTerminal();

    void on_btnDisconnected_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
