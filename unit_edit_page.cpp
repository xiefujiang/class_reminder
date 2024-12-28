#include "unit_edit_page.h"
#include "ui_unit_edit_page.h"

static QModelIndex current_edting_index;

unit_edit_page::unit_edit_page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::unit_edit_page)
{
    ui->setupUi(this);
}

unit_edit_page::~unit_edit_page()
{
    delete ui;
}

void unit_edit_page::on_pushButton_clicked()
{
    QString Complete_Data;

    if((ui->text_name->toPlainText()==""&&ui->text_teacher->toPlainText()==""&&ui->text_clsrm->toPlainText()=="")||(ui->text_name->toPlainText()=="none"&&ui->text_teacher->toPlainText()=="none"&&ui->text_clsrm->toPlainText()=="none"))
        Complete_Data = "";
    else
    {
        if(ui->text_name->toPlainText()=="")
            Complete_Data = "none";
        else
            Complete_Data = ui->text_name->toPlainText();
        Complete_Data += '\n';
        if(ui->text_teacher->toPlainText()=="")
            Complete_Data += "none";
        else
            Complete_Data += ui->text_teacher->toPlainText();
        Complete_Data += '\n';
        if(ui->text_clsrm->toPlainText()=="")
            Complete_Data += "none";
        else
            Complete_Data += ui->text_clsrm->toPlainText();
    }
    emit SendSaveData(Complete_Data, current_edting_index);
    this->hide();
}

void unit_edit_page::RecvEditData(QStringList DataList, QModelIndex &index)
{
    current_edting_index = index;
    ui->text_name->setText(DataList[0]);
    ui->text_teacher->setText(DataList[1]);
    ui->text_clsrm->setText(DataList[2]);
    this->show();
}


void unit_edit_page::on_btn_clear_clicked()
{
    ui->text_name->clear();
    ui->text_teacher->clear();
    ui->text_clsrm->clear();
}

