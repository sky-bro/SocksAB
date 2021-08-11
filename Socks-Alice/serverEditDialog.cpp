#include "serverEditDialog.h"

#include <QDebug>

#include "mainDialog.h"
#include "ui_serverEditDialog.h"

/**
 * @brief ServerEditDialog::ServerEditDialog
 * @param table
 * @param row
 * @param parent
 */
ServerEditDialog::ServerEditDialog(QTableWidget *table, int row,
                                   QWidget *parent)
    : QDialog(parent), serverList(table), r(row), ui(new Ui::ServerEditDialog) {
    ui->setupUi(this);
    if (table->rowCount() > row) {  // edit row
        ui->lineEditName->setText(table->item(row, 0)->text());
        ui->lineEditServerAddr->setText(table->item(row, 1)->text());
        ui->spinBoxServerPort->setValue(table->item(row, 2)->text().toUShort());
        ui->comboBoxMethod->setCurrentText(table->item(row, 3)->text());
        ui->lineEditProxyAddr->setText(table->item(row, 4)->text());
        ui->spinBoxProxyPort->setValue(table->item(row, 5)->text().toUShort());
        ui->lineEditKey->setText(table->item(row, 6)->text());
    }
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
            &ServerEditDialog::save);
}

ServerEditDialog::~ServerEditDialog() {
    delete ui;
    qInfo("Server Edit Dialog destructed!");
}

void ServerEditDialog::save() {
    // 编辑的服务器下标超过服务器列表最大下标，新增服务器
    if (r >= serverList->rowCount()) {
        r = serverList->rowCount();
        serverList->insertRow(r);
        qDebug() << "inserted th row: " << serverList->rowCount() << r;
    }
    serverList->setItem(r, COL_NAME,
                        new QTableWidgetItem(ui->lineEditName->text()));
    serverList->setItem(r, COL_IP,
                        new QTableWidgetItem(ui->lineEditServerAddr->text()));
    serverList->setItem(r, COL_PORT,
                        new QTableWidgetItem(ui->spinBoxServerPort->text()));
    serverList->setItem(
        r, COL_METHOD, new QTableWidgetItem(ui->comboBoxMethod->currentText()));
    serverList->setItem(r, COL_PROXYIP,
                        new QTableWidgetItem(ui->lineEditProxyAddr->text()));
    serverList->setItem(r, COL_PROXYPORT,
                        new QTableWidgetItem(ui->spinBoxProxyPort->text()));
    serverList->setItem(r, COL_KEY,
                        new QTableWidgetItem(ui->lineEditKey->text()));
    qDebug() << "saved!";
    this->accept();
}
