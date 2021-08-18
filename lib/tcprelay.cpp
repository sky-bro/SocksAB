#include "tcprelay.h"

TcpRelay::TcpRelay(QTcpSocket* localSocket, int timeout,
                   QHostAddress server_addr, quint16 server_port,
                   Cipher::CipherCreator get_cipher)
    : m_stage(INIT),
      m_serverAddr(server_addr),
      m_serverPort(server_port),
      m_cipher(get_cipher()),
      m_local(localSocket),
      m_remote(new QTcpSocket()),
      m_timer(new QTimer()) {
    m_timer->setInterval(timeout);
    connect(m_timer.get(), &QTimer::timeout, this, &TcpRelay::onTimeout);

    connect(m_local, &QAbstractSocket::errorOccurred, this,
            &TcpRelay::onLocalTcpSocketError);
    connect(m_local, &QTcpSocket::disconnected, this, &TcpRelay::close);
    connect(m_local, &QTcpSocket::readyRead, this,
            &TcpRelay::onLocalTcpSocketReadyRead);
    connect(m_local, &QTcpSocket::readyRead, m_timer.get(),
            static_cast<void (QTimer::*)()>(&QTimer::start));

    connect(m_remote, &QTcpSocket::connected, this,
            &TcpRelay::onRemoteConnected);
    connect(m_remote, &QAbstractSocket::errorOccurred, this,
            &TcpRelay::onRemoteTcpSocketError);
    connect(m_remote, &QTcpSocket::disconnected, this, &TcpRelay::close);
    connect(m_remote, &QTcpSocket::readyRead, this,
            &TcpRelay::onRemoteTcpSocketReadyRead);
    connect(m_remote, &QTcpSocket::readyRead, m_timer.get(),
            static_cast<void (QTimer::*)()>(&QTimer::start));

    m_local->setReadBufferSize(RemoteRecvSize);
    m_local->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    m_local->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    m_remote->setReadBufferSize(RemoteRecvSize);
    m_remote->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    m_remote->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    qInfo() << "TcpRelay Constructed!";
}

bool TcpRelay::writeToRemote(const char* data, size_t length) {
    return m_remote->write(data, length) != -1;
}

void TcpRelay::onRemoteConnected() {
    qDebug() << "remote connected, enter STREAM stage";
    if (!m_dataToWrite.empty()) {
        qDebug() << "writing saved data to remote:"
                 << QByteArray(m_dataToWrite.data(), m_dataToWrite.size());
        writeToRemote(m_dataToWrite.data(), m_dataToWrite.size());
        m_dataToWrite.clear();
    }
    m_stage = STREAM;
}

void TcpRelay::onRemoteTcpSocketError() {
    qDebug() << "onRemoteTcpSocketError";
    QString msg = "Remote socket: " + m_remote->errorString();
    // it's not an "error" if remote host closed a connection
    if (m_remote->error() == QAbstractSocket::RemoteHostClosedError) {
        qDebug() << msg;
    } else {
        qWarning() << msg;
    }
    m_local->flush();
    close();
}

void TcpRelay::onLocalTcpSocketError() {
    // it's not an "error" if remote host closed a connection
    qDebug() << "onLocalTcpSocketError";
    QString msg = "Local socket[" + m_local->peerAddress().toString() + ":" +
                  m_local->peerPort() + "]:" + m_local->errorString();
    if (m_local->error() == QAbstractSocket::RemoteHostClosedError) {
        qDebug().noquote() << msg;
    } else {
        qWarning().noquote() << msg;
    }
    m_remote->flush();
    close();
}

void TcpRelay::onLocalTcpSocketReadyRead() {
    std::string data;
    data.resize(RemoteRecvSize);
    int64_t readSize = m_local->read(&data[0], data.size());
    if (readSize == -1) {
        qWarning("Attempted to read from closed local socket, but failed.");
        close();
        return;
    }
    data.resize(readSize);

    if (data.empty()) {
        qWarning(
            "Attempted to read from closed local socket, but got nothing.");
        close();
        return;
    }
    handleLocalTcpData(data);
}

void TcpRelay::onRemoteTcpSocketReadyRead() {
    std::string data;
    data.resize(RemoteRecvSize);
    int64_t readSize = m_remote->read(&data[0], data.size());
    if (readSize == -1) {
        qWarning("Attempted to read from closed remote socket.");
        close();
        return;
    }
    data.resize(readSize);

    if (data.empty()) {
        // 什么时候会出现这种情况
        qWarning("received empty data from remote");
        close();
        return;
    }

    handleRemoteTcpData(data);
    m_local->write(data.data(), data.size());
}

void TcpRelay::onTimeout() {
    qWarning("time out triggered!");
    close();
}

void TcpRelay::close() {
    if (m_stage == DESTROYED) {
        qDebug("already destroyed");
        return;
    }
    m_remote->deleteLater();
    m_local->deleteLater();
    m_stage = DESTROYED;
    emit finished();
}
