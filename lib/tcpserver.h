#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QSet>
#include <QTcpServer>

#include "cipher.h"
#include "tcprelay.h"

#ifndef MaxPendingConnections
#define MaxPendingConnections 1024
#endif

class TcpServer : public QTcpServer {
    Q_OBJECT
  public:
    TcpServer(int timeout, bool isLocal);
    // alice (client) listen
    bool listen(const QHostAddress& localAddr, quint16 localPort,
                const QHostAddress& serverAddr, quint16 serverPort,
                std::string method, std::string password,
                const QHostAddress& proxyAddr = QHostAddress::Any,
                quint16 proxyPort = 0);
    // bob (server) listen
    bool listen(const QHostAddress& serverAddr, quint16 serverPort,
                std::string method, std::string password);
    ~TcpServer() override;

  protected:
    void incomingConnection(qintptr socketDescriptor) override;

  private:
    const int m_timeout;
    const bool m_isLocal;
    QHostAddress m_serverAddr;
    quint16 m_serverPort;
    std::string m_password;
    std::string m_method;
    QHostAddress m_proxyAddr;
    quint16 m_proxyPort;

    std::list<TcpRelay*> m_tcpRelays;
};

#endif  // TCPSERVER_H
