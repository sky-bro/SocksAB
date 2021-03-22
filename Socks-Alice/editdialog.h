#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(QTableWidget* table, int row, QWidget *parent = nullptr);
    ~EditDialog();

private:
    QTableWidget* serverList;
    int r;
    Ui::EditDialog *ui;

private slots:
    void save();
};

#endif // EDITDIALOG_H
