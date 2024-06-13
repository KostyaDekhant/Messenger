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

    ui->connectBttn->click();
}



MainWindow::~MainWindow()
{
    delete ui;
}

//Установка соединения с сервером
void MainWindow::on_connectBttn_clicked()
{
    socket->connectToHost("192.168.0.11", 27015);

    if(!socket->waitForConnected(5000)) { // ждем соединения в течение 5 секунд
        QMessageBox::information(nullptr, "Внимание!", "Ошибка при подключении к серверу! \nКод ошибки: " + socket->errorString());
    } else {
        qDebug() << "Успешное подключение к серверу";
        ui->connectBttn->setEnabled(false);
    }
}

//Отправка сообщений на сервер
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

//Тип сообщения
void MainWindow::TypeMessageDetect(QString str)
{
    QJsonDocument jDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jObj = jDoc.object();
    qDebug() << "JSON: " << jObj;
    QString type = jObj["type"].toString();

    if(type == "message")
    {
        QJsonObject temp = jObj["value"].toObject();
        QString time = temp["time"].toString(),
                sender =  temp["sender"].toString(),
                text = temp["text"].toString();

        if(sender.toInt() != (user["id"].toInt())) //левая сторона, чужие смс
        {
            ui->textBrowser->insertHtml("<br><p style=\"text-align:left;\"><font size=\"5\"><b>" + text + "</b></font></p>"
                                        "<p style=\"text-align:left;\"><font size=\"1\">" + time + "</font></p>");
        }

        else //правая сторона, свои смс
        {
            //full_msg = str + " ";

            ui->textBrowser->insertHtml(
                "<br><p style=\"text-align:right;\"><font size=\"5\"><b>" + text + "</b></font></p>"
                "<p style=\"text-align:right;\"><font size=\"1\">" + time + "</font></p>");
        }
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
    }
    else if (type == "who's_online")
    {
        OnlineIcons(jObj["value"].toArray());
    }
    else if (type == "auth")
    {
        bool status = jObj["value"].toBool();

        user.insert("id", jObj["id"].toInt());
        qDebug() <<  user;
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
    }
    else if (type == "signup")
    {
        bool status = jObj["value"].toBool();

        user.insert("id", jObj["id"].toInt());

        qDebug() <<  user;

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
    else if(type == "load_dialog")
    {
        currentChat = jObj["id_chat"].toInt();
        AcceptMessResponse(jObj["value"]);
    }
    else
    {
        qDebug() << "Неизвестный тип данных: " << type;
    }
}

//Вывод сообщений на экран
void MainWindow::AcceptMessResponse(QJsonValue value)
{
    ui->textBrowser->clear();
    QJsonArray jArray = value.toArray();
    if (!jArray.size()) {
        ui->textBrowser->append("Начало диалога!");
        return;
    }
    foreach (const QJsonValue& it, jArray) {
        //QJsonObject temp = jObj["value"].toObject();

        QString time = it["time"].toString(),
            sender =  it["sender"].toString(),
            text = it["text"].toString();

        if(sender != (user["username"].toString())) //левая сторона, чужие смс
        {
            ui->textBrowser->insertHtml("<br><p style=\"text-align:left;\"><font size=\"5\"><b>" + text + "</b></font></p>"
                                        "<p style=\"text-align:left;\"><font size=\"1\">" + time + "</font></p>");
        }

        else //правая сторона, свои смс
        {
            ui->textBrowser->insertHtml(
                "<br><p style=\"text-align:right;\"><font size=\"5\"><b>" + text + "</b></font></p>"
                "<p style=\"text-align:right;\"><font size=\"1\">" + time + "</font></p>");
        }
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
    }

}

//Приёмка сообщений с сервера
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

//Нажатие на чат
void MainWindow::onButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    //SendToServer(button->text());

    QJsonObject jObj;
    jObj.insert("type", "open_chat");

    QJsonObject users;

    users.insert("sender", user["username"].toString());
    users.insert("recipient", button->text());

    jObj.insert("value", users);

    QJsonDocument jDoc = QJsonDocument(jObj);
    QString jStr = QString(jDoc.toJson());

    SendToServer(jStr);

    ui->textBrowser->clear();
}

//Вывод пользователей, которые онлайн
void MainWindow::OnlineIcons(QJsonArray stringList)
{
    bool add_flag = false;


    for(int i = 0; i < stringList.size(); i++) {
        QJsonValue jsonValue = stringList.at(i);

        qDebug() << i << " " << jsonValue.toString() ;
        if(jsonValue.toString() == user["username"].toString())
            continue;

        bool flag = false;
        for(QPushButton* bttn : buttonList)
        {
            if(bttn->text() == jsonValue.toString())
            {
                flag = true;
                break;
            }
        }
        if(flag)
            continue;

        QPushButton* button = new QPushButton(jsonValue.toString());
        button->setFixedSize(ui->scrollArea->viewport()->size().width()-18, 40);
        user_layout->addWidget(button);
        buttonList.append(button);

        connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));

        add_flag = true;
    }

    ui->scrollArea->setWidgetResizable(true);

    if(add_flag)
    {
        qDebug() << "добавлена кнопка";
    }
    else
        qDebug() << "кнопка не добавлена ";
}

//Данные о пользователе с окна авторизации
void MainWindow::auth_slot(QJsonObject userData)
{
    userData["descrip"] = descriptor;
    user = userData;

    QJsonObject jObj;
    jObj.insert("type", "auth");
    jObj.insert("value", userData);

    QJsonDocument jDoc = QJsonDocument(jObj);
    QString jStr = QString(jDoc.toJson());

    SendToServer(jStr);
}

//Данные о пользователе с окна регистрации
void MainWindow::signup_slot(QJsonObject userData)
{
    userData["descrip"] = descriptor;
    user = userData;

    QJsonObject jObj;
    jObj.insert("type", "signup");
    jObj.insert("value", userData);

    QJsonDocument jDoc = QJsonDocument(jObj);
    QString jStr = QString(jDoc.toJson());

    SendToServer(jStr);
}

//отправка сообщений на кнопку
void MainWindow::on_send_msg_bttn_clicked()
{
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

//при нажатии на Enter отправка сообщений
void MainWindow::on_lineEdit_returnPressed()
{
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

//Сигнал от окна авторизации для его закрытия и открытия основного окна
void MainWindow::on_Auth_bttn_clicked()
{
    this->hide();
    auth->show();
    //emit authsignal();
}

//Создание чата
void MainWindow::on_addChat_clicked()
{
    QString chat_name = ui->username_lb->text();
}

