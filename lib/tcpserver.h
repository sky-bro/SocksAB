#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include "cipher.h"
#include "tcprelay.h"
#include <QSet>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(int timeout, bool isLocal, QHostAddress serverAddr, quint16 serverPort, std::string method, std::string password);
    TcpServer(int timeout, bool isLocal, QString serverAddr, quint16 serverPort, std::string method, std::string password);
    TcpServer(int timeout, bool isLocal, QString serverAddr, quint16 serverPort, std::string method, std::string password, QString proxyAddr, quint16 proxyPort);
    bool listen(QString localAddr, quint16 localPort);
    ~TcpServer() override;

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    const int m_timeout;
    const bool m_isLocal;
    const QHostAddress m_serverAddr;
    const quint16 m_serverPort;
    const QHostAddress m_proxyAddr;
    const quint16 m_proxyPort;

    std::string m_method;
    std::string m_password;

    std::list<std::shared_ptr<TcpRelay> > m_conSet;
};

#endif // TCPSERVER_H
