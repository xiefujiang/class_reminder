#include "config_page.h"
#include "ui_config_page.h"
#include <QFontDatabase>
#include <QStandardItemModel>
#include <QTableWidgetItem>
#include <QModelIndex>
#include <QSettings>
#include <QColor>
#include <QColorDialog>
#include <QDate>

QString oldstr;
bool is_skip = false;
QStandardItemModel * model = new QStandardItemModel();

config_page::config_page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::config_page)
{
    ui->setupUi(this);

    int font_id = QFontDatabase::addApplicationFont(":/font/res/cute_font.ttf");
    QString font_name = QFontDatabase::applicationFontFamilies(font_id).at(0);
    QFont font(font_name, 12);
    QApplication::setFont(font);

    QStringList items;
    items << "自动导入" << "手动输入";
    ui->comboBox->addItems(items);

    ui->table->setShowGrid(true);
    ui->table->setSortingEnabled(false);
    QStandardItemModel *model = new QStandardItemModel();
    QStringList wkdays = QObject::tr("周一，周二，周三，周四，周五").split("，");
    QStringList nums = QObject::tr("第一大节，第二大节，第三大节，第四大节，第五大节").split("，");
    model->setHorizontalHeaderLabels(wkdays);
    model->setVerticalHeaderLabels(nums);
    QString filename = QCoreApplication::applicationDirPath();
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);
    QStringList wks = QObject::tr("Monday,Tuesday,Wednesday,Thursday,Friday").split(",");
    QStringList clss = QObject::tr("cls1_name,cls1_teacher,cls1_clsrm,cls2_name,cls2_teacher,cls2_clsrm,cls3_name,cls3_teacher,cls3_clsrm,cls4_name,cls4_teacher,cls4_clsrm,cls5_name,cls5_teacher,cls5_clsrm").split(",");
    QStandardItem* item = 0;
    for(int i = 0; i < 5; i++)
    {
        cfg.beginGroup(wks[i]);
        int j = 0;
        int k = 1;
        while(true)
        {
            QString completed;
            for(int l = 0; l < 3; l++)
            {
                if(l != 0)
                    completed.append('\n');

                completed.append(cfg.value(clss[j]).toString());
                j++;
            }
            if(completed=="none\nnone\nnone")
                completed = "";
            item = new QStandardItem(completed);
            model->setItem(k-1, i, item);
            k++;
            if(j == 15)
                break;
        }
        cfg.endGroup();
    }
    ui->table->setModel(model);
    connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)), this, SLOT(updateData(const QModelIndex &)));
    ui->table->wordWrap();
    ui->table->resizeRowsToContents();
    ui->table->resizeColumnsToContents();
    ui->table->show();

    this->setWindowFlag(Qt::FramelessWindowHint);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    unit_edit_page *page = new unit_edit_page();
    connect(this, SIGNAL(sendEditData(QStringList,QModelIndex&)), page, SLOT(RecvEditData(QStringList,QModelIndex&)));
    connect(page, SIGNAL(SendSaveData(QString,QModelIndex&)), this, SLOT(RecvSaveData(QString,QModelIndex&)));
}

config_page::~config_page()
{
    delete ui;
}

void config_page::RecvSaveData(QString Complete_Data, QModelIndex &index)
{
    ui->table->model()->setData(index,Complete_Data);
    ui->table->resizeColumnsToContents();
    ui->table->resizeRowsToContents();
}


void config_page::on_BTN_CANCEL_clicked()
{
    this->hide();
}


void config_page::on_BTN_OK_clicked()   //--------------保存表格至cfg
{
    QString filename = QCoreApplication::applicationDirPath();
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);
    QStringList list_wkdays = QObject::tr("Monday,Tuesday,Wednesday,Thursday,Friday").split(",");
    QStringList list_cls = QObject::tr("cls1_,cls2_,cls3_,cls4_,cls5_").split(",");
    QStringList list_subcls = QObject::tr("name,teacher,clsrm").split(",");
    QModelIndex index;
    QString str;
    QStringList unit;
    for(int i = 0;i < 5;i++)    //列
    {
        cfg.beginGroup(list_wkdays [i]);
        for(int j = 0;j < 5;j++)    //行
        {
            index = ui->table->model()->index(j, i);
            str = ui->table->model()->data(index).toString();
            if(str=="")
            {
                unit={"none","none","none"};
            }
            else
                unit = str.split('\n');
            for(int m = 0; m < 3; m++)
            {
                if(unit[m]=="")
                {
                    cfg.setValue(list_cls[i]+list_subcls[m],"none");
                    continue;
                }
                cfg.setValue(list_cls[j]+list_subcls[m], unit[m]);
            }
        }
        cfg.endGroup();
    }
    this->hide();
}


void config_page::on_size_slider_sliderMoved(int position)
{
    int processed = position;
    emit sendChangeSize(processed);
}


void config_page::on_transparency_slider_sliderMoved(int position)
{
    int processed = position;

    emit sendChangeTransparency(processed);
}

void config_page::on_table_doubleClicked(const QModelIndex &index)
{
    //oldstr = ui->table->model()->data(index).toString();
    //qDebug() << "doubleclicked! index:" << ui->table->model()->data(index).toString();
    //QStringList str = ui->table->model()->data(index).toString().split('\n');
    //QString newstr = str.join("/");
    //is_skip = true;
    //ui->table->model()->setData(index, newstr, Qt::EditRole);
    //ui->table->update();
    //qDebug() << "changed";
    QModelIndex index1 = index;
    QStringList datalist;
    //Debug() << "index:" <<ui->table->model()->data(index).toString();
    if(ui->table->model()->data(index).toString()=="")
        datalist = {"","",""};
    else
        datalist = ui->table->model()->data(index).toString().split('\n');
    emit sendEditData(datalist, index1);
    //qDebug() << index.row() << index.column();
}

void config_page::updateData(const QModelIndex &index)
{

}

void config_page::on_pushButton_clicked()
{

}


void config_page::on_btn_color_clicked()
{

}


void config_page::on_Btn_GetCLS_clicked()
{
    QString filename = QCoreApplication::applicationDirPath();
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);
    cfg.beginGroup("Main");
    QDate date = QDate::currentDate();
    cfg.setValue("checking_date",date.toString("yyyy-MM-dd"));
    cfg.endGroup();
}


void config_page::on_comboBox_currentIndexChanged(int index)
{
    if(index)
    {
        ui->Btn_GetCLS->hide();
    }
    else if(!index)
    {
        ui->Btn_GetCLS->show();
    }
}

