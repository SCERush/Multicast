#include "multicastreceiver.h"
#include <QNetworkInterface>
#include <QHostAddress>
#include <QDebug>

// Helper function to find interface by IPv4 address
static QNetworkInterface findInterfaceByIp(const QString &ipStr)
{
    QHostAddress target(ipStr);
    if (target.isNull()) {
        qWarning() << "[Receiver] Invalid IP address:" << ipStr;
        return QNetworkInterface();
    }

    for (const QNetworkInterface &interface : QNetworkInterface::allInterfaces()) {
        // 跳过无效或非活动接口
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

            for (const QNetworkAddressEntry &entry : interface.addressEntries()) {
                if (entry.ip() == target) {
                    qDebug() << "[Receiver] Found interface:" << interface.name() << "for IP" << ipStr;
                    return interface;
                }
            }
        }
    }

    qWarning() << "[Receiver] No network interface found with IP:" << ipStr;
    return QNetworkInterface(); // invalid
}

MulticastReceiver::MulticastReceiver(quint16 port,
                                     const QString &groupAddress,
                                     const QString &localInterfaceIp,
                                     QObject *parent)
    : QObject(parent), m_port(port)
{
    udpSocket = new QUdpSocket(this);
    groupAddr = QHostAddress(groupAddress);

    // 绑定到 AnyIPv4（必须）
    if (!udpSocket->bind(QHostAddress::AnyIPv4, m_port, QUdpSocket::ShareAddress)) {
        qWarning() << "[Receiver] Failed to bind UDP socket:" << udpSocket->errorString();
        return;
    }

    // 👇 关键：通过 IP 找到 QNetworkInterface
    QNetworkInterface iface = findInterfaceByIp(localInterfaceIp);
    if (iface.isValid()) {
        if (!udpSocket->joinMulticastGroup(groupAddr, iface)) {
            qWarning() << "[Receiver] Failed to join multicast group on interface" << localInterfaceIp
                       << ":" << udpSocket->errorString();
            return;
        }
    } else {
        qWarning() << "[Receiver] Cannot join multicast: interface not found for" << localInterfaceIp;
        return;
    }

    connect(udpSocket, &QUdpSocket::readyRead, this, &MulticastReceiver::readPendingDatagrams);
    qDebug() << "[Receiver] Successfully joined multicast group" << groupAddress
             << "on interface" << localInterfaceIp << "port" << m_port;
}

void MulticastReceiver::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(udpSocket->pendingDatagramSize()));
        QHostAddress senderAddress;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

        // 🔥 关键：以十六进制形式输出原始字节，不进行任何解码
        QString hexStr = datagram.toHex(':').toUpper(); // 用 ':' 分隔，大写（可选）
        // 或者：QString hexStr = datagram.toHex(); // 紧凑格式，如 "48656c6c6f"

        emit messageReceived(hexStr, senderAddress.toString(), senderPort);
    }
}
