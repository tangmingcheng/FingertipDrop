#ifndef SYNCSERVER_H
#define SYNCSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QSslServer>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QNetworkInterface>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslConfiguration>
#include <QClipboard>
#include <QGuiApplication>
#include <QFile>
#include <QHostInfo>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QTimer>
#include <QProperty>


class SyncServer : public QObject
{
    Q_OBJECT
public:
    explicit SyncServer(QObject *parent = nullptr);
    void startServers();
    void stopServers();

private:
    QString getLocalIPAddress();
    QString generateHtmlContent(quint16 websocketPort);
    QString parseDeviceName(const QString &deviceInfo);

    template<typename SocketType>
    void handleHttpRequest(SocketType *clientConnection, const QString &requestStr);

signals:
    void deviceConnected(const QString &deviceName, const QString &deviceIP, const QString &status);
    void clientsChanged(int count);
    void clientsStatusChanged(bool active);

public slots:
    void handleFilePath(const QString &rawFilePath);

private slots:
    void onNewWebSocketConnection();
    void onWebSocketMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
    void onWebSocketDisconnected();
    void onHttpRequest();
    void onSslConnection(QSslSocket *clientConnection);
    void onSslErrors(QSslSocket *socket, const QList<QSslError> &errors);
    void onClipboardChanged();  // 剪贴板内容变化时调用
    void updateHttpServerInfo();

private:
    QWebSocketServer *webSocketServer;
    QWebSocket *clientSocket;
    QSslServer *httpServer;
    QList<QWebSocket *> clients;
    QClipboard *clipboard;  // 用于监控剪贴板内容
    QString generatedHtmlContent;

    QString currentFileName;
    QString currentFileType;
    QFile currentFile;

    QString htmlPath;
    QString jsPath;
    QString cssPath;

public:
    QBindable<QString> bindableHttpServerAddress();
    QBindable<quint16> bindableHttpServerPort();

private:
    QProperty<QString> m_httpServerAddress;
    QProperty<quint16> m_httpServerPort;

};

#endif // SYNCSERVER_H
