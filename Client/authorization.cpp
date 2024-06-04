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
    ui->Auth_lb->setText("Lol");
}

void Authorization::on_pushButton_clicked()
{

}


//Сделать подключение к бд, но не напрямую, а делать запрос серверу
void Authorization::on_Login_bttn_clicked()
{
    QString name = ui->login_line->text();
    QString pass =  ui->password_line->text();
    if(true) //успешная авторизация
    {
        userData["username"] = name;
        userData["password"] = pass; //

        emit authsignal(userData);
    }
}

