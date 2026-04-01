#include "multicastsender.h"
#include <QTimer>
#include <QDebug>
#include <QVariant>
#include <QNetworkInterface>
#include <QHostAddress>

// 辅助函数：根据 IPv4 地址查找网络接口 (逻辑与 receiver 中保持一致)
QNetworkInterface MulticastSender::findInterfaceByIp(const QString &ipStr)
{
    if (ipStr.isEmpty()) {
        return QNetworkInterface(); // 返回无效接口，表示不指定
    }

    QHostAddress target(ipStr);
    if (target.isNull()) {
        qWarning() << "[Sender] Invalid IP address:" << ipStr;
        return QNetworkInterface();
    }

    for (const QNetworkInterface &interface : QNetworkInterface::allInterfaces()) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

            for (const QNetworkAddressEntry &entry : interface.addressEntries()) {
                // 匹配 IPv4 地址
                if (entry.ip() == target) {
                    qDebug() << "[Sender] Found interface:" << interface.name() << "for IP" << ipStr;
                    return interface;
                }
            }
        }
    }

    qWarning() << "[Sender] No network interface found with IP:" << ipStr;
    return QNetworkInterface();
}

MulticastSender::MulticastSender(quint16 port,
                                 const QString &groupAddress,
                                 const QString &localInterfaceIp,
                                 QObject *parent)
    : QObject(parent), m_port(port), timer(nullptr)
{
    udpSocket = new QUdpSocket(this);
    groupAddr = QHostAddress(groupAddress);

    // === 关键修复：绑定到具体的本地IP（而不再是 Any）===
    QHostAddress bindAddr = localInterfaceIp.isEmpty()
                                ? QHostAddress::Any
                                : QHostAddress(localInterfaceIp);

    bool bound = udpSocket->bind(bindAddr, 0,   // ←←← 修改：使用具体 IP
                                 QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
    if (!bound) {
        qWarning() << "[Sender] Bind failed:" << udpSocket->errorString();
    } else {
        qDebug() << "[Sender] Successfully bound to" << bindAddr.toString() << ":0";
    }

    // 2. 指定多播发送接口（保留原有逻辑）
    if (!localInterfaceIp.isEmpty()) {
        QNetworkInterface iface = findInterfaceByIp(localInterfaceIp);
        if (iface.isValid()) {
            udpSocket->setMulticastInterface(iface);   // ←←← 保留
            qDebug() << "[Sender] ✅ Multicast outgoing interface set to:"
                     << iface.name() << "(" << localInterfaceIp << ")";
        } else {
            qWarning() << "[Sender] Failed to set multicast interface. Will use system default.";
        }
    }

    // 设置 TTL
    udpSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, QVariant(1));

    qDebug() << "[Sender] Initialized ready to send to" << groupAddress << ":" << m_port;
}

void MulticastSender::setAutoSendData(const QByteArray &data)
{
    m_autoSendData = data;
}

void MulticastSender::startSending(int intervalMs)
{
    if (m_autoSendData.isEmpty()) {
        qWarning() << "[Sender] Attempted to start auto-send but no data is set.";
        return;
    }

    if (!timer) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MulticastSender::onTimeout);
    }
    timer->start(intervalMs);
}

void MulticastSender::stopSending()
{
    if (timer) {
        timer->stop();
    }
}

void MulticastSender::sendOnce(const QByteArray &data)
{
    if (data.isEmpty()) {
        qWarning() << "[Sender] Attempted to send empty data.";
        return;
    }

    // 确保 socket 状态正常
    if (udpSocket->state() != QAbstractSocket::BoundState && udpSocket->state() != QAbstractSocket::ConnectedState) {
        udpSocket->bind(QHostAddress::Any, m_port, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
    }

    qint64 sent = udpSocket->writeDatagram(data, groupAddr, m_port);

    if (sent == -1) {
        qWarning() << "[Sender] Send failed:" << udpSocket->errorString();
    } else {
        qDebug() << "[Sender] Sent" << sent << "bytes to" << groupAddr.toString() << ":" << m_port;
        // 调试时可打印十六进制
        // qDebug() << "Payload (Hex):" << data.toHex(' ');
    }
}

void MulticastSender::onTimeout()
{
    if (!m_autoSendData.isEmpty()) {
        sendOnce(m_autoSendData);
    }
}
