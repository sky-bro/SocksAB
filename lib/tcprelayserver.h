#ifndef TCPRELAYSERVER_H
#define TCPRELAYSERVER_H

#include <memory>

#include "tcprelay.h"

class TcpRelayServer : public TcpRelay {
    Q_OBJECT
  public:
    TcpRelayServer();
    TcpRelayServer(std::unique_ptr<QTcpSocket> localSocket, int timeout,
                   QHostAddress server_addr, quint16 server_port,
                   Cipher::CipherCreator get_cipher);
    ~TcpRelayServer();

  private:
    void handleStageADDR(std::string &data);

  protected:
    void handleLocalTcpData(std::string &data) final;
    bool handleRemoteTcpData(std::string &data) final;
};

#endif  // TCPRELAYSERVER_H
