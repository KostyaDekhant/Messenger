#include "authorization.h"
#include "ui_authorization.h"

Authorization::Authorization(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Authorization)
{
    ui->setupUi(this);
    //connect(this, &Authorization::authsignal, );
}

Authorization::~Authorization()
{
    delete ui;
}

void Authorization::authslot()
{
    ui->label->setText("Lol");
}

void Authorization::on_pushButton_clicked()
{
    if(true) //успешная авторизация
    {
        emit authsignal();
    }
}


