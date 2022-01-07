#ifndef DIALOG_H
#define DIALOG_H

#include <QCloseEvent>
#include <QCoreApplication>
#include <QDialog>
#include <QHostAddress>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include "httpproxy.h"
#include "tcpserver.h"

// QT_BEGIN_NAMESPACE
namespace Ui {
class MainDialog;
}
// QT_END_NAMESPACE

enum USERTYPE { ADMIN, USER, VISITOR };

extern int COL_COUNT;
enum COLTYPE {
    COL_NAME = 0,
    COL_IP,
    COL_PORT,
    COL_METHOD,
    COL_PROXYIP,
    COL_PROXYPORT,
    COL_KEY
};

extern QString columns[];

class MainDialog : public QDialog {
    Q_OBJECT

  public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    virtual void closeEvent(QCloseEvent *event);

  private:
    USERTYPE usertype = VISITOR;
    Ui::MainDialog *ui;
    QSystemTrayIcon *mSystemTrayIcon;
    TcpServer tcpServer;
    HttpProxy httpServer;
    bool isStarted;  // current state is running
    // a server is set
    bool serverSet = false;
    // local and server config:
    QHostAddress m_localAddr;  // local addr
    quint16 m_localPort;       // local port
    quint16 m_httpport;
    QHostAddress m_serverAddr;
    quint16 m_serverPort;
    std::string m_method;
    std::string m_password;
    QHostAddress m_proxyAddr;
    quint16 m_proxyPort;

    QSystemTrayIcon::ActivationReason m_reason;
    QTimer *m_pTimer = nullptr;

    QBrush bgBrush;
    QBrush fgBrush;
    QMenu *menuConnection;

    void initTrayIcon();
    void initServerTable();
    void setState(bool isStarted);

    void readSettings();
    void writeSettings();
    void checkCurrentIndex(const QModelIndex &index);
    void setServer(int r);

    bool isValidConfig(const QJsonObject &config);
    QJsonObject getServerConfig(int r);
    void updateServerConfig(int r, const QJsonObject &config);

  public slots:
    void setUser(USERTYPE usertype);
    void reject();
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
    // from clipboard
    void onImport();
    // to clipboard
    void onExport();
    void onConnect();
    void onDisconnect();
    void onTestLatency();
    void onTestAllLatency();
    void onQuit();
};
#endif  // DIALOG_H
