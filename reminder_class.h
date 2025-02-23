#ifndef REMINDER_CLASS_H
#define REMINDER_CLASS_H



#include <QWidget>
#include <QMouseEvent>
#include"config_page.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class Reminder_class;
}
QT_END_NAMESPACE

typedef struct wkday_struct
{
    int num;            //1,2,3,4,5,6,7
    QString Chinese;    //周一，周二，周三....
} wkday;
Q_DECLARE_METATYPE(wkday)



struct unit_lesson
{
    bool is_this_exist;
    QString Name;
    QString Teacher;
    QString Classroom;
};

struct unit_day
{
    bool is_this_exist = true;
    unit_lesson les1;
    unit_lesson les2;
    unit_lesson les3;
    unit_lesson les4;
    unit_lesson les5;
};

struct loaded_cources
{
    unit_day monday;
    unit_day tuesday;
    unit_day wednesday;
    unit_day thursday;
    unit_day friday;
};

struct timing
{
    bool is_classing;
    unit_lesson current_lesson_if_classing;
    bool is_remains_class_today = true;
    int spare_mins_to_next_class_if_remains_today;
};





class Reminder_class : public QWidget
{
    Q_OBJECT

public:
    Reminder_class(QWidget *parent = nullptr);
    ~Reminder_class();
    config_page config_class;
    void toggleTransparentForInput(bool state);
    wkday DateToWkday(QDateTime time1);
    timing time2timing(QDateTime time, bool is_F_or_G);

    //unit_day NO_LESSON_TODAY;//用于代替周六周日的unit_day
    //unit_lesson NO_LESSON_NOW;//用于代替没课的节次
    void ReadCLSFromCFG();

private slots:
    void on_pushButton_clicked();

    void on_left_triggered(QAction *arg1);

    void on_left_clicked();

    void on_right_clicked();

    void Time_timerUpdate();

public slots:
    void RecvSizeChanged(int processed);

    void RecvTransparecyChanged(int processed);

    void RecvChangeColor(QString color);

    void ToggleTopBottom();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::Reminder_class *ui;
};
#endif // REMINDER_CLASS_H
