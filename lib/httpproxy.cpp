#include "httpproxy.h"

#include <qobjectdefs.h>

#include <QAbstractSocket>
#include <QTcpSocket>

HttpProxy::HttpProxy() : QTcpServer() {
    this->setMaxPendingConnections(MaxPendingConnections);
}

bool HttpProxy::httpListen(const QHostAddress &http_addr, uint16_t http_port,
                           uint16_t socks_port) {
    qDebug() << "using socks port:" << socks_port;
    socksProxy =
        QNetworkProxy(QNetworkProxy::Socks5Proxy, "127.0.0.1", socks_port);
    return this->listen(http_addr, http_port);
}

void HttpProxy::incomingConnection(qintptr fd) {
    QTcpSocket *local_socket = new QTcpSocket(this);
    connect(local_socket, &QTcpSocket::readyRead, this,
            &HttpProxy::onLocalReadyRead);
    connect(local_socket, &QTcpSocket::disconnected, local_socket,
            &QTcpSocket::deleteLater);
#if QT_VERSION >= 0x051500
    connect(local_socket, &QAbstractSocket::errorOccurred, this,
            &HttpProxy::onLocalSocketError);
#else
    connect(local_socket,
            SIGNAL(QAbstractSocket::error(QAbstractSocket::SocketError)), this,
            SLOT(HttpProxy::onLocalSocketError(QAbstractSocket::SocketError)));
#endif
    local_socket->setSocketDescriptor(fd);
    qDebug() << "new connection:" << local_socket->peerAddress()
             << local_socket->peerPort();
}

void HttpProxy::onLocalSocketError(QAbstractSocket::SocketError) {
    QTcpSocket *local_socket = qobject_cast<QTcpSocket *>(sender());
    if (local_socket->property("other").isValid()) {
        //        qDebug() << "seg fault here???";
        QTcpSocket *remote_socket =
            local_socket->property("other").value<QTcpSocket *>();
        remote_socket->close();
        remote_socket->deleteLater();
    }
    local_socket->close();
    local_socket->deleteLater();
}

void HttpProxy::onRemoteSocketError(QAbstractSocket::SocketError) {
    QTcpSocket *remote_socket = qobject_cast<QTcpSocket *>(sender());
    QTcpSocket *local_socket =
        qobject_cast<QTcpSocket *>(remote_socket->parent());
    local_socket->close();
    remote_socket->close();
    local_socket->deleteLater();
    remote_socket->deleteLater();
}

void HttpProxy::onLocalReadyRead() {
    QTcpSocket *local_socket = qobject_cast<QTcpSocket *>(sender());
    QTcpSocket *remote_socket = nullptr;

    // is tunneling
    if (local_socket->property("tunneling").isValid()) {
        QByteArray d = local_socket->readAll();
        remote_socket = local_socket->property("other").value<QTcpSocket *>();
        // assert(local_socket != remote_socket);
        qDebug() << "about to write to remote:" << remote_socket->state();
        if (remote_socket->isWritable())
            qDebug() << "wrote" << remote_socket->write(d)
                     << "bytes to remote_socket";
        else {
            qDebug() << "remote socket not writable";
        }
        // qDebug() << "wrote data to remote:" << d;
        return;
    }

    // not tunneling, parse request
    QByteArray reqData = local_socket->readAll();
    qDebug() << "reqData:" << reqData;
    int pos = reqData.indexOf("\r\n");
    // http request line
    QByteArray reqLine = reqData.left(pos);
    // other headers
    reqData.remove(0, pos + 2);

    QList<QByteArray> entries = reqLine.split(' ');
    QByteArray method = entries.value(0);
    QByteArray address = entries.value(1);
    QByteArray version = entries.value(2);

    // connect to server of host:port
    QString host;
    uint16_t port;
    // key = host:port, to identify a child of a socket,
    // which is the remote_socket (if the same local_socket triggers this slot)
    QString key;

    // type 1 proxy, only http
    if (method != "CONNECT") {
        QUrl url = QUrl::fromEncoded(address);
        if (!url.isValid()) {
            QDebug(QtMsgType::QtCriticalMsg) << "Invalid URL: " << url;
            local_socket->disconnectFromHost();
            return;
        }
        host = url.host();
        port = url.port(80);
        QString req = url.path();
        if (url.hasQuery()) {
            req.append('?').append(url.query());
        }
        reqLine = method + " " + req.toUtf8() + " " + version + "\r\n";
        reqData.prepend(reqLine);
        // TODO: review
        // do we need this key
        // local_socket : remote_socket -> 1 : 1 or 1 : n
        // if 1 : 1, use property "other"
        //        key = host + ':' + QString::number(port);
        //        remote_socket = local_socket->findChild<QTcpSocket *>(key);
        remote_socket = local_socket->property("other").value<QTcpSocket *>();
        if (remote_socket) {
            remote_socket->write(reqData);
            return;  // if we find an existing socket, then use it and return
        }
    } else {  // CONNECT method
        // type 2 proxy: http or https
        /**
         * 1. CONNECT HOST:PORT VERSION
         * 2. remote_socket connect to this host:port, reply to local_socket
         * "HTTP/1.0 200 Connection established\r\n\r\n";
         * 3. tunnel created
         */
        QList<QByteArray> host_port_list = address.split(':');
        host = QString(host_port_list.first());
        port = host_port_list.last().toUShort();
    }

    remote_socket = new QTcpSocket(local_socket);
    remote_socket->setProxy(socksProxy);
    QVariant other_socket;
    other_socket.setValue(remote_socket);
    local_socket->setProperty("other", other_socket);
    if (method != "CONNECT") {
        // type 1 http proxy: supports http only
        //        remote_socket->setObjectName(key);
        remote_socket->setProperty("reqData", reqData);
    } else {
        // type 2 http proxy: supports http & https
        local_socket->setProperty("tunneling", true);
        //        QVariant other_socket;
        //        other_socket.setValue(remote_socket);
        //        local_socket->setProperty("other", other_socket);
    }
    connect(remote_socket, &QTcpSocket::connected, this,
            &HttpProxy::onRemoteConnected);
    connect(remote_socket, &QTcpSocket::readyRead, this,
            &HttpProxy::onRemoteReadyRead);
    connect(remote_socket, &QTcpSocket::disconnected, remote_socket,
            &QTcpSocket::deleteLater);
#if QT_VERSION >= 0x051500
    connect(remote_socket, &QAbstractSocket::errorOccurred, this,
            &HttpProxy::onRemoteSocketError);
#else
    connect(remote_socket,
            SIGNAL(QAbstractSocket::error(QAbstractSocket::SocketError)), this,
            SLOT(HttpProxy::onRemoteSocketError(QAbstractSocket::SocketError)));
#endif
    qDebug() << "remote_socket connecting to: " << host << port;
    remote_socket->connectToHost(host, port);
}

void HttpProxy::onRemoteConnected() {
    QTcpSocket *remote_socket = qobject_cast<QTcpSocket *>(sender());
    qDebug() << "connected to remote: " << remote_socket->peerName()
             << remote_socket->peerPort();
    QTcpSocket *local_socket =
        qobject_cast<QTcpSocket *>(remote_socket->parent());
    if (local_socket->property("tunneling").isValid()) {
        static const QByteArray httpsHeader =
            "HTTP/1.0 200 Connection established\r\n\r\n";
        local_socket->write(httpsHeader);
        qDebug() << "https header sent";
    } else {
        QByteArray reqData = remote_socket->property("reqData").toByteArray();
        remote_socket->write(reqData);
        //        qDebug() << "reqData sent: " << reqData;
    }
}

void HttpProxy::onRemoteReadyRead() {
    QTcpSocket *remote_socket = qobject_cast<QTcpSocket *>(sender());
    QTcpSocket *local_socket =
        qobject_cast<QTcpSocket *>(remote_socket->parent());
    // assert(local_socket != remote_socket);
    QByteArray d = remote_socket->readAll();
    local_socket->write(d);
    //    qDebug() << "wrote data to local:" << d;
}
