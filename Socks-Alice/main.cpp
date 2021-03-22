#include "dialog.h"
#include "logindialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginDialog login_dialog;
    login_dialog.show();
//    Dialog w;
//    w.show();
    return a.exec();
}
