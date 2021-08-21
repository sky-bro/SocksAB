#include "tcpserver.h"

#include <QTcpSocket>

#include "tcprelayclient.h"
#include "tcprelayserver.h"

TcpServer::TcpServer(int timeout, bool isLocal)
    : m_timeout(timeout), m_isLocal(isLocal) {
    setMaxPendingConnections(MaxPendingConnections);
    qInfo() << "TcpServer Constructed!";
}

TcpServer::~TcpServer() {
    if (isListening()) close();
    qInfo("TcpServer destructed!");
}

bool TcpServer::listen(const QHostAddress& localAddr, quint16 localPort,
                       const QHostAddress& serverAddr, quint16 serverPort,
                       std::string method, std::string password,
                       const QHostAddress& proxyAddr, quint16 proxyPort) {
    m_serverAddr = serverAddr;
    m_serverPort = serverPort;
    m_password = password;
    m_method = method;
    m_proxyAddr = proxyAddr;
    m_proxyPort = proxyPort;
    qInfo() << "listening at -"
            << m_serverAddr.toString() + ":" + QString::number(m_serverPort)
            << m_proxyAddr.toString() + ":" + QString::number(m_proxyPort);

    return QTcpServer::listen(localAddr, localPort);
}

bool TcpServer::listen(const QHostAddress& serverAddr, quint16 serverPort,
                       std::string method, std::string password) {
    m_serverAddr = serverAddr;
    m_serverPort = serverPort;
    m_method = method;
    m_password = password;

    qInfo() << "listening at -"
            << m_serverAddr.toString() + ":" + QString::number(m_serverPort);

    return QTcpServer::listen(m_serverAddr, m_serverPort);
}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
    std::unique_ptr<QTcpSocket> localSocket = std::make_unique<QTcpSocket>();
    localSocket->setSocketDescriptor(socketDescriptor);
    QHostAddress peerAddress = localSocket->peerAddress();
    quint16 peerPort = localSocket->peerPort();
    qInfo() << "New connection from"
            << peerAddress.toString() + ":" + QString::number(peerPort);
    // timeout * 1000: convert sec to msec
    TcpRelay* tcpRelay;
    if (m_isLocal) {
        tcpRelay = new TcpRelayClient(
            std::move(localSocket), m_timeout * 1000, m_serverAddr,
            m_serverPort,
            [this]() { return std::make_unique<Cipher>(m_method, m_password); },
            m_proxyAddr, m_proxyPort);
    } else {
        tcpRelay = new TcpRelayServer(std::move(localSocket), m_timeout * 1000,
                                      m_serverAddr, m_serverPort, [this]() {
                                          return std::make_unique<Cipher>(
                                              m_method, m_password);
                                      });
    }
    m_tcpRelays.emplace_back(tcpRelay);
    connect(tcpRelay, &TcpRelay::finished, this, [tcpRelay, this]() {
        qInfo() << "tcprelay removed from m_conSet";
        m_tcpRelays.remove(tcpRelay);
        tcpRelay->deleteLater();
    });
}
