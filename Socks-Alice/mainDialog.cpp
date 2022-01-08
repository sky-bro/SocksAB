#include "mainDialog.h"

#include <QClipboard>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

#include "loginDialog.h"
#include "serverEditDialog.h"
#include "ui_mainDialog.h"

int COL_COUNT = 7;
QString columns[] = {"name",    "ip",        "port", "method",
                     "proxyIp", "proxyPort", "key"};

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::MainDialog),
      tcpServer(10, true),
      httpServer() {
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    // socks5 proxy must be checked
    // http proxy relies on socks5 proxy
    initTrayIcon();
    initServerTable();
    setState(false);
    readSettings();
}

MainDialog::~MainDialog() {
    delete ui;
    qInfo("Main Dialog destructed!");
}

void MainDialog::closeEvent(QCloseEvent *event) {
    event->ignore();
    hide();
}

void MainDialog::initTrayIcon() {
    qDebug() << "init Tray Icon";
    mSystemTrayIcon = new QSystemTrayIcon(this);
    mSystemTrayIcon->setIcon(QIcon(":/images/SocksAB.png"));

    connect(mSystemTrayIcon, &QSystemTrayIcon::activated, this,
            &MainDialog::trayiconActivated);

    QMenu *menu = new QMenu(this);

    QAction *run_stop_action = new QAction("Run", mSystemTrayIcon);
    connect(run_stop_action, &QAction::triggered, this,
            &MainDialog::on_btnRun_clicked);
    QAction *show_hide_action = new QAction("Show/Hide", mSystemTrayIcon);
    connect(show_hide_action, &QAction::triggered, this,
            &MainDialog::show_hide);
    QAction *quit_action = new QAction("Exit", mSystemTrayIcon);
    connect(quit_action, &QAction::triggered, this, &MainDialog::onQuit);

    menu->addAction(run_stop_action);
    menu->addAction(show_hide_action);
    menu->addAction(quit_action);

    mSystemTrayIcon->setContextMenu(menu);
}

void MainDialog::initServerTable() {
    ui->serverList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->serverList, &QTableWidget::customContextMenuRequested, this,
            &MainDialog::onCustomContextMenuRequested);
    menuConnection = new QMenu(this);
    menuConnection->addAction(ui->actionAdd);
    menuConnection->addAction(ui->actionEdit);
    menuConnection->addAction(ui->actionImport);
    menuConnection->addAction(ui->actionExport);
    menuConnection->addAction(ui->actionDelete);
    menuConnection->addAction(ui->actionConnect);
    menuConnection->addAction(ui->actionDisconnect);
    //    menuConnection->addAction(ui->actionTestLatency);
    //    menuConnection->addAction(ui->actionTestAllLatency);
    connect(ui->actionAdd, &QAction::triggered, this, &MainDialog::onAdd);
    connect(ui->actionEdit, &QAction::triggered, this, &MainDialog::onEdit);
    connect(ui->actionImport, &QAction::triggered, this, &MainDialog::onImport);
    connect(ui->actionExport, &QAction::triggered, this, &MainDialog::onExport);
    connect(ui->actionDelete, &QAction::triggered, this, &MainDialog::onDelete);
    connect(ui->actionConnect, &QAction::triggered, this,
            &MainDialog::onConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this,
            &MainDialog::onDisconnect);
    //    connect(ui->actionTestLatency, &QAction::triggered, this,
    //            &MainDialog::onTestLatency);
    //    connect(ui->actionTestAllLatency, &QAction::triggered, this,
    //            &MainDialog::onTestAllLatency);
}

void MainDialog::setState(bool isStarted) {
    qDebug() << "setState: " << isStarted;
    this->isStarted = isStarted;
    QString text = isStarted ? "Stop" : "Run";
    QString toolTip =
        isStarted ? "Socks-Alice (Running)" : "Socks-Alice (Stopped)";
    mSystemTrayIcon->contextMenu()->actions().first()->setText(text);
    ui->btnRun->setText(text);
    ui->lineEditIP->setEnabled(!isStarted);
    ui->spinBoxSocksPort->setEnabled(!isStarted);
    ui->spinBoxHttpPort->setEnabled(!isStarted);
    ui->checkBoxHttp->setEnabled(!isStarted);
    mSystemTrayIcon->setToolTip(toolTip);
}

void MainDialog::setUser(USERTYPE usertype) {
    this->usertype = usertype;
    // 2: admin tab
    // 1: user tab
    // if admin, hide user tab
    // alawys ADMIN for now
    int index /*= ADMIN*/;
    index = usertype == ADMIN ? 1 : 2;
    ui->tabWidget->setTabVisible(index, false);
    mSystemTrayIcon->show();
    show();
}

void MainDialog::reject() {
    qInfo() << "reject called";
    if (false) {
	QDialog::reject();
    } else {
	show_hide();
    }
}

QSettings* getConf() {
    QString configPath = QCoreApplication::applicationDirPath() + "/Socks-Alice.ini";
    QSettings *psettings;
    if (QFile(configPath).exists()) {
        psettings = new QSettings(configPath, QSettings::IniFormat);
    } else {
        psettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "sky-bro", "Socks-Alice");
    }
    return psettings;
}

void MainDialog::readSettings() {
    QSettings *psettings = getConf(), &settings = *psettings;

    resize(settings.value("window/size", QSize(500, 300)).toSize());

    ui->lineEditIP->setText(settings.value("local/ip", "127.1").toString());
    ui->spinBoxSocksPort->setValue(
        settings.value("local/socks_port", 1081).toInt());
    ui->spinBoxHttpPort->setValue(
        settings.value("local/http_port", 1080).toInt());
    bool http_check =
        settings.value("local/http_check", "true").toString() == "true";
    ui->checkBoxHttp->setChecked(http_check);

    int size = settings.beginReadArray("servers");
    bool flag = true;
    for (int r = 0; r < size; ++r) {
        settings.setArrayIndex(r);
        ui->serverList->insertRow(r);
        ui->serverList->setItem(
            r, COL_NAME,
            new QTableWidgetItem(settings.value("name").toString()));
        ui->serverList->setItem(
            r, COL_IP, new QTableWidgetItem(settings.value("ip").toString()));
        ui->serverList->setItem(
            r, COL_PORT,
            new QTableWidgetItem(settings.value("port").toString()));
        ui->serverList->setItem(
            r, COL_METHOD,
            new QTableWidgetItem(settings.value("method").toString()));
        ui->serverList->setItem(
            r, COL_PROXYIP,
            new QTableWidgetItem(settings.value("proxy ip").toString()));
        ui->serverList->setItem(
            r, COL_PROXYPORT,
            new QTableWidgetItem(settings.value("proxy port").toString()));
        ui->serverList->setItem(
            r, COL_KEY, new QTableWidgetItem(settings.value("key").toString()));
        if (flag && settings.value("highlight").toString() == "true") {
            qDebug() << "found highlight row in config";
            flag = false;
            setServer(r);
        }
    }
    settings.endArray();
    delete psettings;
}

void MainDialog::writeSettings() {
    QSettings *psettings = getConf(), &settings = *psettings;
    settings.setValue("window/size", size());

    settings.setValue("local/ip", ui->lineEditIP->text());
    settings.setValue("local/socks_port", ui->spinBoxSocksPort->value());
    settings.setValue("local/http_port", ui->spinBoxHttpPort->value());
    bool http_check = ui->checkBoxHttp->isChecked();
    settings.setValue("local/http_check", http_check ? "true" : "false");

    settings.beginWriteArray("servers");
    bool flag = true;
    for (int i = 0; i < ui->serverList->rowCount(); ++i) {
        settings.setArrayIndex(i);
        if (flag &&
            ui->serverList->item(i, 0)->background().color() == Qt::green) {
            flag = false;
            settings.setValue("highlight", "true");
            qDebug() << "highlight row set in config";
        } else {
            settings.setValue("highlight", "false");
        }
        settings.setValue("name", ui->serverList->item(i, COL_NAME)->text());
        settings.setValue("ip", ui->serverList->item(i, COL_IP)->text());
        settings.setValue("port", ui->serverList->item(i, COL_PORT)->text());
        settings.setValue("method",
                          ui->serverList->item(i, COL_METHOD)->text());
        settings.setValue("proxy ip",
                          ui->serverList->item(i, COL_PROXYIP)->text());
        settings.setValue("proxy port",
                          ui->serverList->item(i, COL_PROXYPORT)->text());
        settings.setValue("key", ui->serverList->item(i, COL_KEY)->text());
    }
    settings.endArray();

    qInfo() << "settings written to" << settings.fileName();
    delete psettings;
}

void MainDialog::checkCurrentIndex(const QModelIndex &index) {
    bool valid = index.isValid();  // on a row
    bool connected = false;
    if (valid) {
        int row = ui->serverList->currentRow();
        connected = ui->serverList->item(row, COL_NAME)->background().color() ==
                    Qt::green;
    }
    ui->actionTestLatency->setEnabled(valid);
    ui->actionEdit->setEnabled(valid && !connected);
    ui->actionExport->setEnabled(valid);
    ui->actionDelete->setEnabled(valid && !connected);
    ui->actionConnect->setEnabled(!serverSet);
    ui->actionDisconnect->setEnabled(valid && connected);
}

void MainDialog::setServer(int r) {
    if (isStarted) return;  // please disconnect first
    m_serverAddr = QHostAddress(ui->serverList->item(r, COL_IP)->text());
    m_serverPort = ui->serverList->item(r, COL_PORT)->text().toUShort();
    m_method = ui->serverList->item(r, COL_METHOD)->text().toStdString();
    m_password = ui->serverList->item(r, COL_KEY)->text().toStdString();
    m_proxyAddr = QHostAddress(ui->serverList->item(r, COL_PROXYIP)->text());
    m_proxyPort = ui->serverList->item(r, COL_PROXYPORT)->text().toUShort();
    bgBrush = ui->serverList->item(r, COL_NAME)->background();
    fgBrush = ui->serverList->item(r, COL_NAME)->foreground();
    ui->serverList->item(r, 0)->setBackground(Qt::green);
    ui->serverList->item(r, 0)->setForeground(Qt::black);
    serverSet = true;
    qDebug() << "serverSet true";
}

bool MainDialog::isValidConfig(const QJsonObject &config) {
    for (int i = 0; i < COL_COUNT; ++i) {
        if (!config.contains(columns[i])) return false;
    }
    return true;
}

QJsonObject MainDialog::getServerConfig(int r) {
    QJsonObject config;
    for (int i = 0; i < COL_COUNT; ++i) {
        if (i == COL_PORT || i == COL_PROXYPORT)
            config.insert(columns[i],
                          ui->serverList->item(r, i)->text().toUShort());
        else
            config.insert(columns[i], ui->serverList->item(r, i)->text());
    }
    return config;
}

void MainDialog::updateServerConfig(int r, const QJsonObject &config) {
    for (int i = 0; i < COL_COUNT; ++i) {
        if (i == COL_PORT || i == COL_PROXYPORT)
            ui->serverList->item(r, i)->setText(
                QString::number(config[columns[i]].toInt()));
        else
            ui->serverList->item(r, i)->setText(config[columns[i]].toString());
    }
}

void MainDialog::trayiconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (m_pTimer) {
        m_reason = QSystemTrayIcon::DoubleClick;
        return;
    }

    m_reason = reason;

    m_pTimer = new QTimer;
    m_pTimer->setSingleShot(true);
    connect(m_pTimer, &QTimer::timeout, this, [=]() {
        if (m_reason == QSystemTrayIcon::DoubleClick)
            show_hide();
        else if (reason == QSystemTrayIcon::Trigger ||
                 reason == QSystemTrayIcon::MiddleClick) {
            mSystemTrayIcon->contextMenu()->popup(QCursor::pos());
        }
        // tidyup
        m_pTimer->deleteLater();
        m_pTimer = nullptr;
    });
    m_pTimer->start(200);
}

void MainDialog::show_hide() {
    if (isVisible())
        hide();
    else {
        show();
        setWindowState(Qt::WindowState::WindowActive);
    }
}

void MainDialog::on_btnRun_clicked() {
    if (!isStarted) {
        // start server
        m_localAddr = QHostAddress(ui->lineEditIP->text());
        m_localPort = ui->spinBoxSocksPort->value();
        if (!serverSet) {
            QMessageBox::warning(this, "server is not set",
                                 "please select a server first");
            return;
        }
        if (tcpServer.listen(m_localAddr, m_localPort, m_serverAddr,
                             m_serverPort, m_method, m_password, m_proxyAddr,
                             m_proxyPort)) {
            if (ui->checkBoxHttp->isChecked()) {
                // share same port with socks5 proxy ? TODO
                // unnecessary
                m_httpport = ui->spinBoxHttpPort->value();
                if (!httpServer.httpListen(m_localAddr, m_httpport,
                                           m_localPort)) {
                    tcpServer.close();
                }
            }
        }
        if (tcpServer.isListening()) {
            mSystemTrayIcon->showMessage(tr("Listening"),
                                         tr("listening at -- %0:%1")
                                             .arg(m_localAddr.toString())
                                             .arg(m_localPort),
                                         QSystemTrayIcon::Information, 3000);
            setState(true);
        } else {
            mSystemTrayIcon->showMessage(tr("Run Failed"),
                                         tr("Please check your ip/port"),
                                         QSystemTrayIcon::Warning, 3000);
        }
    } else {
        // stop server
        tcpServer.close();
        if (httpServer.isListening()) httpServer.close();
        setState(false);
    }
}

void MainDialog::onCustomContextMenuRequested(const QPoint &pos) {
    checkCurrentIndex(ui->serverList->indexAt(pos));
    menuConnection->popup(ui->serverList->viewport()->mapToGlobal(pos));
}

void MainDialog::onEdit() {
    int row = ui->serverList->currentRow();  // start from 0
    ServerEditDialog *editDlg = new ServerEditDialog(ui->serverList, row, this);
    connect(editDlg, &ServerEditDialog::finished, editDlg,
            &ServerEditDialog::deleteLater);
    if (editDlg->exec()) {
        //            configHelper->save(*model);
        writeSettings();
    }
}

void MainDialog::onAdd() {
    int row = ui->serverList->rowCount();
    //    ui->serverList->insertRow(row);
    ServerEditDialog *editDlg = new ServerEditDialog(ui->serverList, row, this);
    connect(editDlg, &ServerEditDialog::finished, editDlg,
            &ServerEditDialog::deleteLater);
    if (editDlg->exec()) {
        //            configHelper->save(*model);
        writeSettings();
    }
}

void MainDialog::onDelete() {
    int row = ui->serverList->currentRow();
    QString serverName = ui->serverList->item(row, COL_NAME)->text();
    QMessageBox::StandardButton button;
    button = QMessageBox::question(
        this, tr("Deleting Server"),
        QString("Are you sure you want to delete '%1'").arg(serverName),
        QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes) {
        ui->serverList->removeRow(row);
    }
}

void MainDialog::onImport() {
    QClipboard *clipboard = QApplication::clipboard();
    QJsonDocument configDoc = QJsonDocument::fromJson(QByteArray::fromBase64(
        clipboard->text().toUtf8(), QByteArray::Base64Encoding));
    qInfo() << "configDoc: " << configDoc.toJson();
    QJsonObject config = configDoc.object();
    if (!isValidConfig(config)) return;

    // 插入新行
    int row = ui->serverList->rowCount();
    ui->serverList->insertRow(row);
    for (int i = 0; i < COL_COUNT; ++i) {
        ui->serverList->setItem(row, i, new QTableWidgetItem());
    }
    updateServerConfig(row, config);
}

void MainDialog::onExport() {
    int row = ui->serverList->currentRow();
    QJsonObject config = getServerConfig(row);
    QJsonDocument configDoc(config);
    QString configB64Str = configDoc.toJson(QJsonDocument::Compact).toBase64();
    qInfo() << "exported config:" << configB64Str;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(configB64Str);
}

void MainDialog::onConnect() {
    int row = ui->serverList->currentRow();
    setServer(row);
    if (!isStarted) on_btnRun_clicked();
}

void MainDialog::onDisconnect() {
    int row = ui->serverList->currentRow();
    if (ui->serverList->item(row, COL_NAME)->background().color() ==
        Qt::green) {
        if (isStarted) on_btnRun_clicked();
        ui->serverList->item(row, COL_NAME)->setBackground(bgBrush);
        ui->serverList->item(row, COL_NAME)->setForeground(fgBrush);
        serverSet = false;
    }
}

void MainDialog::onTestLatency() {
    // TODO
}

void MainDialog::onTestAllLatency() {
    // TODO
}

void MainDialog::onQuit() {
    if (tcpServer.isListening()) tcpServer.close();
    writeSettings();
    QApplication::quit();
}
