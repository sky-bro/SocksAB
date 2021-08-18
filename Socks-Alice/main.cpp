#include <QApplication>
#include <QDateTime>
#include <QMutex>

#include "loginDialog.h"
#include "mainDialog.h"

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
    QApplication a(argc, argv);
    USERTYPE usertype = USER;
    LoginDialog loginDialog;
    loginDialog.show();
    MainDialog mainDialog;
    QObject::connect(&loginDialog, &LoginDialog::loginSuccess, &mainDialog,
                     &MainDialog::setUser);
    return a.exec();
}
