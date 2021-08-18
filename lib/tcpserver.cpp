#include "tcpserver.h"

#include <QTcpSocket>

#include "tcprelayclient.h"
#include "tcprelayserver.h"

TcpServer::TcpServer(int timeout, bool isLocal)
    : m_timeout(timeout), m_isLocal(isLocal) {
    setMaxPendingConnections(FD_SETSIZE);
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
    qInfo() << "TcpServer::listen: " << m_serverAddr << m_serverPort
            << m_proxyAddr << m_proxyPort;

    return QTcpServer::listen(localAddr, localPort);
}

bool TcpServer::listen(const QHostAddress& serverAddr, quint16 serverPort,
                       std::string method, std::string password) {
    m_serverAddr = serverAddr;
    m_serverPort = serverPort;
    m_method = method;
    m_password = password;

    return QTcpServer::listen(m_serverAddr, m_serverPort);
}

TcpServer::~TcpServer() {
    if (isListening()) close();
    qInfo("TcpServer destructed!");
}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket* localSocket = new QTcpSocket();
    localSocket->setSocketDescriptor(socketDescriptor);
    QHostAddress peerAddress = localSocket->peerAddress();
    quint16 peerPort = localSocket->peerPort();
    qInfo() << "New connection from" << peerAddress.toString() + ":" + peerPort;
    // timeout * 1000: convert sec to msec
    TcpRelay* con;
    if (m_isLocal) {
        con = new TcpRelayClient(
            localSocket, m_timeout * 1000, m_serverAddr, m_serverPort,
            [this]() { return std::make_unique<Cipher>(m_method, m_password); },
            m_proxyAddr, m_proxyPort);
    } else {
        con = new TcpRelayServer(localSocket, m_timeout * 1000, m_serverAddr,
                                 m_serverPort, [this]() {
                                     return std::make_unique<Cipher>(
                                         m_method, m_password);
                                 });
    }
    m_conSet.emplace_back(con);
    connect(con, &TcpRelay::finished, this, [con, this]() {
        qInfo() << "con removed from m_conSet";
        m_conSet.remove(con);
        con->deleteLater();
    });
}
