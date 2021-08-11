#include "loginDialog.h"

#include <QMessageBox>

#include "mainDialog.h"
#include "ui_loginDialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog) {
    //    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); //
    //    hide help button setWindowFlags(Qt::FramelessWindowHint);
    //    setWindowFlags( windowFlags() & ~Qt::WindowMaximizeButtonHint );
    ui->setupUi(this);
}

LoginDialog::~LoginDialog() {
    delete ui;
    qInfo("LoginDialog destructed!");
}

void LoginDialog::on_btnLogin_clicked() {
    QMap<QString, QString> userDB;
    userDB.insert("user", "password");
    userDB.insert("admin", "password");
    userDB.insert("sky", "sky");
    userDB.insert("", "");

    QSet<QString> admins = {"admin", "sky", ""};

    QString username = ui->usernameInput->currentText();
    QString password = ui->passwordInput->text();
    auto it = userDB.find(username);
    if (it == userDB.end() || it.value() != password) {
        qWarning("login failed using {%s, %s}", qPrintable(username),
                 qPrintable(password));
        QMessageBox::warning(this, "login failed",
                             "please check your account/password!");
        return;
    }
    bool isAdmin = admins.contains(username);
    emit loginSuccess(isAdmin ? ADMIN : USER);
    qInfo("login succeeded as %s", isAdmin ? "USER" : "ADMIN");
    close();
}
