#ifndef TCPRELAY_H
#define TCPRELAY_H

#include <QObject>
#include <QTcpSocket>
#include "cipher.h"
#include <QTimer>
#include <QHostAddress>
#include <QNetworkProxy>

class TcpRelay: public QObject {
  Q_OBJECT
 public:
  TcpRelay(QTcpSocket *localSocket, int timeout, QHostAddress server_addr, quint16 server_port, Cipher::CipherCreator get_cipher);
  virtual ~TcpRelay() {
    qInfo() << "TcpRelay destroyed";
  }

  enum STAGE {INIT, ADDR, UDP_ASSOC, DNS, CONNECTING, STREAM, DESTROYED };

 signals:
  void finished();

 protected:
  static const int64_t RemoteRecvSize = 65536;
  STAGE m_stage;
  std::string m_dataToWrite;
  QHostAddress m_serverAddr; // server bob's addr
  quint16 m_serverPort;
  QHostAddress m_remoteAddr; // real server addr
  std::unique_ptr<Cipher> m_cipher;
  std::unique_ptr<QTcpSocket> m_local;
  std::unique_ptr<QTcpSocket> m_remote;
  std::unique_ptr<QTimer> m_timer;

  bool writeToRemote(const char *data, size_t length);

//  virtual void handleStageAddr(std::string &data) = 0;
  virtual void handleLocalTcpData(std::string &data) = 0;
  virtual void handleRemoteTcpData(std::string &data) = 0;

 protected slots:
  void onRemoteConnected();

  void onRemoteTcpSocketError();
  void onLocalTcpSocketError();

  void onLocalTcpSocketReadyRead();
  void onRemoteTcpSocketReadyRead();

  void onTimeout();

  void close();
};

#endif // TCPRELAY_H
