#include "tcpserver.h"
#include "tcprelayclient.h"
#include "tcprelayserver.h"
#include <QTcpSocket>

//TcpServer::TcpServer(int timeout, bool isLocal, QHostAddress serverAddr, quint16 serverPort, std::string method, std::string password) :
//  m_timeout(timeout), m_isLocal(isLocal), m_serverAddr(serverAddr), m_serverPort(serverPort), m_password(password), m_method(method), m_proxyPort(0) {
//    setMaxPendingConnections(FD_SETSIZE);
//}

//TcpServer::TcpServer(int timeout, bool isLocal, QString serverAddr, quint16 serverPort, std::string method, std::string password)
//    : TcpServer(timeout, isLocal, QHostAddress(serverAddr), serverPort, method, password) {
//}

TcpServer::TcpServer(int timeout, bool isLocal) : m_timeout(timeout), m_isLocal(isLocal) {
    setMaxPendingConnections(FD_SETSIZE);
}

bool TcpServer::listen(const QHostAddress &localAddr, quint16 localPort, const QHostAddress& serverAddr, quint16 serverPort, std::string method, std::string password, const QHostAddress& proxyAddr, quint16 proxyPort) {
    // local: socks5 proxy and http proxy share same port
    // TODO
    m_serverAddr = serverAddr;
    m_serverPort = serverPort;
    m_password = password;
    m_method = method;
    m_proxyAddr = proxyAddr;
    m_proxyPort = proxyPort;
    qDebug() << "TcpServer::listen: " << m_serverAddr << m_serverPort << m_proxyAddr << m_proxyPort;

    return QTcpServer::listen(localAddr, localPort);
}

bool TcpServer::listen(const QHostAddress &serverAddr, quint16 serverPort, std::string method, std::string password) {
    m_serverAddr = serverAddr;
    m_serverPort = serverPort;
    m_method = method;
    m_password = password;

    return QTcpServer::listen(m_serverAddr, m_serverPort);
}

TcpServer::~TcpServer() {
  if (isListening()) close();
}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
  auto localSocket = std::make_unique<QTcpSocket>();
  localSocket->setSocketDescriptor(socketDescriptor);
  qInfo() << "New connection from " << localSocket->peerAddress() << ":" << localSocket->peerPort();
  //timeout * 1000: convert sec to msec
  TcpRelay* con;
  if (m_isLocal) {
    con = new TcpRelayClient(localSocket.release(),
                                           m_timeout * 1000,
                                           m_serverAddr, m_serverPort,
    [this]() {
      return std::make_unique<Cipher>(m_method, m_password);
    }, m_proxyAddr, m_proxyPort);
  } else {
    con = new TcpRelayServer(localSocket.release(),
                                           m_timeout * 1000,
                                           m_serverAddr, m_serverPort,
    [this]() {
      return std::make_unique<Cipher>(m_method, m_password);
    });
  }
  m_conSet.push_back(con);
  connect(con, &TcpRelay::finished, this, [con, this]() {
    qInfo() << "con removed from m_conSet";
    m_conSet.remove(con);
    con->deleteLater();
  });
}
