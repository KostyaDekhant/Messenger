#include <server.h>

Server::Server()
{
    if(this->listen(QHostAddress::Any, 2323))
    {
        qDebug() << "start";
    }
    else
    {
        qDebug() << "Error: ";
    }
    nextBlockSize = 0;
    db = QSqlDatabase::addDatabase("QSQLITE", "SQLITE");
    db.setDatabaseName("./../users.db"); //C:/qt projects/
    if(db.open())
    {
        qDebug() << "База данных подключена успешно.";
        //bool flag = Authorization("lol1");
    }
    else
    {
        qDebug() << "Что-то пошло не так: " << db.lastError();
    }

    //db.close();
}

bool Server::Authorization(QString username) //, QString password)
{
    QSqlQuery query(db);
    query.prepare("SELECT name FROM user WHERE user.name LIKE ?");
    query.addBindValue(username);
    if (query.exec())
        if(query.next())
        {
            qDebug() << username << " есть в базе данных";

            return true;
        }
        else
        {
            qDebug() << username << " отсутствует в базе данных";
        }
    else {
        qDebug() << "Error executing query: " << query.lastError().text();
    }
    /*if (query.exec())
        QString name = "";
        while (query.next()) {
            name = query.value(0).toString();
            // Здесь вы можете использовать результаты запроса, например, вывести их в консоль или сохранить их в QList
        }
        return true;
    else {
        qDebug() << "Error executing query: " << query.lastError().text();
    }*/
    //delete query;
    return false;
}


void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    //QString descrip = socketDescriptor;
    Sockets.push_back(socket); //Сделать для сокетов имена, к примеру в list или map
    qDebug() << "client connected " << socketDescriptor;
    Counter++;
}

//bool flag = Authorization("lol");

void Server::WriteUserInfoToDB(QJsonObject user)
{
    QSqlQuery query(db);
    //bool flag = Authorization("lol");
    query.prepare("INSERT INTO user (pk_user, name, is_online, password) VALUES (:pk_user, :name, :is_online, :password)"); //:pk_user,
    query.bindValue(":pk_user", Counter+10);
    query.bindValue(":name", user.value("username"));
    query.bindValue(":is_online", 1);
    query.bindValue(":password", user.value("password"));
    //
    if (query.exec())
        qDebug() << "Ну типа да";
    else {
        qDebug() << "Error executing query: " << query.lastError().text();
    }

    //delete query;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_7);
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
        /*QString str;
        in >> str;
        qDebug() << str;
        SendToClient(str);*/
        while(true)
        {
            if(nextBlockSize == 0)
            {
                qDebug() << "nextBlockSize = 0";
                if(socket->bytesAvailable() < 2)
                {
                    qDebug() << "Data < 2, break";
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)
            {
                qDebug() << "Data not full, break";
                break;
            }
            QString str;
            QJsonObject user;
            QTime time;
            //проверка типа
            QString type = "tak";
            in >> type;
            qDebug() << "Тип сообщения: " << type;
            if(type == "message")
            {
                in >> time >> str;
                qDebug() << str;
                //nextBlockSize = 0;
                SendToClient(str);
            }
            else if(type == "userinfo")
            {
                in >> user;
                //проверка на существование
                if(!Authorization(user.value("username").toString()))
                {
                    WriteUserInfoToDB(user);
                    qDebug() << "Добавлен: " << user;
                }
                else
                    qDebug() << "Пользователь не добавлен";
                //nextBlockSize = 0;
            }
            else
            {
                qDebug() << "Неизвестный тип данных: " << type;
            }
            nextBlockSize = 0;


            break;
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void Server::SendToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    //out << str;
    //socket->write(Data);
    int size_of_sockets = Sockets.size();
    for(int i = 0; i < size_of_sockets; i++)
    {
        Sockets[i]->write(Data);
    }
}
