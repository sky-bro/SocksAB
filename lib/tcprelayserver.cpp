#include "tcprelayserver.h"
#include "address.h"

TcpRelayServer::TcpRelayServer(QTcpSocket *localSocket, int timeout, QHostAddress server_addr, quint16 server_port, Cipher::CipherCreator get_cipher):
  TcpRelay(localSocket, timeout, server_addr, server_port, get_cipher) {

}

// remote_addr | remote_port | proxy_addr | proxy_port |
void TcpRelayServer::handleStageADDR(std::string &data) {
  Address address;
  int len1 = address.update_from_data(data); // get proxy addr:port
  data = data.substr(len1);
  // TODO: optional proxy_addr ?
  Address proxy_addr(data); // remote addr
  if (address.m_addr_type == Address::NOTYPE || proxy_addr.m_addr_type == Address::NOTYPE) {
    qCritical("Can't parse header. Wrong encryption method or password?");
    close();
    return;
  }

  QDebug(QtMsgType::QtInfoMsg).noquote().nospace()
      << "Connecting " << address << " from "
      << m_local->peerAddress().toString() << ":" << m_local->peerPort();

  m_stage = CONNECTING;
  if (data.size() > proxy_addr.m_data.length()) {
    data = data.substr(proxy_addr.m_data.length());
    m_dataToWrite += data;
  }
  // no hostname for proxy for now...
  if (proxy_addr.m_data.length())
    m_remote->setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, proxy_addr.m_addr.toString(), proxy_addr.m_port));
  if (address.m_addr_type == Address::HOST) m_remote->connectToHost(address.m_hostname, address.m_port);
  else m_remote->connectToHost(address.m_addr, address.m_port);
}

void TcpRelayServer::handleLocalTcpData(std::string &data) {
  try {
    data = m_cipher->dec(data);
  } catch (const std::exception &e) {
    QDebug(QtMsgType::QtCriticalMsg) << "Local:" << e.what();
    close();
    return;
  }

  if (data.empty()) {
    qWarning("Data is empty after decryption.");
    return;
  }

  if (m_stage == STREAM) {
    writeToRemote(data.data(), data.size());
  } else if (m_stage == CONNECTING || m_stage == DNS) {
    // take DNS into account, otherwise some data will get lost
    m_dataToWrite += data;
  } else if (m_stage == INIT) {
    handleStageADDR(data);
  } else {
    qCritical("Local unknown stage.");
  }
}

void TcpRelayServer::handleRemoteTcpData(std::string &data) {
  data = m_cipher->enc(data);
}
