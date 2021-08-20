#include "tcprelayserver.h"

#include "address.h"

TcpRelayServer::TcpRelayServer(std::unique_ptr<QTcpSocket> localSocket,
                               int timeout, QHostAddress server_addr,
                               quint16 server_port,
                               Cipher::CipherCreator get_cipher)
    : TcpRelay(std::move(localSocket), timeout, server_addr, server_port,
               get_cipher) {
    qInfo("TcpRelayServer Constructed!");
}

TcpRelayServer::~TcpRelayServer() { qInfo("TcpRelayServer Destructed!"); }

// remote_addr | remote_port | proxy_addr | proxy_port |
void TcpRelayServer::handleStageADDR(std::string &data) {
    Address address;
    int len1 = address.update_from_data(data);  // get proxy addr:port
    data = data.substr(len1);
    // TODO: optional proxy_addr ?
    Address proxy_addr(data);  // remote addr
    if (address.m_addr_type == Address::NOTYPE ||
        proxy_addr.m_addr_type == Address::NOTYPE) {
        qCritical("Can't parse header. Wrong encryption method or password?");
        close();
        return;
    }

    qDebug().noquote() << "Connecting" << address << "from"
                       << m_local->peerAddress().toString() + ":" +
                              QString::number(m_local->peerPort());

    m_stage = CONNECTING;
    if (data.size() > proxy_addr.m_data.length()) {
        data = data.substr(proxy_addr.m_data.length());
        m_dataToWrite += data;
    }
    // no hostname for proxy for now...
    if (proxy_addr.m_port != 0) {
        if (proxy_addr.m_addr_type == Address::HOST)
            m_remote->setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy,
                                             proxy_addr.m_hostname,
                                             proxy_addr.m_port));

        else
            m_remote->setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy,
                                             proxy_addr.m_addr.toString(),
                                             proxy_addr.m_port));
        qDebug() << "set proxy:" << proxy_addr;
    }
    if (address.m_addr_type == Address::HOST)
        m_remote->connectToHost(address.m_hostname, address.m_port);
    else
        m_remote->connectToHost(address.m_addr, address.m_port);
}

void TcpRelayServer::handleLocalTcpData(std::string &data) {
    qDebug() << "handleLocalTcpData";
    try {
        data = m_cipher->dec(data);
    } catch (const std::exception &e) {
        qWarning() << "Exception occurred decrypting Local data: " << e.what();
        close();
        return;
    }

    if (data.empty()) {
        // 可能需要更多数据才能解密
        qWarning("Data is empty after decryption.");
        return;
    }
    qDebug() << "got local data after dec:"
             << QByteArray(data.data(), data.size());

    if (m_stage == STREAM) {
        writeToRemote(data.data(), data.size());
    } else if (m_stage == CONNECTING || m_stage == DNS) {
        // take DNS into account, otherwise some data will get lost
        qDebug("still CONNECTING, save data for later");
        m_dataToWrite += data;
    } else if (m_stage == INIT) {
        handleStageADDR(data);
    } else {
        qCritical("Local unknown stage.");
    }
}

bool TcpRelayServer::handleRemoteTcpData(std::string &data) {
    qDebug() << "handleRemoteTcpData, before encryption:"
             << QByteArray(data.data(), data.size());
    data = m_cipher->enc(data);
    return true;
}
