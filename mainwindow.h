#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QSplitter>

#include "multicastreceiver.h"
#include "multicastsender.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MulticastReceiver;
class MulticastSender;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onMessageReceived(const QString &msg, const QString &ip, quint16 port);
    void onSendButtonClicked();
    void onStartAutoSend();
    void onStopAutoSend();
    void onBindReceiveClicked();
    void onBindSendClicked();
    QByteArray hexStringToByteArray(const QString &hexString);

private:
    Ui::MainWindow *ui;
    MulticastReceiver *receiver;
    MulticastSender *sender;

    // 接收区域组件
    QTextEdit *receiveTextEdit;
    QLabel *receiveLabel;

    // 发送区域组件
    QTextEdit *sendTextEdit;
    QLabel *sendLabel;
    QPushButton *btnSend;
    QPushButton *btnStart;
    QPushButton *btnStop;

    // 新增：自动发送间隔
    QLineEdit *intervalLineEdit;     // 新增：自动发送间隔输入框
    QLabel *intervalLabel;           // 新增：标签\

    // ========== 接收区域新增控件 ==========
    QLabel *receivePortLabel;
    QLineEdit *receivePortEdit;
    QLabel *receiveGroupLabel;
    QLineEdit *receiveGroupEdit;
    QLabel *receiveIpLabel;
    QLineEdit *receiveIpEdit;
    QPushButton *btnBindReceive;

    // ========== 发送区域新增控件 ==========
    QLabel *sendPortLabel;
    QLineEdit *sendPortEdit;
    QLabel *sendGroupLabel;
    QLineEdit *sendGroupEdit;
    QLabel *sendIpLabel;
    QLineEdit *sendIpEdit;
    QPushButton *btnBindSend;
};
#endif // MAINWINDOW_H
