#include "reminder_class.h"
#include "ui_reminder_class.h"
#include <windows.h>
#include <QSystemTrayIcon>
#include <QDateTime>
#include <QTimer>
#include <QFont>
#include <QLocale>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QScreen>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QProcess>

//cfg-datetime-format:"yyyy-mm-dd"

int monitor_x = 0;
int monitor_y = 0;
int total_cources_num = 0;
QDateTime CurrentDay;
wkday Currentwkday;
int Current_wk_num;
loaded_cources Loaded_Cources;
QDateTime currentTime;
int CurrentTransparency = 180;
QString CurrentColorInQColor = QColor::fromRgb(255,255,255).name();
bool isTop = false;


Reminder_class::Reminder_class(QWidget *parent)
    : QWidget(parent/*, Qt::WindowStaysOnTopHint*/)
    , ui(new Ui::Reminder_class)
{
    ui->setupUi(this);
    QScreen *screen = QGuiApplication::primaryScreen();
    QSize size = screen->size();
    monitor_x = size.width();
    monitor_y = size.height();

    ui->left->installEventFilter(this);
    ui->right->installEventFilter(this);
    this->installEventFilter(this);

    ui->left->hide();
    ui->right->hide();
    ui->left->setMouseTracking(true);
    this->setMouseTracking(true);

    int font_id = QFontDatabase::addApplicationFont(":/font/res/cute_font.ttf");
    QString font_name = QFontDatabase::applicationFontFamilies(font_id).at(0);
    QFont font(font_name, 12);
    QApplication::setFont(font);
    //int width = (int)(monitor_x*(432.0/2560.0));
    //int height = (int)(monitor_y*(800.0/1440.0));
    int width = 345;
    int height = 365;
    //qDebug() << "x" << processed_x << "y" << processed_y;
    this->setGeometry(monitor_x-width, 0, width, height);
    this->setFixedSize(width, height);
    ui->frame->setFixedSize(width, height);
    //qDebug() << "width" << width << "height" << height;

    ui->frame->repaint();

    ReadCLSFromCFG();
    //qDebug() << Loaded_Cources.thursday.les2.Name;

    CurrentDay = QDateTime::currentDateTime();
    QString temp = CurrentDay.toString("dddd");
    //ui->label_date->setText(CurrentDay.toString("yyyy-MM-dd"));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Time_timerUpdate()));
    timer->start(1000);
    ui->label_time->show();
    //ui->label_date->show();
    //qDebug() << temp;

    if(temp == "Monday")
    {
        Currentwkday.Chinese = "周一";
        Currentwkday.num = 1;
        ui->label_Checking_wkday->setText("周一");
    }
    else if(temp == "Tuesday")
    {
        Currentwkday.Chinese = "周二";
        Currentwkday.num = 2;
        ui->label_Checking_wkday->setText("周二");
    }
    else if(temp == "Wednesday")
    {
        Currentwkday.Chinese = "周三";
        Currentwkday.num = 3;
        ui->label_Checking_wkday->setText("周三");
    }
    else if(temp == "Thursday")
    {
        Currentwkday.Chinese = "周四";
        Currentwkday.num = 4;
        ui->label_Checking_wkday->setText("周四");
        qDebug() <<"周四";
    }
    else if(temp == "Friday")
    {
        Currentwkday.Chinese = "周五";
        Currentwkday.num = 5;
        ui->label_Checking_wkday->setText("周五");
    }
    else if(temp == "Saturday")
    {
        Currentwkday.Chinese = "周六";
        Currentwkday.num = 6;
        ui->label_Checking_wkday->setText("周六");
    }
    else if(temp == "Sunday")
    {
        Currentwkday.Chinese = "周日";
        Currentwkday.num = 7;
        ui->label_Checking_wkday->setText("周日");
    }


    //qDebug() << "1";
    this->setWindowFlag(Qt::WindowStaysOnBottomHint);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::Tool);
    this->setWindowFlag(Qt::WindowTransparentForInput);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowTitle("Reminder");
    this->setWindowIcon(QIcon(":/icon/res/Reminder_icon_filled.png"));

    QString filename = QCoreApplication::applicationDirPath();
    //qDebug() << filename;
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);
    cfg.beginGroup("Main");
    if(!cfg.value("initialized").toBool())   //初始化cfg----------------------------
    {
        config_class.SetStartup(true);
        cfg.endGroup();
        QMessageBox::warning(NULL, "ERROR", "配置文件已损坏，请重新配置课表!", QMessageBox::Ok);
        QStringList list_wkdays = QObject::tr("Monday,Tuesday,Wednesday,Thursday,Friday").split(",");
        QStringList list_cls = QObject::tr("cls1_,cls2_,cls3_,cls4_,cls5_").split(",");
        QStringList list_subcls = QObject::tr("name,teacher,clsrm").split(",");
        for(int i = 0; i < 5; i++)
        {
            cfg.beginGroup(list_wkdays[i]);
            for(int j = 0; j < 5; j++)
            {
                for(int m = 0; m < 3; m++)
                {
                    cfg.setValue(list_cls[j] + list_subcls[m], "none");
                }
            }
            cfg.endGroup();
        }
        cfg.beginGroup("Main");
        cfg.setValue("initialized", true);
        cfg.setValue("account","");
        cfg.setValue("password","");
        cfg.setValue("checking_date","");
        cfg.setValue("is_allow_startup",true);
        cfg.setValue("bg_color_red",255);
        cfg.setValue("bg_color_green",255);
        cfg.setValue("bg_color_blue",255);
        cfg.setValue("bg_transparency",150);
        CurrentColorInQColor=QColor(255,255,255).name();
        CurrentTransparency=150;

        qApp->quit();
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    }
    else cfg.endGroup();
    cfg.beginGroup("Main");
    CurrentColorInQColor = QColor::fromRgb(cfg.value("bg_color_red").toInt(),cfg.value("bg_color_green").toInt(),cfg.value("bg_color_blue").toInt()).name();
    CurrentTransparency = cfg.value("bg_transparency").toInt();
    //qDebug() << "read:" << CurrentColorInQColor << CurrentTransparency;
    QString str="QFrame {border-bottom-left-radius: 20%;background-color: rgba(";
    str += cfg.value("bg_color_red").toString();
    str += ",";
    str += cfg.value("bg_color_green").toString();
    str += ",";
    str += cfg.value("bg_color_blue").toString();
    str += ",";
    str += cfg.value("bg_transparency").toString();
    str += ");}";
    ui->frame->setStyleSheet(str);

    QStringList header={"1,2节","3,4节","5,6节","7,8节","9,10节"};
    ui->tableWidget->setVerticalHeaderLabels(header);
    for(int i = 0; i<5; i++)
    {
        ui->tableWidget->setRowHeight(i,50);
    }
    ui->tableWidget->setColumnWidth(0,300);
    ui->tableWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0); border-radius: 0px;");
    ui->tableWidget->setItem(0,1, new QTableWidgetItem(" "));
    ui->tableWidget->setItem(0,2, new QTableWidgetItem(" "));
    ui->tableWidget->setItem(0,3, new QTableWidgetItem(" "));
    ui->tableWidget->setItem(0,4, new QTableWidgetItem(" "));
    QStringList strs={"cls1","cls2","cls3","cls4","cls5"};

    //Currentwkday.num=5;
    switch (Currentwkday.num) {
    case 1:
        if(Loaded_Cources.monday.les1.is_this_exist)
        {
            strs[0]=Loaded_Cources.monday.les1.Name+" "+Loaded_Cources.monday.les1.Teacher+" "+Loaded_Cources.monday.les1.Classroom;
            //strs.append(Loaded_Cources.monday.les1.Name+" "+Loaded_Cources.monday.les1.Teacher+" "+Loaded_Cources.monday.les1.Classroom);
        }
        else
        {
            strs[0]="none";
            //strs.append("none");
        }
        if(Loaded_Cources.monday.les2.is_this_exist)
        {
            strs[1]=Loaded_Cources.monday.les2.Name+" "+Loaded_Cources.monday.les2.Teacher+" "+Loaded_Cources.monday.les2.Classroom;

        }
        else
        {
            strs[1]="none";
        }
        if(Loaded_Cources.monday.les3.is_this_exist)
        {
            strs[2]=Loaded_Cources.monday.les3.Name+" "+Loaded_Cources.monday.les3.Teacher+" "+Loaded_Cources.monday.les3.Classroom;
            //strs.append(Loaded_Cources.monday.les3.Name+" "+Loaded_Cources.monday.les3.Teacher+" "+Loaded_Cources.monday.les3.Classroom);
        }
        else
        {
            strs[2]="none";
            //strs.append("none");
        }
        if(Loaded_Cources.monday.les4.is_this_exist)
        {
            strs[3]=Loaded_Cources.monday.les4.Name+" "+Loaded_Cources.monday.les4.Teacher+" "+Loaded_Cources.monday.les4.Classroom;
            //strs.append(Loaded_Cources.monday.les4.Name+" "+Loaded_Cources.monday.les4.Teacher+" "+Loaded_Cources.monday.les4.Classroom);
        }
        else
        {
            strs[3]="none";
            //strs.append("none");
        }
        if(Loaded_Cources.monday.les5.is_this_exist)
        {
            strs[4]=Loaded_Cources.monday.les5.Name+" "+Loaded_Cources.monday.les5.Teacher+" "+Loaded_Cources.monday.les5.Classroom;
            //strs.append(Loaded_Cources.monday.les5.Name+" "+Loaded_Cources.monday.les5.Teacher+" "+Loaded_Cources.monday.les5.Classroom);
        }
        else
        {
            strs[4]="none";
            //strs.append("none");
        }
        break;
    case 2:
        if(Loaded_Cources.tuesday.les1.is_this_exist)
        {
            strs[0]=(Loaded_Cources.tuesday.les1.Name+" "+Loaded_Cources.tuesday.les1.Teacher+" "+Loaded_Cources.tuesday.les1.Classroom);
        }
        else
        {
            strs[0]=("none");
        }
        if(Loaded_Cources.tuesday.les2.is_this_exist)
        {
            strs[1]=(Loaded_Cources.tuesday.les2.Name+" "+Loaded_Cources.tuesday.les2.Teacher+" "+Loaded_Cources.tuesday.les2.Classroom);
        }
        else
        {
            strs[1]=("none");
        }
        if(Loaded_Cources.tuesday.les3.is_this_exist)
        {
            strs[2]=(Loaded_Cources.tuesday.les3.Name+" "+Loaded_Cources.tuesday.les3.Teacher+" "+Loaded_Cources.tuesday.les3.Classroom);
        }
        else
        {
            strs[2]=("none");
        }
        if(Loaded_Cources.tuesday.les4.is_this_exist)
        {
            strs[3]=(Loaded_Cources.tuesday.les4.Name+" "+Loaded_Cources.tuesday.les4.Teacher+" "+Loaded_Cources.tuesday.les4.Classroom);
        }
        else
        {
            strs[3]=("none");
        }
        if(Loaded_Cources.tuesday.les5.is_this_exist)
        {
            strs[4]=(Loaded_Cources.tuesday.les5.Name+" "+Loaded_Cources.tuesday.les5.Teacher+" "+Loaded_Cources.tuesday.les5.Classroom);
        }
        else
        {
            strs[4]=("none");
        }
        break;
    case 3:
        if(Loaded_Cources.wednesday.les1.is_this_exist)
        {
            strs[0]=(Loaded_Cources.wednesday.les1.Name+" "+Loaded_Cources.wednesday.les1.Teacher+" "+Loaded_Cources.wednesday.les1.Classroom);
        }
        else
        {
            strs[0]=("none");
        }
        if(Loaded_Cources.wednesday.les2.is_this_exist)
        {
            strs[1]=(Loaded_Cources.wednesday.les2.Name+" "+Loaded_Cources.wednesday.les2.Teacher+" "+Loaded_Cources.wednesday.les2.Classroom);
        }
        else
        {
            strs[1]=("none");
        }
        if(Loaded_Cources.wednesday.les3.is_this_exist)
        {
            strs[2]=(Loaded_Cources.wednesday.les3.Name+" "+Loaded_Cources.wednesday.les3.Teacher+" "+Loaded_Cources.wednesday.les3.Classroom);
        }
        else
        {
            strs[2]=("none");
        }
        if(Loaded_Cources.wednesday.les4.is_this_exist)
        {
            strs[3]=(Loaded_Cources.wednesday.les4.Name+" "+Loaded_Cources.wednesday.les4.Teacher+" "+Loaded_Cources.wednesday.les4.Classroom);
        }
        else
        {
            strs[3]=("none");
        }
        if(Loaded_Cources.wednesday.les5.is_this_exist)
        {
            strs[4]=(Loaded_Cources.wednesday.les5.Name+" "+Loaded_Cources.wednesday.les5.Teacher+" "+Loaded_Cources.wednesday.les5.Classroom);
        }
        else
        {
            strs[4]=("none");
        }
        break;
    case 4:
        qDebug() << "case 4";
        if(Loaded_Cources.thursday.les1.is_this_exist)
        {
            strs[0]=Loaded_Cources.thursday.les1.Name+" "+Loaded_Cources.thursday.les1.Teacher+" "+Loaded_Cources.thursday.les1.Classroom;
        }
        else
        {
            strs[0]="none";
        }
        qDebug() << Loaded_Cources.thursday.les2.Name;
        if(Loaded_Cources.thursday.les2.is_this_exist)
        {
            strs[1]=Loaded_Cources.thursday.les2.Name+" "+Loaded_Cources.thursday.les2.Teacher+" "+Loaded_Cources.thursday.les2.Classroom;
        }
        else
        {
            strs[1]="none";
        }
        if(Loaded_Cources.thursday.les3.is_this_exist)
        {
            strs[2]=(Loaded_Cources.thursday.les3.Name+" "+Loaded_Cources.thursday.les3.Teacher+" "+Loaded_Cources.thursday.les3.Classroom);
        }
        else
        {
            strs[2]=("none");
        }
        if(Loaded_Cources.thursday.les4.is_this_exist)
        {
            strs[3]=(Loaded_Cources.thursday.les4.Name+" "+Loaded_Cources.thursday.les4.Teacher+" "+Loaded_Cources.thursday.les4.Classroom);
        }
        else
        {
            strs[3]=("none");
        }
        if(Loaded_Cources.thursday.les5.is_this_exist)
        {
            strs[4]=(Loaded_Cources.thursday.les5.Name+" "+Loaded_Cources.thursday.les5.Teacher+" "+Loaded_Cources.thursday.les5.Classroom);
        }
        else
        {
            strs[4]=("none");
        }
        break;
    case 5:
        if(Loaded_Cources.friday.les1.is_this_exist)
        {
            strs[0]=(Loaded_Cources.friday.les1.Name+" "+Loaded_Cources.friday.les1.Teacher+" "+Loaded_Cources.friday.les1.Classroom);
        }
        else
        {
            strs[0]=("none");
        }
        if(Loaded_Cources.friday.les2.is_this_exist)
        {
            strs[1]=(Loaded_Cources.friday.les2.Name+" "+Loaded_Cources.friday.les2.Teacher+" "+Loaded_Cources.friday.les2.Classroom);
        }
        else
        {
            strs[1]=("none");
        }
        if(Loaded_Cources.friday.les3.is_this_exist)
        {
            strs[2]=(Loaded_Cources.friday.les3.Name+" "+Loaded_Cources.friday.les3.Teacher+" "+Loaded_Cources.friday.les3.Classroom);
        }
        else
        {
            strs[2]=("none");
        }
        if(Loaded_Cources.friday.les4.is_this_exist)
        {
            strs[3]=(Loaded_Cources.friday.les4.Name+" "+Loaded_Cources.friday.les4.Teacher+" "+Loaded_Cources.friday.les4.Classroom);
        }
        else
        {
            strs[3]=("none");
        }
        if(Loaded_Cources.friday.les5.is_this_exist)
        {
            strs[4]=(Loaded_Cources.friday.les5.Name+" "+Loaded_Cources.friday.les5.Teacher+" "+Loaded_Cources.friday.les5.Classroom);
        }
        else
        {
            strs[4]=("none");
        }
        break;

    case 6:
    case 7:
        strs.append("none");
        strs.append("none");
        strs.append("none");
        strs.append("none");
        strs.append("none");
        break;

    default:
        break;
    }

    if(Currentwkday.num==6||Currentwkday.num==7)
    {
        for(int i = 0; i<5; i++)
        {
            ui->tableWidget->setItem(0,i,new QTableWidgetItem(""));
        }
        return;
    }
    for(int i =0; i<5; i++)
    {
        if(strs[i]=="none")
        {
            ui->tableWidget->setItem(0,i,new QTableWidgetItem(""));
            continue;
        }
        ui->tableWidget->setItem(0,i,new QTableWidgetItem(strs[i]));
    }

    //ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: rgba(0, 0, 0, 0); }");
    //ui->tableWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0);border-radius:10px;");
}

Reminder_class::~Reminder_class()
{
    delete ui;
}

void Reminder_class::toggleTransparentForInput(bool state)
{
    if(state == true)
    {
        ui->left->hide();
        ui->right->hide();
    }
    else if(state == false)
    {
        ui->left->show();
        ui->right->show();
    }
}

//根据当前时间计算此时状态
timing Reminder_class::time2timing(QDateTime time, bool is_F_or_G)
{
    return timing();
    // QDateTime start1(time.date(), QTime(8, 30, 0));
    // QDateTime end1(time.date(), QTime(10, 5, 0));
    // QDateTime start2(time.date(), QTime(10, 25, 0));
    // QDateTime end2(time.date(), QTime(12, 0, 0));
    // QDateTime start3(time.date(), QTime(13, 30, 0));
    // QDateTime end3(time.date(), QTime(15, 5, 0));
    // QDateTime start4(time.date(), QTime(15, 25, 0));
    // QDateTime end4(time.date(), QTime(17, 0, 0));
    // QDateTime start5(time.date(), QTime(18, 0, 0));
    // QDateTime end5(time.date(), QTime(19, 35, 0));
    // QDateTime start2_if_F_or_G(time.date(), QTime(10, 15, 0));
    // QDateTime end2_if_F_or_G(time.date(), QTime(11, 50, 0));
    // timing ret;
    // if(is_F_or_G)
    // {
    //     if(time >= start2_if_F_or_G && time <= end2_if_F_or_G)
    //     {
    //         ret.is_classing = true;
    //         ret.current_lesson_if_classing = Loaded_Cources.today->les2;
    //         if((Loaded_Cources.today->les3.is_this_exist == false) && (Loaded_Cources.today->les4.is_this_exist == false) && (Loaded_Cources.today->les5.is_this_exist == false))
    //         {
    //             ret.is_remains_class_today = false;
    //         }
    //     }
    //     else if(time > end1 && time < start2_if_F_or_G)
    //     {
    //         ret.is_classing = false;
    //     }
    // }
    // else
    // {
    //     if(time >= start1 && time <= end1)
    //     {
    //         ret.is_classing = true;
    //         ret.current_lesson_if_classing = Loaded_Cources.today->les1;
    //         if((Loaded_Cources.today->les2.is_this_exist == false) && (Loaded_Cources.today->les3.is_this_exist == false) && (Loaded_Cources.today->les4.is_this_exist == false) && (Loaded_Cources.today->les5.is_this_exist == false))
    //         {
    //             ret.is_remains_class_today = false;
    //         }
    //     }
    //     else if(time >= start2 && time <= end2)
    //     {
    //         ret.is_classing = true;
    //         ret.current_lesson_if_classing = Loaded_Cources.today->les2;
    //         if((Loaded_Cources.today->les3.is_this_exist == false) && (Loaded_Cources.today->les4.is_this_exist == false) && (Loaded_Cources.today->les5.is_this_exist == false))
    //         {
    //             ret.is_remains_class_today = false;
    //         }
    //     }
    //     else if(time >= start3 && time <= end3)
    //     {
    //         ret.is_classing = true;
    //         ret.current_lesson_if_classing = Loaded_Cources.today->les3;
    //         if((Loaded_Cources.today->les4.is_this_exist == false) && (Loaded_Cources.today->les5.is_this_exist == false))
    //         {
    //             ret.is_remains_class_today = false;
    //         }
    //     }
    //     else if(time >= start4 && time <= end4)
    //     {
    //         ret.is_classing = true;
    //         ret.current_lesson_if_classing = Loaded_Cources.today->les4;
    //         if(Loaded_Cources.today->les5.is_this_exist == false)
    //         {
    //             ret.is_remains_class_today = false;
    //         }
    //     }
    //     else if(time >= start5 && time <= end5)
    //     {
    //         ret.is_classing = true;
    //         ret.current_lesson_if_classing = Loaded_Cources.today->les5;
    //         ret.is_remains_class_today = false;
    //     }
    //     else
    //     {
    //         ret.is_classing = false;
    //     }
    // }
    // return ret;
}


void Reminder_class::ReadCLSFromCFG()
{
    QString filename = QCoreApplication::applicationDirPath();
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);

    QStringList list_wkdays = QObject::tr("Monday,Tuesday,Wednesday,Thursday,Friday").split(",");
    QStringList list_cls = QObject::tr("cls1_,cls2_,cls3_,cls4_,cls5_").split(",");
    QStringList list_subcls = QObject::tr("name,teacher,clsrm").split(",");

    // 遍历从周一到周五
    for (int i = 0; i < list_wkdays.size(); ++i) {
        // 进入每一天的组
        cfg.beginGroup(list_wkdays[i]);

        // 遍历每节课
        for (int j = 0; j < list_cls.size(); ++j) {
            QString lessonName = list_cls[j] + list_subcls[0];
            QString teacherName = list_cls[j] + list_subcls[1];
            QString classroomName = list_cls[j] + list_subcls[2];

            unit_lesson* lesson = nullptr;

            // 根据当前是周几来选择对应的 unit_day
            switch (i) {
            case 0: lesson = &Loaded_Cources.monday.les1 + j; break;
            case 1: lesson = &Loaded_Cources.tuesday.les1 + j; break;
            case 2: lesson = &Loaded_Cources.wednesday.les1 + j; break;
            case 3: lesson = &Loaded_Cources.thursday.les1 + j; break;
            case 4: lesson = &Loaded_Cources.friday.les1 + j; break;
            }

            if (cfg.value(lessonName) == "none" && cfg.value(teacherName) == "none" && cfg.value(classroomName) == "none") {
                lesson->is_this_exist = false;
            } else {
                lesson->is_this_exist = true;
                lesson->Name = cfg.value(lessonName).toString();
                lesson->Teacher = cfg.value(teacherName).toString();
                lesson->Classroom = cfg.value(classroomName).toString();
            }
        }

        // 结束当前组
        cfg.endGroup();
    }
}



void Reminder_class::on_pushButton_clicked()
{
    this->hide();
}

void Reminder_class::on_left_triggered(QAction *arg1)
{
    //QMessageBox::information(NULL, "111", "triggered", QMessageBox::Close);
    //qDebug() << "niubi";
}

void Reminder_class::mouseMoveEvent(QMouseEvent *event)
{
    //QRect rect1 = ui->left->rect();
    //qDebug() << rect1 << event->pos();
    //if(rect1.contains(event->pos()))
    //{
    //    qDebug() << "contains";
    //}
    //qDebug() << "1111111";
}

bool Reminder_class::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->left)
    {
        if(event->type() == QEvent::Enter)
        {
            //qDebug() << "entered";
            ui->left->setStyleSheet("background-color: rgba(255, 0, 0, 180);border-radius: 0px;");
            return true;
        }
        else if(event->type() == QEvent::Leave)
        {
            ui->left->setStyleSheet("background-color: rgba(255, 255, 0, 180);border-radius: 0px;");
            return true;
        }
    }
    else if(watched == ui->right)
    {
        if(event->type() == QEvent::Enter)
        {
            ui->right->setStyleSheet("background-color: rgba(255, 0, 0, 180);border-radius: 0px;");
            return true;
        }
        else if(event->type() == QEvent::Leave)
        {
            ui->right->setStyleSheet("background-color: rgba(255, 255, 0, 180);border-radius: 0px;");
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Reminder_class::on_left_clicked()
{
    //qDebug() << "niubi111";
}

void Reminder_class::on_right_clicked()
{

}

void Reminder_class::Time_timerUpdate()
{
    currentTime = QDateTime::currentDateTime();
    ui->label_time->setText(currentTime.toString("yyyy-MM-dd hh:mm:ss"));
}

void Reminder_class::RecvSizeChanged(int processed)
{
    qDebug() << "Size: " << processed;
}

void Reminder_class::RecvTransparecyChanged(int processed)
{
    //qDebug() << "Transparency: " << processed;
    QString str = "QFrame {border-bottom-left-radius: 20%;background-color: rgba(";
    str += QString::number(QColor::fromString(CurrentColorInQColor).red());
    str += ",";
    str += QString::number(QColor::fromString(CurrentColorInQColor).green());
    str += ",";
    str += QString::number(QColor::fromString(CurrentColorInQColor).blue());
    str += ",";
    str += QString::number(processed);
    str += ");}";
    ui->frame->setStyleSheet(str);
    CurrentTransparency = processed;
}

void Reminder_class::RecvChangeColor(QString color)
{
    QString str = "QFrame {border-bottom-left-radius: 20%;background-color: rgba(";
    str += QString::number(QColor::fromString(color).red());
    str += ",";
    str += QString::number(QColor::fromString(color).green());
    str += ",";
    str += QString::number(QColor::fromString(color).blue());
    str += ",";
    str += QString::number(CurrentTransparency);
    str += ");}";
    //qDebug() << "Color: " << str;
    ui->frame->setStyleSheet(str);
    CurrentColorInQColor = color;
}

void Reminder_class::ToggleTopBottom()
{
    if(isTop)
    {
        this->setWindowFlags(this->windowFlags()& ~Qt::WindowStaysOnTopHint);
        setWindowFlag(Qt::WindowStaysOnBottomHint);
        isTop = false;
        this->show();
    }
    else
    {
        this->setWindowFlags(this->windowFlags()& ~Qt::WindowStaysOnBottomHint);
        //setWindowFlag(Qt::WindowStaysOnTopHint);
        isTop = true;
        SetWindowPos((HWND)this->winId(),HWND_TOPMOST,this->pos().x(),this->pos().y(),this->size().width(),this->size().height(),SWP_SHOWWINDOW);
    }
}

