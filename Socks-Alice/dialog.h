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
#include "httpproxy.h"
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
    TcpServer tcpServer;
    HttpProxy httpServer;
    bool isStarted; // current state is running
    bool serverSet = false;
    // local and server config:
    QHostAddress m_localAddr; // local addr
    quint16 m_localPort; // local port
    quint16 m_httpport;
    QHostAddress m_serverAddr;
    quint16 m_serverPort;
    std::string m_method;
    std::string m_password;
    QHostAddress m_proxyAddr;
    quint16 m_proxyPort;

    QBrush bgBrush;
    QBrush fgBrush;
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
    void onQuit();
};
#endif // DIALOG_H
