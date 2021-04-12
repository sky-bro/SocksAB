#ifndef HTTPPROXY_H
#define HTTPPROXY_H

#include <QTcpServer>
#include <QNetworkProxy>

#define FD_SETSIZE 1024

class HttpProxy: public QTcpServer
{
    Q_OBJECT
public:
    HttpProxy();

    /**
     * setProxy for this tcpserver: use local socks server
     */
    bool httpListen(const QHostAddress &http_addr, uint16_t http_port, uint16_t socks_port);

protected:
    /**
     * local_socket(fd) <--> remote_socket (setproxy(localhost:socks_port))
     */
    void incomingConnection(qintptr fd);

private:
    QNetworkProxy socksProxy;

private slots:
    void onLocalSocketError(QAbstractSocket::SocketError);
    void onRemoteSocketError(QAbstractSocket::SocketError);
    /**
     * two types of http proxy
     * connect method: supports both http & https
     * the other: only http
     * ref: https://imququ.com/post/web-proxy.html
     */
    void onLocalReadyRead();
    void onRemoteConnected();
    void onRemoteReadyRead();
};

#endif // HTTPPROXY_H
