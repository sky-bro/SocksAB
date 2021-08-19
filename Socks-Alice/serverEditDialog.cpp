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
        for (int i = 0; i < COL_COUNT; ++i) {
            serverList->setItem(r, i, new QTableWidgetItem());
        }
        qInfo() << "insert a new row:" << serverList->rowCount();
    }
    serverList->item(r, COL_NAME)->setText(ui->lineEditName->text());
    serverList->item(r, COL_IP)->setText(ui->lineEditServerAddr->text());
    serverList->item(r, COL_PORT)->setText(ui->spinBoxServerPort->text());
    serverList->item(r, COL_METHOD)->setText(ui->comboBoxMethod->currentText());
    serverList->item(r, COL_PROXYIP)->setText(ui->lineEditProxyAddr->text());
    serverList->item(r, COL_PROXYPORT)->setText(ui->spinBoxProxyPort->text());
    serverList->item(r, COL_KEY)->setText(ui->lineEditKey->text());
    qInfo() << "edit server successfully!";
    this->accept();
}
