#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
   ui->setupUi(this);
  connect(ui->lineEdit_2, SIGNAL(returnPressed()),ui->pushButton,SIGNAL(clicked()));connect(ui->pushButton,SIGNAL(clicked()),SLOT(close()));
}

Dialog::~Dialog()
{
    delete ui;
}
QString Dialog::directory()
{return (ui->lineEdit->text());
}
QString Dialog::folder_name()
{
    return (ui->lineEdit_2->text());
}
