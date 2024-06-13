#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auth = new Authorization;
    connect(this, &MainWindow::authsignal, auth, &Authorization::success_slot);
    connect(auth, &Authorization::auth_signal, this, &MainWindow::auth_slot);
    connect(auth, &Authorization::signup_signal, this, &MainWindow::signup_slot);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    //connect(socket, &QTcpSocket::disconnected,  this, &MainWindow::SendSocketToServer)
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    user_widget = new QWidget;
    user_layout = new QVBoxLayout;
    user_widget->setLayout(user_layout);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(user_widget);

    user_layout->setAlignment(Qt::AlignTop); // устанавливаем выравнивание для layout

    nextBlockSize = 0;

    ui->pushButton->click();
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //socket->connectToHost("127.0.0.1", 2323);
    socket->connectToHost("192.168.0.11", 27015);

    if(!socket->waitForConnected(5000)) { // ждем соединения в течение 5 секунд
        QMessageBox::information(nullptr, "Внимание!", "Ошибка при подключении к серверу! \nКод ошибки: " + socket->errorString());
    } else {
        qDebug() << "Успешное подключение к серверу";
        ui->pushButton->setEnabled(false);
    }
}

template<typename T>
void MainWindow::SendToServer(T arg)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    out << quint16(0) << arg;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
    ui->lineEdit->clear();
}

void MainWindow::TypeMessageDetect(QString str)
{
    QJsonDocument jDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jObj = jDoc.object();
    qDebug() << "JSON: " << jObj;
    QString type = jObj["type"].toString();

    if(type == "message")
    {
        //in >> time >> str >> sender_name;
        //QString full_msg;

        //qDebug() << user["username"].toString();
        qDebug() << jObj["sender"].toString() << ": " << jObj["value"].toString();
        return;

        /*
        if(sender_name != user["username"].toString()) //левая сторона, чужие смс
        {
            full_msg = time.toString()+ " " + sender_name + ": " + str;
            ui->textBrowser->insertHtml("<br><p style=\"text-align:left;\"><font size=\"5\"><b>" + str + "</b></font></p>"
                                                                                                         "<p style=\"text-align:left;\"><font size=\"1\">" + time.toString() + "</font></p>");

        }
        else //правая сторона, свои смс
        {
            full_msg = str + " ";

            ui->textBrowser->insertHtml(
                "<br><p style=\"text-align:right;\"><font size=\"5\"><b>" + str + "</b></font></p>"
                                                                                  "<p style=\"text-align:right;\"><font size=\"1\">" + time.toString() + "</font></p>");
        }
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
        */


    }
    else if (type == "descrip")
    {
        //in >> descriptor;
    }
    else if (type == "who's_online")
    {
        /*
        in >> stringList;
        OnlineIcons(stringList);*/
    }
    else if (type == "auth")
    {
        bool status = jObj["value"].toBool();
        qDebug() << status;
        emit authsignal(status);
        if(status)
        {
            auth->close();
            ui->Auth_bttn->setEnabled(false);
            this->setWindowTitle(user["username"].toString());
            this->show();
        }
        else
        {
            QJsonObject temp;
            user = temp;
        }
        //in >> descriptor;
    }
    else if (type == "signup")
    {
        bool status = jObj["value"].toBool();
        qDebug() << status;
        emit signupsignal(status);
        if(status)
        {
            auth->close();
            ui->Auth_bttn->setEnabled(false);
            this->setWindowTitle(user["username"].toString());
            this->show();
        }
        else
        {
            QJsonObject temp;
            user = temp;
        }
    }
    else
    {
        qDebug() << "Неизвестный тип данных: " << type;
    }
}


//убрать
void MainWindow::SendUserDataToServer(QJsonObject userData)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    QString msg_type = "userinfo";
    out << quint16(0) << msg_type << userData;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}


void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_7);
    if(in.status() == QDataStream::Ok)
    {
        while(true)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }
            QString str;
            in >> str;
            TypeMessageDetect(str);
            nextBlockSize = 0;

        }
    }
    else
    {
        ui->textBrowser->append("read error: ");
    }
}

void MainWindow::onButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    //SendToServer(button->text());
    ui->textBrowser->clear();

}

void MainWindow::OnlineIcons(QStringList stringList)
{

    //buttonList = new QList<QPushButton*>();


    //scrollArea.show();
    int sizelist = stringList.size();

    if(sizelist == buttonList.size())
        return;

    bool add_flag = false;
    for(int i = 0; i < stringList.size(); i++) //stringList.size()
    {
        if(stringList[i] == user["username"].toString())
            continue;

        bool flag = false;
        for(QPushButton* bttn : buttonList)
        {
            if(bttn->text() == stringList[i])
            {
                flag = true;
                break;
            }
        }
        if(flag)
            continue;

        QPushButton* button = new QPushButton(stringList[i]);
        button->setFixedSize(ui->scrollArea->viewport()->size().width()-18, 40);
        user_layout->addWidget(button);
        buttonList.append(button);

        connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));

        add_flag = true;
    }

    ui->scrollArea->setWidgetResizable(true);
    //ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    if(add_flag)
    {
        qDebug() << "добавлена кнопка";
    }
    else
        qDebug() << "кнопка не добавлена ";
}


void MainWindow::auth_slot(QJsonObject userData)
{
    userData["descrip"] = descriptor;
    user = userData;

    QJsonObject jObj;
    jObj.insert("type", "auth");
    jObj.insert("value", userData);
    //SendUserDataToServer(userData);

    QJsonDocument jDoc = QJsonDocument(jObj);
    QString jStr = QString(jDoc.toJson());

    SendToServer(jStr);
}

void MainWindow::signup_slot(QJsonObject userData)
{
    userData["descrip"] = descriptor;
    user = userData;

    QJsonObject jObj;
    jObj.insert("type", "signup");
    jObj.insert("value", userData);
    //SendUserDataToServer(userData);

    QJsonDocument jDoc = QJsonDocument(jObj);
    QString jStr = QString(jDoc.toJson());

    SendToServer(jStr);
}


void MainWindow::on_pushButton_2_clicked()
{
    if (currentChat == -1) return;




    //SendToServer(ui->lineEdit->text());
}


void MainWindow::on_lineEdit_returnPressed()
{
    //SendToServer(ui->lineEdit->text());

    if (currentChat == -1) return;

    QJsonObject jResObj, mess;
    jResObj.insert("type", "message");
    mess.insert("sender", this->user["username"]);
    mess.insert("pk_chat", this->currentChat);
    mess.insert("text", ui->lineEdit->text());
    jResObj.insert("value", mess);
    QJsonDocument jReqDoc = QJsonDocument{jResObj};
    QString jReqStr = QString(jReqDoc.toJson());
    SendToServer(jReqStr);
}

void MainWindow::on_Auth_bttn_clicked()
{
    this->hide();
    auth->show();
    //emit authsignal();
}


void MainWindow::on_addChat_clicked()
{
    QString chat_name = ui->username_lb->text();
}

