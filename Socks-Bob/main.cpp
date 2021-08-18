#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>

#include "tcpserver.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &msg) {
    QMutex mutex;
    QMutexLocker locker(&mutex);

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    QByteArray datetime = QDateTime::currentDateTime()
                              .toString("yyyy-MM-dd hh.mm.ss.zzz")
                              .toLocal8Bit();
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "%s Debug: %s (%s:%u, %s)\n", datetime.constData(),
                    localMsg.constData(), file, context.line, function);
            break;
        case QtInfoMsg:
            fprintf(stderr, "%s Info: %s (%s:%u, %s)\n", datetime.constData(),
                    localMsg.constData(), file, context.line, function);
            break;
        case QtWarningMsg:
            fprintf(stderr, "%s Warning: %s (%s:%u, %s)\n",
                    datetime.constData(), localMsg.constData(), file,
                    context.line, function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "%s Critical: %s (%s:%u, %s)\n",
                    datetime.constData(), localMsg.constData(), file,
                    context.line, function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "%s Fatal: %s (%s:%u, %s)\n", datetime.constData(),
                    localMsg.constData(), file, context.line, function);
            break;
    }
}

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
    });
    parser.process(a);
    QString str_ip = parser.value("ip"), str_port = parser.value("port"),
            str_key = parser.value("key");
    if (str_ip.isEmpty()) str_ip = "0.0.0.0";
    if (str_port.isEmpty()) str_port = "1082";
    if (str_key.isEmpty()) str_key = "sky-io";
    QHostAddress serverAddr(str_ip);
    quint16 serverPort = str_port.toUShort();
    QString method = "chacha20-ietf-poly1305";
    method = "aes-128-gcm";
    method = "aes-128-ctr";
    // std::string password = str_key.toStdString();
    TcpServer tcpserver(30, false);

    if (tcpserver.listen(serverAddr, serverPort, method.toStdString(),
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
