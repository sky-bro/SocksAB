#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>

#include "log.h"
#include "tcpserver.h"

int main(int argc, char *argv[]) {
    qInstallMessageHandler(messageHandler);
    QNetworkProxyFactory::setUseSystemConfiguration(false);
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
         QCoreApplication::translate("main",
                                     "shared secret between Alice and Bob"),
         QCoreApplication::translate("main", "key")},
        {{"m", "method"},
         QCoreApplication::translate("main", "encryption method"),
         QCoreApplication::translate("main", "method")},
        {{"l", "logLevel"},
         QCoreApplication::translate(
             "main", "logLevel, level >= logLevel will be printed out"),
         QCoreApplication::translate("main", "logLevel")},
        {{"f", "logFile"},
         QCoreApplication::translate("main", "logFile path, default to stderr"),
         QCoreApplication::translate("main", "logFile")},
    });
    parser.process(a);
    QString str_ip = parser.value("ip"), str_port = parser.value("port"),
            str_key = parser.value("key"), str_method = parser.value("method"),
            str_logLevel = parser.value("logLevel"),
            logFilePath = parser.value("logFile");
    if (!logFilePath.isEmpty())
        logFile = fopen(logFilePath.toStdString().data(), "a");
    if (!str_logLevel.isEmpty()) logLevel = str_logLevel.toInt();
    if (str_ip.isEmpty()) str_ip = "0.0.0.0";
    if (str_port.isEmpty()) str_port = "1082";
    if (str_key.isEmpty()) str_key = "sky-io";
    if (str_method.isEmpty()) str_method = "aes-128-gcm";
    QHostAddress serverAddr(str_ip);
    quint16 serverPort = str_port.toUShort();
    // std::string password = str_key.toStdString();
    TcpServer tcpserver(30, false);

    if (tcpserver.listen(serverAddr, serverPort, str_method.toStdString(),
                         str_key.toStdString())) {
        qInfo("hello from Bob (Server Side), listening at %s:%d",
              qUtf8Printable(tcpserver.serverAddress().toString()),
              tcpserver.serverPort());
    } else {
        qCritical() << "fail to listen at" << (str_ip + ":" + str_port);
        return 1;
    }

    return QCoreApplication::exec();
}
