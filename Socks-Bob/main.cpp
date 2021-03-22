#include <QCoreApplication>
#include <QCommandLineParser>
#include "tcpserver.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addOptions({
        {{"i", "ip"},
            QCoreApplication::translate("main", "ip address of Socks-Bob"),
            QCoreApplication::translate("main", "ip")},
        {{"p", "port"},
            QCoreApplication::translate("main", "port of Socks-Bob"),
            QCoreApplication::translate("main", "port")},
        {{"k", "key"},
            QCoreApplication::translate("main", "shared secret between Alice and Bob"),
            QCoreApplication::translate("main", "key")},
     });
  parser.process(a);
  QString str_ip = parser.value("ip"), str_port = parser.value("port"), str_key = parser.value("key");
  if (str_ip.isEmpty()) str_ip = "0.0.0.0";
  if (str_port.isEmpty()) str_port = "1082";
  if (str_key.isEmpty()) str_key = "sky-io";
  QString serverAddr(str_ip);
  quint16 serverPort  = str_port.toUShort();
  QString method = "chacha20-ietf-poly1305";
  method = "aes-128-gcm";
  method = "aes-128-ctr";
  // std::string password = str_key.toStdString();
  TcpServer tcpserver(30, false, serverAddr, serverPort, method.toStdString(), str_key.toStdString());
  qDebug() << "hello from Bob (Server Side)" << ", listening at" << (str_ip + ":" + str_port);
  tcpserver.listen(serverAddr, serverPort);

  return a.exec();
}
