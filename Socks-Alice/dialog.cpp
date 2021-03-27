#include "dialog.h"
#include "ui_dialog.h"
#include "editdialog.h"
#include <QSettings>


Dialog::Dialog(QWidget *parent, USERTYPE usertype)
    : QDialog(parent)
    , ui(new Ui::Dialog), tcpServer(nullptr)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    initTrayIcon();
    initServerTable();
    setUser(usertype);
    readSettings();
    setState(false);

    if (usertype == ADMIN) {
//        if (updateRoutes()) applyRoute();
//        setState(false);
    } else {
//        guardNodes = new QMap<QString, IP_Port>();
//        exitNodes = new QMap<QString, IP_Port>();
//        fetchRoutes(*guardNodes, *exitNodes);
//        tcpServer->setGuardAddr(guardNodes->first());
//        tcpServer->setExitAddr(*exitNodes->first());

        // 静态链路置灰

    }

}

Dialog::~Dialog()
{
    delete ui;
//    delete guardNodes;
//    delete exitNodes;
}

void Dialog::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("Exit"), QString(tr("Are you sure you want to exit?\nElse it will be hidden!")), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (button == QMessageBox::No) {
        event->ignore();  //忽略退出信号，程序继续运行
        hide();
    }
    else if (button == QMessageBox::Yes) {
         event->accept();  //接受退出信号，程序退出
        if (tcpServer) tcpServer->close();
        writeSettings();
        exit(0);
    } else {
        event->ignore();
    }
}

void Dialog::initTrayIcon()
{
    qDebug() << "init Tray Icon";
    mSystemTrayIcon = new QSystemTrayIcon(this);
    mSystemTrayIcon->setIcon(QIcon(":/images/heart.png"));
    mSystemTrayIcon->show();

    connect(mSystemTrayIcon, &QSystemTrayIcon::activated, this, &Dialog::trayiconActivated);

    QMenu *menu = new QMenu(this);

    QAction *show_hide_action = new QAction("Show/Hide", mSystemTrayIcon);
    connect(show_hide_action, &QAction::triggered, this, &Dialog::show_hide);
    QAction *quit_action = new QAction("Exit", mSystemTrayIcon);
    connect(quit_action, &QAction::triggered, this, &QCoreApplication::quit);

    menu->addAction(show_hide_action);
    menu->addAction(quit_action);

    mSystemTrayIcon->setContextMenu(menu);

}

void Dialog::initServerTable()
{
    ui->serverList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->serverList, &QTableWidget::customContextMenuRequested,
                this, &Dialog::onCustomContextMenuRequested);
    menuConnection = new QMenu(this);
    menuConnection->addAction(ui->actionAdd);
    menuConnection->addAction(ui->actionEdit);
    menuConnection->addAction(ui->actionDelete);
    menuConnection->addAction(ui->actionConnect);
    menuConnection->addAction(ui->actionDisconnect);
    menuConnection->addAction(ui->actionTestLatency);
    menuConnection->addAction(ui->actionTestAllLatency);
    connect(ui->actionAdd, &QAction::triggered, this, &Dialog::onAdd);
    connect(ui->actionEdit, &QAction::triggered, this, &Dialog::onEdit);
    connect(ui->actionDelete, &QAction::triggered, this, &Dialog::onDelete);
    connect(ui->actionConnect, &QAction::triggered, this, &Dialog::onConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &Dialog::onDisconnect);
    connect(ui->actionTestLatency, &QAction::triggered, this, &Dialog::onTestLatency);
    connect(ui->actionTestAllLatency, &QAction::triggered, this, &Dialog::onTestAllLatency);
}

void Dialog::setState(bool isStarted)
{
    this->isStarted = isStarted;
    if (isStarted) {
        // server running
        ui->btnRun->setText("Stop");
//        ui->lineEditIP->setEnabled(false);
//        ui->spinBoxSocksPort->setEnabled(false);
        mSystemTrayIcon->setToolTip(tr("Socks-Alice(Running)，listening at -- %0:%1").arg(addr.toString()).arg(port));
    } else {
        // server stopped
        ui->btnRun->setText("Run");
//        ui->lineEditIP->setEnabled(true);
//        ui->spinBoxPort->setEnabled(true);
        mSystemTrayIcon->setToolTip(tr("Socks-Alice(Stopped)"));
    }
}

void Dialog::setUser(USERTYPE usertype)
{
    this->usertype = usertype;
    // 2: admin tab
    // 1: user tab
    // if admin, hide user tab
    // alawys ADMIN for now
    int index /*= ADMIN*/;
    index = usertype == ADMIN ? 1 : 2;

    ui->tabWidget->setTabVisible(index, false);
}

void Dialog::readSettings()
{
    QSettings settings("sky-bro", "Socks-Alice");

    resize(settings.value("window/size", QSize(500, 300)).toSize());

    ui->lineEditIP->setText(settings.value("local/ip", "127.1").toString());
    ui->spinBoxSocksPort->setValue(settings.value("local/socks_port", 1081).toInt());
    ui->spinBoxHttpPort->setValue(settings.value("local/http_port", 1080).toInt());

    int size = settings.beginReadArray("servers");
    bool flag = true;
    for (int r = 0; r < size; ++r) {
        settings.setArrayIndex(r);
        ui->serverList->insertRow(r);
        ui->serverList->setItem(r, COL_NAME, new QTableWidgetItem(settings.value("name").toString()));
        ui->serverList->setItem(r, COL_IP, new QTableWidgetItem(settings.value("ip").toString()));
        ui->serverList->setItem(r, COL_PORT, new QTableWidgetItem(settings.value("port").toString()));
        ui->serverList->setItem(r, COL_METHOD, new QTableWidgetItem(settings.value("method").toString()));
        ui->serverList->setItem(r, COL_PROXYIP, new QTableWidgetItem(settings.value("proxy ip").toString()));
        ui->serverList->setItem(r, COL_PROXYPORT, new QTableWidgetItem(settings.value("proxy port").toString()));
        ui->serverList->setItem(r, COL_KEY, new QTableWidgetItem(settings.value("key").toString()));
        if (flag && settings.value("highlight").toString() == "true") {
            flag = false;
            setServer(r);
        }
    }
    settings.endArray();
}

void Dialog::writeSettings()
{
    QSettings settings("sky-bro", "Socks-Alice");

    settings.setValue("window/size", size());

    settings.setValue("local/ip", ui->lineEditIP->text());
    settings.setValue("local/socks_port", ui->spinBoxSocksPort->value());
    settings.setValue("local/http_port", ui->spinBoxHttpPort->value());

    settings.beginWriteArray("servers");
    bool flag = true;
    for (int i = 0; i < ui->serverList->rowCount(); ++i) {
        settings.setArrayIndex(i);
        if (flag && ui->serverList->item(i, 0)->background().color() == Qt::green) {
            flag = false;
            settings.setValue("highlight", "true");
        } else {
            settings.setValue("highlight", "false");
        }
        settings.setValue("name", ui->serverList->item(i, COL_NAME)->text());
        settings.setValue("ip", ui->serverList->item(i, COL_IP)->text());
        settings.setValue("port", ui->serverList->item(i, COL_PORT)->text());
        settings.setValue("method", ui->serverList->item(i, COL_METHOD)->text());
        settings.setValue("proxy ip", ui->serverList->item(i, COL_PROXYIP)->text());
        settings.setValue("proxy port", ui->serverList->item(i, COL_PROXYPORT)->text());
        settings.setValue("key", ui->serverList->item(i, COL_KEY)->text());
    }
    settings.endArray();

    cout << "settings written to " << settings.fileName();
}

void Dialog::checkCurrentIndex(const QModelIndex &index)
{
    bool valid = index.isValid(); // on a row
    bool connected = false;
    if (valid) {
        int row = ui->serverList->currentRow();
        connected = ui->serverList->item(row, COL_NAME)->background().color() == Qt::green;
    }
    ui->actionTestLatency->setEnabled(valid);
    ui->actionEdit->setEnabled(valid && !connected);
    ui->actionDelete->setEnabled(valid && !connected);
    ui->actionConnect->setEnabled(valid && !connected && !tcpServer);
    ui->actionDisconnect->setEnabled(valid && connected);
}

void Dialog::setServer(int r)
{
    if (tcpServer) return; // please disconnect first
    QString serverip = ui->serverList->item(r, COL_IP)->text();
    quint16 serverport = ui->serverList->item(r, COL_PORT)->text().toUShort();
    std::string method = ui->serverList->item(r, COL_METHOD)->text().toStdString();
    std::string key = ui->serverList->item(r, COL_KEY)->text().toStdString();
    QString proxyip = ui->serverList->item(r, COL_PROXYIP)->text();
    quint16 proxyport = ui->serverList->item(r, COL_PROXYPORT)->text().toUShort();
    tcpServer = new TcpServer(10, true, serverip, serverport, method, key, proxyip, proxyport);
    bgBrush = ui->serverList->item(r, 0)->background();
    ui->serverList->item(r, 0)->setBackground(Qt::green);
}

void Dialog::trayiconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        show_hide();
//        this->showNormal();
//        this->raise();
        break;
    default:
        break;
    }
}

void Dialog::show_hide()
{
    isVisible() ? hide() : show();
}


void Dialog::on_btnRun_clicked()
{
    if (!isStarted) {
        // start server
        addr = QHostAddress(ui->lineEditIP->text());
//        addr = QHostAddress("127.0.0.1");
        port = ui->spinBoxSocksPort->value();
//        cout << "starting server @" << addr << port;
        if (tcpServer && tcpServer->QTcpServer::listen(addr, port)) {
            mSystemTrayIcon->showMessage(tr("Listening"), tr("listening at -- %0:%1").arg(addr.toString()).arg(port), QSystemTrayIcon::Information, 5000);
            setState(true);
        } else {
            mSystemTrayIcon->showMessage(tr("Run Failed"), tr("Please check your ip/port"), QSystemTrayIcon::Warning, 3000);
        }
    } else {
        // stop server
        tcpServer->close();
        setState(false);
    }
}

void Dialog::onCustomContextMenuRequested(const QPoint &pos)
{
    checkCurrentIndex(ui->serverList->indexAt(pos));
    menuConnection->popup(ui->serverList->viewport()->mapToGlobal(pos));
}

void Dialog::onEdit()
{
    int row = ui->serverList->currentRow(); // start from 0
    EditDialog *editDlg = new EditDialog(ui->serverList, row, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
    if (editDlg->exec()) {
//            configHelper->save(*model);
    }
}

void Dialog::onAdd()
{
    int row = ui->serverList->rowCount();
//    ui->serverList->insertRow(row);
    EditDialog *editDlg = new EditDialog(ui->serverList, row, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
    if (editDlg->exec()) {
//            configHelper->save(*model);
    }
}

void Dialog::onDelete()
{
    int row = ui->serverList->currentRow();
    ui->serverList->removeRow(row);
}

void Dialog::onConnect()
{
    int row = ui->serverList->currentRow();
    setServer(row);
    if (!isStarted) on_btnRun_clicked();
}

void Dialog::onDisconnect()
{
    int row = ui->serverList->currentRow();
    if (ui->serverList->item(row, COL_NAME)->background().color() == Qt::green) {
        if (isStarted) on_btnRun_clicked();
        delete tcpServer;
        tcpServer = nullptr;
        ui->serverList->item(row, COL_NAME)->setBackground(bgBrush);
    }
}

void Dialog::onTestLatency()
{
    // TODO
}

void Dialog::onTestAllLatency()
{
    // TODO
}
