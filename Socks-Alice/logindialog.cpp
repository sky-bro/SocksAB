#include "logindialog.h"
#include "dialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
//    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // hide help button
//    setWindowFlags(Qt::FramelessWindowHint);
//    setWindowFlags( windowFlags() & ~Qt::WindowMaximizeButtonHint );
    ui->setupUi(this);
    setWindowTitle("登录Socks-Alice");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_btnLogin_clicked()
{
    QMap<QString, QString> userDB;
    userDB.insert("user", "password");
    userDB.insert("admin", "password");
    userDB.insert("sky", "sky");
    userDB.insert("", "");

    QSet<QString> admins = {
        "admin",
        "sky",
        ""
    };

    QString username = ui->usernameInput->currentText();
    QString password = ui->passwordInput->text();
    auto it = userDB.find(username);
    if (it == userDB.end() || it.value() != password) {
        qDebug() << "login failed!";
        QMessageBox::warning(this, "登录失败", "帐号或密码错误，请重试。");
        return;
    }

    USERTYPE usertype = USER;
    if (admins.contains(username)) {
        usertype = ADMIN;
    }

    qDebug() << "login success!";
    dialog = new Dialog(0, usertype);
    dialog->show();
    hide();
}
