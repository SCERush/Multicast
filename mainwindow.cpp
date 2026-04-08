#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QSplitter>
#include <QGroupBox>
#include <QRegularExpression>
#include <QIntValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建中心部件和主布局
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    // 创建分割器，用于分离发送和接收区域
    auto *splitter = new QSplitter(Qt::Vertical, this);
    splitter->setHandleWidth(4);

    // ========== 接收区域（上半部分）==========
    auto *receiveGroup = new QGroupBox("接收消息", this);
    auto *receiveLayout = new QVBoxLayout(receiveGroup);

    receiveLabel = new QLabel("接收到的消息:", receiveGroup);
    receiveLabel->setFont(QFont("Arial", 10, QFont::Bold));

    receiveTextEdit = new QTextEdit(receiveGroup);
    receiveTextEdit->setReadOnly(true);
    receiveTextEdit->setPlaceholderText("等待接收组播消息...");
    receiveTextEdit->setFont(QFont("Consolas", 9));

    // ========== 接收区域配置面板 ==========
    auto *receiveConfigLayout = new QHBoxLayout;

    receivePortLabel = new QLabel("端口:", receiveGroup);
    receivePortEdit = new QLineEdit("9002", receiveGroup);
    receivePortEdit->setMaximumWidth(80);

    receiveGroupLabel = new QLabel("组播地址:", receiveGroup);
    receiveGroupEdit = new QLineEdit("226.0.0.90", receiveGroup);
    receiveGroupEdit->setMaximumWidth(120);

    receiveIpLabel = new QLabel("网口IP:", receiveGroup);
    receiveIpEdit = new QLineEdit("192.168.8.100", receiveGroup);
    receiveIpEdit->setMaximumWidth(140);

    btnBindReceive = new QPushButton("绑定接收", receiveGroup);
    btnBindReceive->setStyleSheet("background-color: #2196F3; color: white;");

    receiveConfigLayout->addWidget(receivePortLabel);
    receiveConfigLayout->addWidget(receivePortEdit);
    receiveConfigLayout->addWidget(receiveGroupLabel);
    receiveConfigLayout->addWidget(receiveGroupEdit);
    receiveConfigLayout->addWidget(receiveIpLabel);
    receiveConfigLayout->addWidget(receiveIpEdit);
    receiveConfigLayout->addWidget(btnBindReceive);
    receiveConfigLayout->addStretch();

    receiveLayout->addWidget(receiveLabel);
    receiveLayout->addWidget(receiveTextEdit);
    receiveLayout->addLayout(receiveConfigLayout);

    // ========== 发送区域（下半部分）==========
    auto *sendGroup = new QGroupBox("发送消息", this);
    auto *sendLayout = new QVBoxLayout(sendGroup);

    sendLabel = new QLabel("输入要发送的消息:", sendGroup);
    sendLabel->setFont(QFont("Arial", 10, QFont::Bold));

    sendTextEdit = new QTextEdit(sendGroup);
    sendTextEdit->setPlaceholderText("在此输入要发送的组播消息..."
                                     "（支持格式：\"48 65 6C\" 或 \"48656C\" 或 \"48-65-6C\"）");
    sendTextEdit->setFont(QFont("Consolas", 9));
    sendTextEdit->setMaximumHeight(120);

    // ========== 新增：自动发送间隔输入框 ==========
    auto *intervalLayout = new QHBoxLayout;

    intervalLabel = new QLabel("自动发送间隔 (ms):", sendGroup);
    intervalLabel->setFont(QFont("Arial", 10, QFont::Bold));

    intervalLineEdit = new QLineEdit(sendGroup);
    intervalLineEdit->setText("1000");                    // 默认 1000ms
    intervalLineEdit->setValidator(new QIntValidator(10, 600000, this));  // 限制 10ms ~ 10分钟
    intervalLineEdit->setMaximumWidth(120);
    intervalLineEdit->setFont(QFont("Consolas", 10));

    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(intervalLineEdit);
    intervalLayout->addStretch();   // 让输入框靠左对齐

    // ========== 发送区域配置面板 ==========
    auto *sendConfigLayout = new QHBoxLayout;

    sendPortLabel = new QLabel("端口:", sendGroup);
    sendPortEdit = new QLineEdit("8888", sendGroup);
    sendPortEdit->setMaximumWidth(80);

    sendGroupLabel = new QLabel("组播地址:", sendGroup);
    sendGroupEdit = new QLineEdit("226.0.0.80", sendGroup);
    sendGroupEdit->setMaximumWidth(120);

    sendIpLabel = new QLabel("网口IP:", sendGroup);
    sendIpEdit = new QLineEdit("192.168.8.100", sendGroup);
    sendIpEdit->setMaximumWidth(140);

    btnBindSend = new QPushButton("绑定发送", sendGroup);
    btnBindSend->setStyleSheet("background-color: #2196F3; color: white;");

    sendConfigLayout->addWidget(sendPortLabel);
    sendConfigLayout->addWidget(sendPortEdit);
    sendConfigLayout->addWidget(sendGroupLabel);
    sendConfigLayout->addWidget(sendGroupEdit);
    sendConfigLayout->addWidget(sendIpLabel);
    sendConfigLayout->addWidget(sendIpEdit);
    sendConfigLayout->addWidget(btnBindSend);
    sendConfigLayout->addStretch();

    // ========== 按钮布局 ==========
    auto *buttonLayout = new QHBoxLayout;
    btnSend = new QPushButton("发送一次", sendGroup);
    btnStart = new QPushButton("开始自动发送", sendGroup);
    btnStop = new QPushButton("停止自动发送", sendGroup);

    // ========== 更明显的按钮按压效果 ==========
    QString baseStyle = R"(
    QPushButton {
    font: 9pt "Microsoft YaHei";
        padding: 8px 16px;
        border-radius: 6px;
        border: 1px solid #555;
        background-color: #333333;
        color: white;
    }
    QPushButton:hover {
        border: 2px solid #888888;
        background-color: #4a4a4a;
    }
    QPushButton:pressed {
        background-color: #1e1e1e;
        border: 2px inset #aaaaaa;     /* 内凹效果，非常明显 */
        padding-top: 12px;             /* 模拟按钮被按下去 */
        padding-bottom: 8px;
        color: #dddddd;
    }
    )";

    // 三个按钮分别设置主色调（保持语义）
    btnSend->setStyleSheet(baseStyle + R"(
    QPushButton { background-color: #2e7d32; border-color: #1b5e20; }
    QPushButton:hover { background-color: #388e3c; }
    QPushButton:pressed { background-color: #1b5e20; border-color: #0f3d15; }
    )");

    btnStart->setStyleSheet(baseStyle + R"(
    QPushButton { background-color: #1976d2; border-color: #0d47a1; }
    QPushButton:hover { background-color: #1e88e5; }
    QPushButton:pressed { background-color: #0d47a1; border-color: #002f6c; }
    )");

    btnStop->setStyleSheet(baseStyle + R"(
    QPushButton { background-color: #d32f2f; border-color: #b71c1c; }
    QPushButton:hover { background-color: #e53935; }
    QPushButton:pressed { background-color: #b71c1c; border-color: #7f0000; }
    )");

    buttonLayout->addWidget(btnSend);
    buttonLayout->addWidget(btnStart);
    buttonLayout->addWidget(btnStop);

    sendLayout->addWidget(sendLabel);
    sendLayout->addWidget(sendTextEdit);
    sendLayout->addLayout(intervalLayout);
    sendLayout->addLayout(sendConfigLayout);
    sendLayout->addLayout(buttonLayout);

    // 将两个区域添加到分割器
    splitter->addWidget(receiveGroup);
    splitter->addWidget(sendGroup);
    splitter->setStretchFactor(0, 2); // 接收区域占2份
    splitter->setStretchFactor(1, 1); // 发送区域占1份

    mainLayout->addWidget(splitter);
    setCentralWidget(central);

    // ========== 使用界面输入框的值初始化 receiver 和 sender ==========
    quint16 recvPort = receivePortEdit->text().toUShort();
    QString recvGroupInit = receiveGroupEdit->text().trimmed();
    QString recvIp = receiveIpEdit->text().trimmed();

    quint16 sendPort = sendPortEdit->text().toUShort();
    QString sendGroupInit = sendGroupEdit->text().trimmed();
    QString sendIp = sendIpEdit->text().trimmed();

    receiver = new MulticastReceiver(recvPort,
                                     recvGroupInit.isEmpty() ? "226.0.0.90" : recvGroupInit,
                                     recvIp.isEmpty() ? "192.168.8.100" : recvIp,
                                     this);

    sender = new MulticastSender(sendPort,
                                 recvGroupInit.isEmpty() ? "226.0.0.90" : recvGroupInit,
                                 sendIp.isEmpty() ? "192.168.8.100" : sendIp,
                                 this);


    // 连接信号槽
    connect(receiver, &MulticastReceiver::messageReceived,
            this, &MainWindow::onMessageReceived);
    connect(btnSend, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartAutoSend);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStopAutoSend);
    // 绑定按钮信号
    connect(btnBindReceive, &QPushButton::clicked, this, &MainWindow::onBindReceiveClicked);
    connect(btnBindSend, &QPushButton::clicked, this, &MainWindow::onBindSendClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onMessageReceived(const QString &hexData, const QString &ip, quint16 port)
{
    // 获取当前时间戳，格式：HH:mm:ss.zzz（时:分:秒.毫秒）
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");

    QString displayHex;
    int totalChars = hexData.length();
    int totalBytes = totalChars / 2;

    if (totalBytes <= 96) {
        displayHex = hexData;
    } else {
        QString prefix = hexData.left(128);   // 前 64 字节 → 128 字符
        QString suffix = hexData.right(64);   // 后 32 字节 → 64 字符
        displayHex = prefix + " ... " + suffix;
    }

    // 单行格式：[时间] [IP:PORT] HEX: ...
    QString line = QString("[%1] [%2:%3] [Len:%4] HEX: %5")
                       .arg(timestamp)
                       .arg(ip)
                       .arg(port)
                       .arg(totalBytes)
                       .arg(displayHex);

    receiveTextEdit->append(line);
}

void MainWindow::onSendButtonClicked()
{
    QString hexInput = sendTextEdit->toPlainText().trimmed();

    if (hexInput.isEmpty()) {
        receiveTextEdit->append("<font color='orange'>[系统] 输入框为空，未发送数据。</font>");
        return;
    }

    // 将十六进制字符串转换为 QByteArray
    QByteArray data = hexStringToByteArray(hexInput);

    if (data.isEmpty() && !hexInput.isEmpty()) {
        receiveTextEdit->append("<font color='red'>[错误] 输入的十六进制格式无效，请检查是否有非十六进制字符或空格位置错误。</font>");
        return;
    }

    // 发送二进制数据
    sender->sendOnce(data);

    // 在接收框显示发送记录（可选）
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    QString log = QString("[%1] [本机发送] HEX: %2 (%3 bytes)")
                      .arg(timestamp)
                      .arg(hexInput)
                      .arg(data.size());
    receiveTextEdit->append(log);
}

void MainWindow::onStartAutoSend()
{
    QString hexInput = sendTextEdit->toPlainText().trimmed();
    QByteArray data = hexStringToByteArray(hexInput);

    if (data.isEmpty() && !hexInput.isEmpty()) {
        receiveTextEdit->append("<font color='red'>[错误] 自动发送失败：十六进制格式无效。</font>");
        return;
    }

    // 获取用户输入的间隔时间（ms）
    bool ok = false;
    int interval = intervalLineEdit->text().trimmed().toInt(&ok);

    if (!ok || interval < 10) {
        receiveTextEdit->append("<font color='orange'>[警告] 间隔时间无效，已使用默认1000ms</font>");
        interval = 1000;
    }

    sender->setAutoSendData(data);
    sender->startSending(interval);     // ← 传入用户设置的间隔

    receiveTextEdit->append(QString("<font color='green'>[系统] 已开始自动发送，间隔 %1 ms</font>").arg(interval));
}

void MainWindow::onStopAutoSend()
{
    sender->stopSending();
    receiveTextEdit->append(QString("<font color='red'>[系统] 已停止自动发送</font>"));
}

QByteArray MainWindow::hexStringToByteArray(const QString &hexString)
{
    // 【新增辅助函数】将十六进制字符串转换为 QByteArray
    // 支持格式："48 65 6C 6C 6F" 或 "48656C6C6F" 或 "48-65-6C"
    QString cleanHex = hexString;
    // 移除所有空格、横线、冒号等分隔符
    cleanHex.remove(QRegularExpression("[\\s:-]"));

    // 检查长度是否为偶数
    if (cleanHex.length() % 2 != 0) {
        qWarning() << "Hex string length is odd, appending 0";
        cleanHex.append('0'); // 或者返回空，视需求而定
    }

    // 检查是否包含非十六进制字符
    QRegularExpression hexCheck("^[0-9A-Fa-f]+$");
    if (!hexCheck.match(cleanHex).hasMatch()) {
        return QByteArray(); // 返回空表示格式错误
    }

    QByteArray result;
    result.reserve(cleanHex.length() / 2);

    for (int i = 0; i < cleanHex.length(); i += 2) {
        bool ok;
        // 每次取两个字符转换为一个字节
        quint8 byte = cleanHex.mid(i, 2).toInt(&ok, 16);
        if (ok) {
            result.append(static_cast<char>(byte));
        } else {
            return QByteArray(); // 转换失败
        }
    }

    return result;
}

void MainWindow::onBindReceiveClicked()
{
    quint16 newPort = receivePortEdit->text().toUShort();
    QString newGroup = receiveGroupEdit->text().trimmed();
    QString newIp = receiveIpEdit->text().trimmed();

    if (newGroup.isEmpty()) {
        receiveTextEdit->append("<font color='red'>[错误] 组播地址不能为空</font>");
        return;
    }

    // 先停止旧的接收（重要）
    disconnect(receiver, &MulticastReceiver::messageReceived, this, &MainWindow::onMessageReceived);

    // 删除旧对象，创建新对象（最可靠的方式）
    delete receiver;
    receiver = new MulticastReceiver(newPort, newGroup, newIp.isEmpty() ? "192.168.8.100" : newIp, this);

    connect(receiver, &MulticastReceiver::messageReceived,
            this, &MainWindow::onMessageReceived);

    receiveTextEdit->append(QString("<font color='green'>[系统] 接收已重新绑定 → 端口:%1  组播:%2  接口:%3</font>")
                                .arg(newPort).arg(newGroup).arg(newIp));
}

void MainWindow::onBindSendClicked()
{
    quint16 newPort = sendPortEdit->text().toUShort();
    QString newGroup = sendGroupEdit->text().trimmed();
    QString newIp = sendIpEdit->text().trimmed();

    if (newGroup.isEmpty()) {
        receiveTextEdit->append("<font color='red'>[错误] 组播地址不能为空</font>");
        return;
    }

    // 删除旧发送器，创建新的
    delete sender;
    sender = new MulticastSender(newPort, newGroup, newIp.isEmpty() ? "192.168.8.100" : newIp, this);

    receiveTextEdit->append(QString("<font color='green'>[系统] 发送已重新绑定 → 端口:%1  组播:%2  接口:%3</font>")
                                .arg(newPort).arg(newGroup).arg(newIp));

    // 注意：如果正在自动发送，需要重新 setAutoSendData 并 startSending
}
