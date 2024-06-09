#include "authorization.h"
#include "ui_authorization.h"

Authorization::Authorization(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Authorization)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &Authorization::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    nextBlockSize = 0;
}

Authorization::~Authorization()
{
    delete ui;
}

//Данные об авторизации
void Authorization::slotReadyRead()
{

}

//Сигнал от мейн окна
void Authorization::authslot()
{
    //ui->Auth_lb->setText("Lol");

}

//получение данных с сервера
void Authorization::dbslot()
{

    successful_auth = true;

    successful_auth = false;
}

//Авторизация / Сделать подключение к бд, но не напрямую, а делать запрос серверу
void Authorization::on_Login_bttn_clicked()
{
    QString name = ui->login_line->text();
    QString pass =  ui->password_line->text();
    userData["username"] = name;
    userData["password"] = pass;
    emit dbsignal(userData);
    if(successful_auth) //успешная авторизация
    {
        //сигнал мейн окну с данными user'a
        emit authsignal(userData);
    }
    else
    {
        userData["username"] = "";
        userData["password"] = "";
        ui->login_line->setText("");
        ui->password_line->setText("");
    }

}

