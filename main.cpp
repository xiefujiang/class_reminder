#include "reminder_class.h"


#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QResource>
#include <QImage>
#include <QFontDatabase>

//------------------------font name: MaoKenTangYuan (beta)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    declare d;
    QString filename = QCoreApplication::applicationDirPath();
    unit_edit_page edit_page;
    //edit_page.show();
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);
    cfg.beginGroup("Main");
    if(cfg.value("is_agreed").toBool() != true)
    {
        d.show();
    }
    //-------------------------------------------
    Reminder_class w;
    config_page b;
    w.show();

    int font_id = QFontDatabase::addApplicationFont(":/font/res/cute_font.ttf");
    QString font_name = QFontDatabase::applicationFontFamilies(font_id).at(0);
    QFont font(font_name, 12);
    QApplication::setFont(font);
    //qDebug() << font_name;
    static bool isshowed = true;
    static bool isTransparentForInput = true;
    QSystemTrayIcon tray = new QSystemTrayIcon();
    QIcon icon = QIcon(":/icon/res/Reminder_icon.png");
    tray.setIcon(icon);
    tray.setToolTip("课表");
    QMenu *menu = new QMenu();
    QAction *show = new QAction("显示/隐藏");
    QAction *config = new QAction("配置...");
    QAction *toggleInput = new QAction("切换鼠标穿透");
    QAction *exit = new QAction("退出");
    menu->addAction(show);
    menu->addAction(config);
    menu->addAction(toggleInput);
    menu->addAction(exit);
    tray.setContextMenu(menu);

    a.connect(show, &QAction::triggered, [&](){
        if(isshowed)
        {
            w.hide();
        }
        else
        {
            w.show();
        }
        isshowed = !isshowed;
    });
    a.connect(exit, &QAction::triggered, [&](){
        QApplication::quit();
    });
    a.connect(config, &QAction::triggered, [&](){
        b.show();
    });
    a.connect(toggleInput, &QAction::triggered, [&](){
        if(isTransparentForInput)
            {
            w.setWindowFlags(w.windowFlags()& ~Qt::WindowTransparentForInput);
            w.toggleTransparentForInput(false); //显示按钮
            isTransparentForInput = false;
            w.show();
        }
        else
            {
            w.setWindowFlag(Qt::WindowTransparentForInput);
            w.toggleTransparentForInput(true);  //隐藏按钮
            isTransparentForInput = true;
            w.show();
        }
    });
    tray.show();

    a.connect(&b, &config_page::sendChangeSize, &w, &Reminder_class::RecvSizeChanged);
    a.connect(&b, &config_page::sendChangeTransparency, &w, &Reminder_class::RecvTransparecyChanged);


    return a.exec();
}
