#ifndef MULTICASTSENDER_H
#define MULTICASTSENDER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>

class MulticastSender : public QObject
{
    Q_OBJECT
public:
    // 新增 localInterfaceIp 参数，默认值为空字符串（表示由系统自动选择）
    explicit MulticastSender(quint16 port,
                             const QString &groupAddress,
                             const QString &localInterfaceIp = QString(),
                             QObject *parent = nullptr);

    void sendOnce(const QByteArray &data);
    void setAutoSendData(const QByteArray &data);
    void startSending(int intervalMs);
    void stopSending();

private slots:
    void onTimeout();

private:
    QUdpSocket *udpSocket;
    QHostAddress groupAddr;
    quint16 m_port;
    QTimer *timer;
    QByteArray m_autoSendData;

    // 辅助函数：根据IP查找网络接口
    QNetworkInterface findInterfaceByIp(const QString &ipStr);
};

#endif // MULTICASTSENDER_H
