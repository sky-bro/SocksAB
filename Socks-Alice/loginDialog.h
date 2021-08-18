#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "mainDialog.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
    Q_OBJECT

  public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

  signals:
    void loginSuccess(USERTYPE usertype);

  private slots:
    void on_btnLogin_clicked();

  private:
    Ui::LoginDialog *ui;
};

#endif  // LOGINDIALOG_H
