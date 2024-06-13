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

//Регистрация или логин тут будет

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
void Authorization::success_slot(bool isSuccess)
{
    successful_auth = isSuccess;
}

//Авторизация / Сделать подключение к бд, но не напрямую, а делать запрос серверу
void Authorization::on_Login_bttn_clicked()
{
    QString name = ui->login_line->text();
    QString pass = QCryptographicHash::hash(ui->password_line->text().toUtf8(), QCryptographicHash::Sha256);

    userData["username"] = name;
    userData["password"] = pass;
    if(isSignUp)
    {
        qDebug() << "регистрация";
        //Signup();
        emit signup_signal(userData);
    }
    else
    {
        qDebug() << "Авторизация";
        //Login();
        emit auth_signal(userData);
    }
    _sleep(10);
    if(!successful_auth)
    {
        userData["username"] = "";
        userData["password"] = "";
        ui->login_line->setText("");
        ui->password_line->setText("");
    }
}

void Authorization::Login()
{
    /*QString name = ui->login_line->text();
    QString pass =  ui->password_line->text();
    userData["username"] = name;
    userData["password"] = pass;*/
    emit auth_signal(userData);
    _sleep(10);

    /*if(!successful_auth)
    {
        userData["username"] = "";
        userData["password"] = "";
        ui->login_line->setText("");
        ui->password_line->setText("");
    }
    else //успешная авторизация
    {
        //emit authsignal(userData);
    }*/
}

void Authorization::Signup()
{
    QString name = ui->login_line->text();
    QString pass =  ui->password_line->text();
    userData["username"] = name;
    userData["password"] = pass;

    emit signup_signal(userData);
    _sleep(10);

    if(!successful_auth)
    {
        userData["username"] = "";
        userData["password"] = "";
        ui->login_line->setText("");
        ui->password_line->setText("");
    }
    else //успешная авторизация
    {
        //emit authsignal(userData);
    }
}


void Authorization::on_sign_up_bttn_clicked()
{
    if(!isSignUp)
    {
        ui->sign_up_bttn->setText("Авторизация");
        ui->Auth_lb->setText("Регистрация");
    }
    else
    {
        ui->sign_up_bttn->setText("Регистрация");
        ui->Auth_lb->setText("Авторизация");
    }
    isSignUp = !isSignUp;
}

