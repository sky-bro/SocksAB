#include "editdialog.h"
#include "ui_editdialog.h"
#include <QDebug>
#include "dialog.h"

//EditDialog::EditDialog(QWidget *parent) :
//    QDialog(parent),
//    ui(new Ui::EditDialog)
//{
//    ui->setupUi(this);
//}

EditDialog::EditDialog(QTableWidget *table, int row, QWidget *parent):
    QDialog(parent),
    serverList(table),
    r(row),
    ui(new Ui::EditDialog)
{
    ui->setupUi(this);
    if (table->rowCount() > row) { // edit row
        ui->lineEditName->setText(table->item(row, 0)->text());
        ui->lineEditServerAddr->setText(table->item(row, 1)->text());
        ui->spinBoxServerPort->setValue(table->item(row, 2)->text().toUShort());
        ui->comboBoxMethod->setCurrentText(table->item(row, 3)->text());
        ui->lineEditProxyAddr->setText(table->item(row, 4)->text());
        ui->spinBoxProxyPort->setValue(table->item(row, 5)->text().toUShort());
        ui->lineEditKey->setText(table->item(row, 6)->text());
    }
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &EditDialog::save);
}

EditDialog::~EditDialog()
{
    delete ui;
}

void EditDialog::save()
{
    if (serverList->rowCount() <= r) {
        r = serverList->rowCount();
        serverList->insertRow(r);
        qDebug() << "inserted row: " << serverList->rowCount() << r;
    }
    serverList->setItem(r, COL_NAME, new QTableWidgetItem(ui->lineEditName->text()));
    serverList->setItem(r, COL_IP, new QTableWidgetItem(ui->lineEditServerAddr->text()));
    serverList->setItem(r, COL_PORT, new QTableWidgetItem(ui->spinBoxServerPort->text()));
    serverList->setItem(r, COL_METHOD, new QTableWidgetItem(ui->comboBoxMethod->currentText()));
    serverList->setItem(r, COL_PROXYIP, new QTableWidgetItem(ui->lineEditProxyAddr->text()));
    serverList->setItem(r, COL_PROXYPORT, new QTableWidgetItem(ui->spinBoxProxyPort->text()));
    serverList->setItem(r, COL_KEY, new QTableWidgetItem(ui->lineEditKey->text()));
    qDebug() << "saved!";
    this->accept();
}
