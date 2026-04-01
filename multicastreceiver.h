#ifndef MULTICASTRECEIVER_H
#define MULTICASTRECEIVER_H

#include <QObject>
#include <QUdpSocket>

class MulticastReceiver : public QObject
{
    Q_OBJECT

public:
    explicit MulticastReceiver(quint16 port,
                               const QString &groupAddress,
                               const QString &localInterfaceIp = QString(), // ← 新增参数
                               QObject *parent = nullptr);

signals:
    void messageReceived(const QString &hexData, const QString &senderIp, quint16 senderPort);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    QHostAddress groupAddr;
    quint16 m_port;
};

#endif // MULTICASTRECEIVER_H
