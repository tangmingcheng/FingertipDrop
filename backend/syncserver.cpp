#include "syncserver.h"


SyncServer::SyncServer(QObject *parent)
    : QObject(parent)
    , webSocketServer(new QWebSocketServer(QStringLiteral("SyncServer"), QWebSocketServer::SecureMode, this))
    , httpServer(new QSslServer(this))
    , clipboard(QGuiApplication::clipboard())
    , htmlPath("qml/Backend/html/index.html")
    , jsPath("qml/Backend/js/script.js")
    , cssPath("qml/Backend/css/style.css")
    , m_httpServerAddress("")
    , m_httpServerPort(0)
{
    connect(clipboard, &QClipboard::dataChanged, this, &SyncServer::onClipboardChanged);
}

QBindable<QString> SyncServer::bindableHttpServerAddress()
{
    return QBindable<QString>(&m_httpServerAddress);
}

QBindable<quint16> SyncServer::bindableHttpServerPort()
{
    return QBindable<quint16>(&m_httpServerPort);
}

QString SyncServer::getLocalIPAddress()
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            address != QHostAddress(QHostAddress::LocalHost)) {
             qDebug() << "[Local IP]" << address;
            return address.toString();
        }
    }
    qDebug() << "[Default Local IP]" << "127.0.0.1";
    return "127.0.0.1"; // 如果无法找到，则返回本地地址
}

// 生成包含 WebSocket 地址和端口的 HTML 内容
QString SyncServer::generateHtmlContent(quint16 websocketPort) {

    QFile file(htmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[HTTP] Failed to open file";
        return QString();
    }

    // 读取原始 HTML 内容
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // 获取 WebSocket 服务器的实际 IP 地址
    QString serverAddress = getLocalIPAddress();

    // 替换占位符
    content.replace("{{WEBSOCKET_ADDRESS}}", serverAddress);
    content.replace("{{WEBSOCKET_PORT}}", QString::number(websocketPort));

    return content;
}

void SyncServer::startServers()
{
    // 加载证书和密钥
    QFile certFile("server.crt");
    QFile keyFile("server.key");

    if (!certFile.open(QIODevice::ReadOnly) || !keyFile.open(QIODevice::ReadOnly)) {
        qWarning() << "[WebSocket] Failed to load certificate or key.";
        return;
    }

    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey privateKey(&keyFile, QSsl::Rsa, QSsl::Pem);

    QSslConfiguration sslConfig;
    sslConfig.setLocalCertificate(certificate);
    sslConfig.setPrivateKey(privateKey);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);

    webSocketServer->setSslConfiguration(sslConfig);
    // 启动 WebSocket 服务器
    if (webSocketServer->listen(QHostAddress::Any, 12345)) {
        QString serverAddress = webSocketServer->serverAddress().toString();
        quint16 serverPort = webSocketServer->serverPort();
        qDebug() << "[WebSocket] Server started at wss://" << serverAddress << ":" << serverPort;

        connect(webSocketServer, &QWebSocketServer::newConnection, this, &SyncServer::onNewWebSocketConnection);
        quint16 websocketPort = webSocketServer->serverPort();
        generatedHtmlContent = generateHtmlContent(websocketPort);
    } else {
        qDebug() << "[WebSocket] Failed to start server";
    }

    httpServer->setSslConfiguration(sslConfig);
    // 启动 HTTP 服务器
    if (httpServer->listen(QHostAddress::Any, 8080)) {
        QString serverAddress = httpServer->serverAddress().toString();
        quint16 serverPort = httpServer->serverPort();
        qDebug() << "[HTTP] Server started at " << serverAddress << ":" << serverPort;
        connect(httpServer, &QSslServer::startedEncryptionHandshake, this, &SyncServer::onSslConnection);
        updateHttpServerInfo();
    } else {
        qDebug() << "[HTTP] Failed to start server";
    }
}

void SyncServer::stopServers()
{
    // 停止 WebSocket 服务器
    if (webSocketServer->isListening()) {
        webSocketServer->close();
        qDebug() << "[WebSocket] Server stopped";
    }

    // 停止 HTTP 服务器
    if (httpServer->isListening()) {
        httpServer->close();
        qDebug() << "[HTTP] Server stopped";
    }
}

void SyncServer::onNewWebSocketConnection()
{
    clientSocket = webSocketServer->nextPendingConnection();
    if (clientSocket) {
        connect(clientSocket, &QWebSocket::textMessageReceived, this, &SyncServer::onWebSocketMessageReceived);
        connect(clientSocket, &QWebSocket::binaryMessageReceived, this, &SyncServer::onBinaryMessageReceived);
        connect(clientSocket, &QWebSocket::disconnected, this, &SyncServer::onWebSocketDisconnected);

        clients << clientSocket;

        // 打印 WebSocket 客户端的连接信息
        qDebug() << "[WebSocket] New WebSocket connection from"
                 << clientSocket->peerAddress().toString()
                 << "on port"
                 << clientSocket->peerPort();

        // 检测客户端数量变化
        int clientCount = clients.size();
        emit clientsChanged(clientCount);
        if (clientCount == 1) {
            emit clientsStatusChanged(true);  // 第一个客户端连接时
        }
    }
}

void SyncServer::onWebSocketMessageReceived(const QString &message)
{
    QWebSocket *senderClient = qobject_cast<QWebSocket *>(sender());
    if (!senderClient) return;

    qDebug() << "[WebSocket] Message received from" << senderClient->peerAddress().toString() << ":" << message;

    // 解析 JSON 消息
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "[WebSocket] Received non-JSON message";
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("type") && obj["type"].toString() == "deviceInfo") {
        QString deviceInfo = obj["info"].toString();
        QString deviceIP = senderClient->peerAddress().toString();
        quint16 devicePort = senderClient->peerPort();

        qDebug() << "[WebSocket] Device info from" << senderClient->peerAddress().toString() << ":" << deviceInfo;

        // 使用通用方法提取设备名
        QString deviceName = parseDeviceName(deviceInfo);

        // 发射信号通知 Backend
        emit deviceConnected(deviceName, deviceIP, "Connected");

    } else {
        // 解析 JSON 元数据
        if (obj["type"].toString() == "fileMetadata") {
            currentFileName = obj["filename"].toString();
            currentFileType = obj["filetype"].toString();

            // 设置保存路径
            QString filePath = QCoreApplication::applicationDirPath() + "/" + currentFileName;
            currentFile.setFileName(filePath);

            if (!currentFile.open(QIODevice::WriteOnly)) {
                qWarning() << "Failed to open file for writing:" << filePath;
            } else {
                qDebug() << "Ready to receive file:" << currentFileName << "with type:" << currentFileType;

                // 发送确认消息，准备接收二进制文件数据
                clientSocket->sendTextMessage("READY_TO_RECEIVE_FILE");
            }
        }
        else {
             qWarning() << "Failed to parse metadata JSON";
        }
    }
}

void SyncServer::onBinaryMessageReceived(const QByteArray &message)
{
    if (currentFile.isOpen()) {
        currentFile.write(message);  // 写入文件的二进制数据
        qDebug() << "Received file data, size:" << message.size();
    }

    // 假设文件数据传输完成后立即关闭文件
    currentFile.close();
    qDebug() << "File transfer completed and saved as:" << currentFile.fileName();

    // 获取文件的完整路径
    QString filePath = currentFile.fileName();
    QFileInfo fileInfo(filePath);
    QString folderPath = fileInfo.absolutePath();

    qDebug() << "File path to open:" << filePath;
    qDebug() << "Folder path to open:" << folderPath;

    // 检测文件写入完成
    QTimer *timer = new QTimer(this);
    timer->setInterval(100); // 检测间隔为100ms
    timer->start();

    connect(timer, &QTimer::timeout, this, [=]() {
        QFileInfo checkFile(filePath);

        // 检测文件是否完全写入
        if (checkFile.exists() && checkFile.size() > 0) {
            qDebug() << "File fully written. Size:" << checkFile.size();
            timer->stop(); // 停止检测
            timer->deleteLater();

            // 打开文件夹
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
                qWarning() << "Failed to open folder with QDesktopServices. Path:" << folderPath;
            } else {
                qDebug() << "Folder opened successfully.";
            }
        } else {
            qDebug() << "Waiting for file to be written...";
        }
    });

}

void SyncServer::onWebSocketDisconnected()
{
    QWebSocket *clientSocket = qobject_cast<QWebSocket *>(sender());
    if (clientSocket) {
        clients.removeAll(clientSocket);
        clientSocket->deleteLater();
        qDebug() << "[WebSocket] Client disconnected from" << clientSocket->peerAddress().toString();

        // 检测客户端数量变化
        int clientCount = clients.size();
        emit clientsChanged(clientCount);
        if (clientCount == 0) {
            emit clientsStatusChanged(false);  // 最后一个客户端断开时
        }
    }
}

void SyncServer::onSslConnection(QSslSocket *clientConnection) {
    if (!clientConnection) {
        qWarning() << "[HTTPS] 无效的 QSslSocket 对象";
        return;
    }

    // 连接 encrypted 信号，确保在加密完成后处理
    connect(clientConnection, &QSslSocket::encrypted, this, [=]() {
        qDebug() << "[HTTPS] TLS 握手成功，客户端地址：" << clientConnection->peerAddress().toString();

        // 在此处理加密后的请求
        connect(clientConnection, &QSslSocket::readyRead, this, [=]() {
            QByteArray request = clientConnection->readAll();

            QString requestStr(request);
            qDebug() << "[HTTP] Request content:" << requestStr;

            handleHttpRequest(clientConnection, requestStr);

            // 确保在数据完全发送之后才断开连接
            connect(clientConnection, &QSslSocket::bytesWritten, clientConnection, [clientConnection](qint64 bytes) {
                if (clientConnection->bytesToWrite() == 0) {
                    clientConnection->disconnectFromHost();
                }
            });

            qDebug() << "[HTTP] Served HTTP request to client" ;
        });
    });

    // 如果握手失败，连接 sslErrors 信号
    connect(clientConnection, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
            this, [=](const QList<QSslError> &errors) {
                for (const QSslError &error : errors) {
                    qWarning() << "[HTTPS] SSL 错误：" << error.errorString();
                }
                clientConnection->ignoreSslErrors();  // 仅用于测试，生产环境应根据需要处理
            });

    clientConnection->startServerEncryption();
}
void SyncServer::onHttpRequest()
{
    QSslSocket *clientConnection = qobject_cast<QSslSocket *>(httpServer->nextPendingConnection());

    if (!clientConnection) {
        qWarning() << "[HTTPS] 未发现挂起连接";
        return;
    }

    // 检查连接是否已加密
    if (!clientConnection->isEncrypted()) {
        qWarning() << "[HTTPS] 未加密连接被拒绝，客户端地址：" << clientConnection->peerAddress().toString();
        clientConnection->disconnectFromHost();
        return;
    }

    connect(clientConnection, &QSslSocket::disconnected, clientConnection, &QSslSocket::deleteLater);

    // 连接 readyRead 信号到数据处理槽
    connect(clientConnection, &QSslSocket::encrypted, this, [this, clientConnection]() {
        QByteArray request = clientConnection->readAll();
        QString requestStr(request);
        qDebug() << "[HTTP] Request content:" << requestStr;

        handleHttpRequest(clientConnection, requestStr);


        // 确保在数据完全发送之后才断开连接
        connect(clientConnection, &QSslSocket::bytesWritten, clientConnection, [clientConnection](qint64 bytes) {
            if (clientConnection->bytesToWrite() == 0) {
                clientConnection->disconnectFromHost();
            }
        });

        qDebug() << "[HTTP] Served HTTP request to client" ;
    });
}

template<typename SocketType>
void SyncServer::handleHttpRequest(SocketType *clientConnection, const QString &requestStr) {
    // 判断请求的路径
    QString filePath;
    if (requestStr.contains("GET /script.js")) {
        filePath = jsPath;
        qDebug() << "[HTTP] Serving script.js";
    } else if (requestStr.contains("GET / ")) {
        filePath = htmlPath;
        qDebug() << "[HTTP] Serving index.html";
    } else if (requestStr.contains("GET /style.css")) {
        filePath = cssPath;
        qDebug() << "[HTTP] Serving style.css";
    } else {
        //QString response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        //clientConnection->write(response.toUtf8());
        //clientConnection->flush();
        //clientConnection->disconnectFromHost();
        //qDebug() << "[HTTP] 404 Not Found";
        //return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[HTTP] Failed to open file at:" << filePath;
        clientConnection->close();
        return;
    }

    // 读取文件内容并准备响应
    QByteArray content = file.readAll();
    file.close();

    // 设置不同的 Content-Type 头部
    QString contentType = "text/html";
    if (filePath.endsWith(".js")) {
        contentType = "application/javascript";
    } else if (filePath.endsWith(".css")) {
        contentType = "text/css";
    }

    QString response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + contentType + "; charset=UTF-8\r\n";

    if (contentType == "text/html") {
        response += "Content-Length: " + QString::number(generatedHtmlContent.toUtf8().size()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        clientConnection->write(response.toUtf8());
        clientConnection->write(generatedHtmlContent.toUtf8());
    } else {
        response += "Content-Length: " + QString::number(content.size()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        clientConnection->write(response.toUtf8());
        clientConnection->write(content);
    }

    clientConnection->flush();

    // 确保在数据完全发送之后断开连接
    connect(clientConnection, &QSslSocket::bytesWritten, clientConnection, [clientConnection](qint64 bytes) {
        if (clientConnection->bytesToWrite() == 0) {
            clientConnection->disconnectFromHost();
        }
    });

    qDebug() << "[HTTP] Served HTTP request to client";
}


void SyncServer::onClipboardChanged()
{
    QString clipboardText = clipboard->text();
    qDebug() << "[Clipboard] Clipboard content changed:" << clipboardText;

    qDebug()<<"[onClipboardChanged]clients"<<clients.size();
    // 将剪贴板内容广播给所有 WebSocket 客户端
    for (QWebSocket *client : clients) {
        if (client->isValid()) {
            client->sendTextMessage(clipboardText);
            qDebug() << "[WebSocket] Sent clipboard content to client:" << client->peerAddress().toString();
        }
    }
}


void SyncServer::updateHttpServerInfo()
{
    if (httpServer->isListening()) {
        m_httpServerAddress.setValue(getLocalIPAddress());
        m_httpServerPort.setValue(httpServer->serverPort());
    } else {
        m_httpServerAddress.setValue("");
        m_httpServerPort.setValue(0);
    }
}

QString SyncServer::parseDeviceName(const QString &deviceInfo)
{
    // 定义常见设备的正则表达式
    QRegularExpression windowsRegex("Windows NT [\\d\\.]+");
    QRegularExpression androidRegex("Android [\\d\\.]+");
    QRegularExpression iosRegex("iPhone OS [\\d_]+|iPad OS [\\d_]+");
    QRegularExpression macRegex("Mac OS X [\\d_]+");
    QRegularExpression linuxRegex("Linux [^;\\)\\s]+");

    QRegularExpressionMatch match;

    // 逐一匹配设备名
    if ((match = windowsRegex.match(deviceInfo)).hasMatch()) {
        return match.captured(0); // 捕获 Windows 版本
    } else if ((match = androidRegex.match(deviceInfo)).hasMatch()) {
        return match.captured(0); // 捕获 Android 版本
    } else if ((match = iosRegex.match(deviceInfo)).hasMatch()) {
        return match.captured(0); // 捕获 iOS 版本
    } else if ((match = macRegex.match(deviceInfo)).hasMatch()) {
        return match.captured(0); // 捕获 MacOS 版本
    } else if ((match = linuxRegex.match(deviceInfo)).hasMatch()) {
        return match.captured(0); // 捕获 Linux 版本
    }

    // 如果未匹配到，返回默认值
    return "Unknown Device";
}


void SyncServer::handleFilePath(const QString &rawFilePath)
{
    QString filePath = rawFilePath;

    if (filePath.startsWith("file:///")) {
        filePath = QUrl(filePath).toLocalFile();  // 移除 "file:///" 前缀
    }

    if (filePath.isEmpty()) {
        qWarning() << "[SyncServer] 文件路径为空，无法处理";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[SyncServer] 无法打开文件:" << filePath;
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();

    qDebug() << "[SyncServer] 文件已读取:" << fileName << ", 大小:" << fileData.size() << "字节";

    // 构造文件元数据 JSON
    QJsonObject metadata;
    metadata["type"] = "fileMetadata";
    metadata["filename"] = fileName;
    metadata["filetype"] = "application/octet-stream";

    QJsonDocument doc(metadata);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    qDebug()<<"clients"<<clients.size();
    // 发送文件和元数据到客户端
    for (QWebSocket *client : clients) {
        if (client && client->isValid()) {
            // 连接到字节写入完成信号
            connect(client, &QWebSocket::bytesWritten, this, [=](qint64 bytes) {
                static qint64 totalBytesWritten = 0;
                totalBytesWritten += bytes;

                if (totalBytesWritten >= fileData.size()) {
                    qDebug() << "[SyncServer] 文件成功发送到客户端:" << fileName;
                    //emit fileTransferCompleted(fileName);

                    // 重置统计数据
                    totalBytesWritten = 0;
                    disconnect(client, &QWebSocket::bytesWritten, nullptr, nullptr);
                }
            });

            // 发送元数据和文件
            client->sendTextMessage(jsonString);
            client->sendBinaryMessage(fileData);
        }
    }
}
