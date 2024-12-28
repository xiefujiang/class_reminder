#ifndef CONFIG_PAGE_H
#define CONFIG_PAGE_H

#include <QWidget>
#include "declare.h"
#include "unit_edit_page.h"

namespace Ui {
class config_page;
}

class config_page : public QWidget
{
    Q_OBJECT

public:
    explicit config_page(QWidget *parent = nullptr);
    ~config_page();

signals:
    void sendChangeSize(int position);//[1, 100]
    void sendChangeTransparency(int position); //[0, 255]
    void sendEditData(QStringList DataList, QModelIndex &index);
    void sendChangeColor(QColor color);

public slots:
    void RecvSaveData(QString Complete_Data, QModelIndex &index);

private slots:
    void on_BTN_CANCEL_clicked();

    void on_BTN_OK_clicked();

    void on_size_slider_sliderMoved(int position);

    void on_transparency_slider_sliderMoved(int position);

    void on_table_doubleClicked(const QModelIndex &index);

    void updateData(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_btn_color_clicked();

    void on_Btn_GetCLS_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::config_page *ui;
};

#endif // CONFIG_PAGE_H
