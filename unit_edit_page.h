#ifndef UNIT_EDIT_PAGE_H
#define UNIT_EDIT_PAGE_H

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class unit_edit_page;
}

class unit_edit_page : public QWidget
{
    Q_OBJECT

public:
    explicit unit_edit_page(QWidget *parent = nullptr);
    ~unit_edit_page();

signals:
    void SendSaveData(QString Complete_Data, QModelIndex &index);

private slots:
    void on_pushButton_clicked();

    void on_btn_clear_clicked();

public slots:
    void RecvEditData(QStringList DataList, QModelIndex &index);

private:
    Ui::unit_edit_page *ui;
};

#endif // UNIT_EDIT_PAGE_H
