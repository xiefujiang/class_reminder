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
#include <QProcess>
#include <QMessageBox>
#include <QDir>
#include <QThread>

#define AUTO_RUN_PATH "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

QString oldstr;
bool is_skip = false;
QString CurrentColorInQColor1 = QColor::fromRgb(255,255,255).name();
int CurrentTransparency1 = 150;
//bool is_allow_startup = true;
//QStandardItemModel * model = new QStandardItemModel();

config_page::config_page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::config_page)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/res/Reminder_icon_filled.png"));

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
    QStringList nums = QObject::tr("第1,2节，第3,4节，第5,6节，第7,8节，第9,10节").split("，");
    model->setHorizontalHeaderLabels(wkdays);
    model->setVerticalHeaderLabels(nums);

    QString filename = QCoreApplication::applicationDirPath();
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);
    cfg.beginGroup("Main");
    CurrentColorInQColor1 = QColor::fromRgb(cfg.value("bg_color_red").toInt(),cfg.value("bg_color_green").toInt(),cfg.value("bg_color_blue").toInt()).name();
    CurrentTransparency1 = cfg.value("bg_transparency").toInt();
    QString str = "QPushButton {background-color: rgb(";
    str += QString::number(QColor::fromString(CurrentColorInQColor1).red());
    str += ",";
    str += QString::number(QColor::fromString(CurrentColorInQColor1).green());
    str += ",";
    str += QString::number(QColor::fromString(CurrentColorInQColor1).blue());
    str += ");border: 1px solid rgba(0,0,0,100);}";
    ui->btn_color->setStyleSheet(str);
    ui->transparency_slider->setValue(CurrentTransparency1);
    if(cfg.value("is_allow_startup").toString()=="false")
    {
        ui->check_Allow_Startup->setCheckState(Qt::Unchecked);
    }
    else
        ui->check_Allow_Startup->setCheckState(Qt::Checked);
    if(cfg.value("is_agreed").toBool())
    {
        ui->edit_account->setText(cfg.value("account").toString());
        ui->edit_password->setText(cfg.value("password").toString());
    }
    else
    {
        ui->comboBox->setCurrentIndex(1);
        this->resize(785,this->height());

    }
    QStringList wks = QObject::tr("Monday,Tuesday,Wednesday,Thursday,Friday").split(",");
    QStringList clss = QObject::tr("cls1_name,cls1_teacher,cls1_clsrm,cls2_name,cls2_teacher,cls2_clsrm,cls3_name,cls3_teacher,cls3_clsrm,cls4_name,cls4_teacher,cls4_clsrm,cls5_name,cls5_teacher,cls5_clsrm").split(",");
    QStandardItem* item = 0;
    cfg.endGroup();
    //model->clear();
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

    ui->edit_account->setValidator(new QIntValidator(ui->edit_account));
    ui->edit_account->setMaxLength(9);
    ui->edit_password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
}

config_page::~config_page()
{
    delete ui;
}

void config_page::SetStartup(bool is_startup)
{
    QString appName = QApplication::applicationName();
    QString appPath = QApplication::applicationFilePath();
    QSettings *reg = new QSettings(AUTO_RUN_PATH, QSettings::NativeFormat);
    if(is_startup)
    {
        QString value = QString("\"%1\"").arg(appPath.replace("/","\\"));
        reg->setValue(appName, value);
    }
    else
    {
        if(!(reg->contains(appName)))
            reg->remove(appName);
    }
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

    if(ui->check_Allow_Startup->isChecked())
    {
        //qDebug() << QCoreApplication::applicationFilePath();
        SetStartup(true);
        cfg.beginGroup("Main");
        cfg.setValue("is_allow_startup",true);
    }
    else
    {
        //DeleteShortcutInStartup("Reminder");
        SetStartup(false);
        cfg.setValue("is_allow_startup",false);
    }

    cfg.setValue("bg_color_red",QColor::fromString(CurrentColorInQColor1).red());
    cfg.setValue("bg_color_green",QColor::fromString(CurrentColorInQColor1).green());
    cfg.setValue("bg_color_blue",QColor::fromString(CurrentColorInQColor1).blue());
    cfg.setValue("bg_transparency", CurrentTransparency1);

    //restart
    qApp->quit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}


void config_page::on_size_slider_sliderMoved(int position)
{
    int processed = position;
    //emit sendChangeSize(processed);
}


void config_page::on_transparency_slider_sliderMoved(int position)
{
    emit sendChangeTransparency(position);
    CurrentTransparency1 = position;
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
    int sum_width = 94;
    for(int i = 0; i < 5; i++)
    {
        sum_width += ui->table->columnWidth(i);
    }
    ui->table->setFixedWidth(sum_width);
    //qDebug()<<"width"<<ui->table->columnWidth(3);
}

void config_page::on_pushButton_clicked()
{

}

void config_page::on_btn_color_clicked()
{
    CurrentColorInQColor1 = QColorDialog::getColor(QColor::fromString(CurrentColorInQColor1),this,"选择颜色",QColorDialog::ShowAlphaChannel).name();
    //ui->btn_color->setStyleSheet("background-color: rgb(255, 255, 255);border: 1px solid rgba(0,0,0,100);");
    QString str = "QPushButton {background-color: rgb(";
    str += QString::number(QColor::fromString(CurrentColorInQColor1).red());
    str += ",";
    str += QString::number(QColor::fromString(CurrentColorInQColor1).green());
    str += ",";
    str += QString::number(QColor::fromString(CurrentColorInQColor1).blue());
    str += ");border: 1px solid rgba(0,0,0,100);}";
    ui->btn_color->setStyleSheet(str);
    emit sendChangeColor(CurrentColorInQColor1);
    //qDebug() << CurrentColorInQColor1;
}

void config_page::on_Btn_GetCLS_clicked()
{
    {
    QString filename = QCoreApplication::applicationDirPath();
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);
    cfg.beginGroup("Main");
    QDate date = QDate::currentDate();
    cfg.setValue("checking_date",date.toString("yyyy-MM-dd"));
    cfg.setValue("account",ui->edit_account->text());
    cfg.setValue("password", ui->edit_password->text());
    cfg.endGroup();
    }
    // QString path =QCoreApplication::applicationDirPath();
    // QProcess *process1 = new QProcess();
    // process1->start(path+"/over-1.exe");
    // process1->waitForFinished();
    // process1->close();
    // QProcess *process2 = new QProcess();
    // process2->start(path+"/over-2.exe");
    // process2->waitForFinished();
    // process2->close();
    // QProcess *process3 = new QProcess();
    // process3->start(path+"/over-3.exe");
    // process3->waitForFinished();
    // process3->close();


    QDir currentDir = QDir::current();
    QStringList exes = {"over-1.exe", "over-2.exe", "over-3.exe"};
    for (const QString& exe : exes)
    {
        QString exePath = currentDir.absoluteFilePath(exe);
        QProcess process;
        //qDebug() << "Starting to execute: " << exePath;
        process.start(exePath);
        //QThread::sleep(1);
        if (!process.waitForFinished())
        {
            QMessageBox::warning(this, "ERROR", "同步失败，请检查学号密码是否正确以及网络连接是否正常！", QMessageBox::Ok);
            return;
        }
    }

    //QThread::sleep(1);
    //QProcess::startDetached(QCoreApplication::applicationDirPath() + "/execute.cmd");

    QString filename1 = QCoreApplication::applicationDirPath();
    filename1 += "/config/cfg.ini";
    QSettings cfg1(filename1, QSettings::IniFormat);
    QStandardItemModel *model = new QStandardItemModel();
    QStringList wkdays = QObject::tr("周一，周二，周三，周四，周五").split("，");
    QStringList nums = QObject::tr("第1,2节，第3,4节，第5,6节，第7,8节，第9,10节").split("，");
    model->setHorizontalHeaderLabels(wkdays);
    model->setVerticalHeaderLabels(nums);
    QStringList wks = QObject::tr("Monday,Tuesday,Wednesday,Thursday,Friday").split(",");
    QStringList clss = QObject::tr("cls1_name,cls1_teacher,cls1_clsrm,cls2_name,cls2_teacher,cls2_clsrm,cls3_name,cls3_teacher,cls3_clsrm,cls4_name,cls4_teacher,cls4_clsrm,cls5_name,cls5_teacher,cls5_clsrm").split(",");
    QStandardItem* item = 0;
    for(int i = 0; i < 5; i++)
    {
        cfg1.beginGroup(wks[i]);
        int j = 0;
        int k = 1;
        while(true)
        {
            QString completed;
            for(int l = 0; l < 3; l++)
            {
                if(l != 0)
                    completed.append('\n');

                completed.append(cfg1.value(clss[j]).toString());
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
        cfg1.endGroup();
    }
    ui->table->setModel(model);
    ui->table->resizeRowsToContents();
    ui->table->resizeColumnsToContents();
    updateData(QModelIndex());
    ui->table->resizeRowsToContents();
    ui->table->resizeColumnsToContents();

}

void config_page::on_comboBox_currentIndexChanged(int index)
{
    if(index)
    {
        //ui->Btn_GetCLS->hide();
        this->resize(785,this->height());
    }
    else if(!index)
    {
        //ui->Btn_GetCLS->show();
        this->resize(1105,this->height());
    }
}

