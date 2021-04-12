#include "tcprelayclient.h"
#include "address.h"

TcpRelayClient::TcpRelayClient(QTcpSocket *localSocket, int timeout, QHostAddress server_addr, quint16 server_port, Cipher::CipherCreator get_cipher):
  TcpRelay(localSocket, timeout, server_addr, server_port, get_cipher), m_proxy_addr(nullptr) {

}

TcpRelayClient::TcpRelayClient(QTcpSocket *localSocket, int timeout, QHostAddress server_addr, quint16 server_port, Cipher::CipherCreator get_cipher, QHostAddress proxy_addr, quint16 proxy_port):
    TcpRelay(localSocket, timeout, server_addr, server_port, get_cipher), m_proxy_addr(new Address(proxy_addr, proxy_port))
{

}

void TcpRelayClient::handleStageINIT(std::string &data) {
  qDebug() << "TcpRelayClient::handleStageINIT";
  static constexpr const char reject_data [] = { 0, 91 };
  static constexpr const char accept_data [] = { 5, 0 };
  static const QByteArray reject(reject_data, 2);
  static const QByteArray accept(accept_data, 2);
  if (data[0] != char(5)) {
    qCritical("An invalid socket connection was rejected. "
              "Please make sure the connection type is SOCKS5.");
    m_local->write(reject);
    qDebug() << "socks init rejected";
  } else {
    qDebug() << "socks init accepted";
    m_local->write(accept);
  }
  m_stage = ADDR;
}

/*
 * send addr to remote server (Bob)
 */
void TcpRelayClient::handleStageADDR(std::string &data) {
  auto cmd = static_cast<int>(data.at(1));
  if (cmd == 3) {//CMD_UDP_ASSOCIATE
    qDebug("UDP associate");
    static const char header_data [] = { 5, 0, 0 };
    QHostAddress addr = m_local->localAddress();
    uint16_t port = m_local->localPort();
    Address address(addr, port);
    std::string toWrite = std::string(header_data, 3) + address.m_data;
    m_local->write(toWrite.data(), toWrite.length());
    m_stage = UDP_ASSOC;
    return;
  }
  if (cmd == 1) {// CMD_CONNECT
    qDebug() << "SOCKS CMD_CONNECT";
    data = data.substr(3);
  } else {
    qCritical("Unknown command %d", cmd);
    close();
    return;
  }

  Address address(data);
  if (address.m_addr_type == Address::NOTYPE) {
    qCritical("Can't parse header. Wrong encryption method or password?");
    close();
    return;
  }

  QDebug(QtMsgType::QtInfoMsg).noquote().nospace()
      << "Connecting " << address << " from "
      << m_local->peerAddress().toString() << ":" << m_local->peerPort();

//  m_stage = DNS;
  m_stage = CONNECTING;
  // VER | REP | RSV | ATYP | BIND.ADDR | BIND.PORT
  // socks version 5 | success | 0 | ipv4 | 00 00 00 00 | 00 00
  // BIND.ADDR : BIND.PORT is the address used by socks server to connect to the real server
  // socks client does not have to know the actual address, so here all zeros for bind.addr:bind.port
  static constexpr const char res [] = { 5, 0, 0, 1, 0, 0, 0, 0, 0, 0 };
  static const QByteArray response(res, 10);
  m_local->write(response);
  if (m_proxy_addr) data.append(m_proxy_addr->m_data);
  m_dataToWrite += m_cipher->enc(data);
  m_remote->connectToHost(m_serverAddr, m_serverPort);
}

void TcpRelayClient::handleLocalTcpData(std::string &data) {
    qDebug() << "TcpRelayClient::handleLocalTcpData";
  switch (m_stage) {
  case INIT:
    handleStageINIT(data);
    break;
  case ADDR:
    handleStageADDR(data);
    break;
  case CONNECTING:
  case DNS:
    m_dataToWrite += m_cipher->enc(data);
    break;
  case STREAM:
    data = m_cipher->enc(data);
    writeToRemote(data.data(), data.size());
    break;
  default:
    qCritical("Local unknown stage.");
  }

}

void TcpRelayClient::handleRemoteTcpData(std::string &data) {
    qDebug() << "TcpRelayClient::handleRemoteTcpData";
  data = m_cipher->dec(data);
}
