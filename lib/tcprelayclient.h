#ifndef TCPRELAYCLIENT_H
#define TCPRELAYCLIENT_H

#include "address.h"
#include "tcprelay.h"

class TcpRelayClient : public TcpRelay {
    Q_OBJECT
  public:
    //  TcpRelayClient();
    TcpRelayClient(QTcpSocket *localSocket, int timeout,
                   QHostAddress server_addr, quint16 server_port,
                   Cipher::CipherCreator get_cipher);
    TcpRelayClient(QTcpSocket *localSocket, int timeout,
                   QHostAddress server_addr, quint16 server_port,
                   Cipher::CipherCreator get_cipher, QHostAddress proxy_addr,
                   quint16 proxy_port);
    ~TcpRelayClient();

  private:
    void handleStageINIT(std::string &data);
    void handleStageADDR(std::string &data);
    std::unique_ptr<Address> m_proxy_addr;

  protected:
    void handleLocalTcpData(std::string &data) final;
    void handleRemoteTcpData(std::string &data) final;
};

#endif  // TCPRELAYCLIENT_H
