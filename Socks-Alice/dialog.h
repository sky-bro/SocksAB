#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenu>
#include <QCoreApplication>
#include <QHostAddress>
#include <QMap>

#include "tcpserver.h"
// #include <SocksAB/tcpserver.h>

#define cout qDebug() << "(" << __TIME__ << __FILE__ << "," << __FUNCTION__ << "," << __LINE__ << "," << this << ")" << ": "

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

enum USERTYPE {
    ADMIN,
    USER
};

enum COLTYPE {
    COL_NAME=0,
    COL_IP,
    COL_PORT,
    COL_METHOD,
    COL_PROXYIP,
    COL_PROXYPORT,
    COL_KEY
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr, USERTYPE usertype=USER);
    ~Dialog();
    virtual  void closeEvent(QCloseEvent *event);

private:
    USERTYPE usertype;
    Ui::Dialog *ui;
    QSystemTrayIcon *mSystemTrayIcon;
    TcpServer *tcpServer;
    bool isStarted;
    QHostAddress addr; // local addr
    quint16 port; // local port
    QBrush bgBrush;
    QMenu *menuConnection;

    void initTrayIcon();
    void initServerTable();
    void setState(bool isStarted);
    void setUser(USERTYPE usertype);
    void readSettings();
    void writeSettings();
    void checkCurrentIndex(const QModelIndex &index);
    void setServer(int r);

private slots:
    void trayiconActivated(QSystemTrayIcon::ActivationReason reason);
    void show_hide();
    void on_btnRun_clicked();
//    void on_btnApply_clicked();
//    void on_btnUpdate_clicked();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onEdit();
    void onAdd();
    void onDelete();
    void onConnect();
    void onDisconnect();
    void onTestLatency();
    void onTestAllLatency();
};
#endif // DIALOG_H
