#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class ServerEditDialog;
}

class ServerEditDialog : public QDialog {
    Q_OBJECT

  public:
    explicit ServerEditDialog(QTableWidget* table, int row,
                              QWidget* parent = nullptr);
    ~ServerEditDialog();

  private:
    QTableWidget* serverList;
    int r;
    Ui::ServerEditDialog* ui;

  private slots:
    void save();
};

#endif  // EDITDIALOG_H
