#include "declare.h"
#include "ui_declare.h"
#include <QSettings>

declare::declare(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::declare)
{
    ui->setupUi(this);
}

declare::~declare()
{
    delete ui;
}

void declare::on_BTN_AGREE_clicked()
{
    QString filename = QCoreApplication::applicationDirPath();
    filename += "/config/cfg.ini";
    QSettings cfg(filename, QSettings::IniFormat);

    cfg.beginGroup("Main");
    cfg.setValue("is_agreed", true);
    cfg.endGroup();
    this->hide();
}


void declare::on_pushButton_clicked()
{
    QApplication::quit();
}

