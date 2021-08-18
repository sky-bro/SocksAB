#include <QApplication>
#include <QDateTime>
#include <QMutex>

#include "log.h"
#include "loginDialog.h"
#include "mainDialog.h"

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
