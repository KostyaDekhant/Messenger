#include <server.h>


struct User
{
    QString name;
    int descrip;
}users[100];

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
    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Server::CheckSocketStatus);
    timer->start(2000);
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
    return false;
}


void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    //connect(socket, &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);

    //QString descrip = socketDescriptor;
    Sockets.push_back(socket); //Сделать для сокетов имена, к примеру в list или map
    qDebug() << "client connected " << socketDescriptor;
    SendDescripToClient(socketDescriptor);
    users[Counter++].descrip = socketDescriptor;
    //Counter++;
}

void Server::DeleteSocket(int index)
{
    qDebug() << "Удалён " << index << " элемент";
    QTcpSocket* Socket = new  QTcpSocket();
    Socket->state() == !QAbstractSocket::ConnectedState;
    Sockets.replace(index, Socket);//nullptr
}

//возможно, беда с новыми пользователями, они чёт отключаются с ошибкой
void Server::CheckSocketStatus()
{
    int count = 0;
    for(QTcpSocket* Socket : Sockets)
    {
        //сделать сразу проверку, есть ли такой же пользователь
        if(Socket->state() != QAbstractSocket::ConnectedState && users[count].descrip != -1) {
            qDebug() << "client disconnected: " << users[count].descrip;// << Socket->socketDescriptor();
            QString str = "client disconnected " + users[count].name;
            SendOtherToClient(str, count);

            SetClientStatus(count, 0);
            users[count].descrip = -1;
            users[count].name = "";
            DeleteSocket(count);
        }
        else if(Socket->state() == QAbstractSocket::ConnectedState && !GetClientStatus(count) && users[count].name != "")
        {
            SetClientStatus(count, 1);
            qDebug() << "Пользователь был авторизован с нескольких устройств!";
        }

        count++;
    }
}


bool Server::GetClientStatus(int index)
{
    if(users[index].name == "")
    {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("SELECT is_online FROM user WHERE user.name LIKE ?");
    query.addBindValue(users[index].name);
    int status = 0;
    if (query.exec()) {
        query.next();
        status = query.value(0).toInt();
    }
    return status == 1 ? true : false;
}

void Server::SetClientStatus(int index, int status)
{
    QSqlQuery query(db);
    query.prepare("UPDATE user SET is_online = :stat WHERE name = :username");
    query.bindValue(":stat", status);
    query.bindValue(":username", users[index].name);
    if (query.exec())
    {
        if(!status)
            qDebug() << "Пользователь " << users[index].name << " не в сети";
    }
    else
    {
        qDebug() << "Ошибка при изменении статуса пользователя: " << query.lastError().text();
    }
}

int Server::MaxPKUser()
{
    QSqlQuery query(db);
    query.prepare("SELECT MAX(pk_user) FROM user");
    int max = 0;
    if (query.exec()) {
        query.next();
        max = query.value(0).toInt();
    }
    return max;
}

void Server::WriteUserInfoToDB(QJsonObject user)
{
    QSqlQuery query(db);
    int max_pk = MaxPKUser()+1;
    query.prepare("INSERT INTO user (pk_user, name, is_online, password) VALUES (:pk_user, :name, :is_online, :password)"); //:pk_user,
    query.bindValue(":pk_user", max_pk);
    query.bindValue(":name", user.value("username"));
    query.bindValue(":is_online", 1);
    query.bindValue(":password", user.value("password"));
    //
    if (query.exec())
        qDebug() << "Добавление пользователя прошло успешно.";
    else {
        qDebug() << "Error executing query: " << query.lastError().text();
    }
}

int Server::GetIdClient(int descrip)
{
    int count = 0;
    for(QTcpSocket* Socket : Sockets)
    {
        if(Socket->state() == QAbstractSocket::ConnectedState)
            if(Socket->socketDescriptor() == descrip)
                return count;
        count++;
    }
    return -1;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_7);
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
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
            int descrip;
            QJsonObject user;
            QTime time;
            //проверка типа
            QString type = "";
            in >> type;
            qDebug() << "Тип сообщения: " << type;
            if(type == "message")
            {
                in >> time >> str >> descrip;
                QString sender_name = users[GetIdClient(descrip)].name;
                qDebug() << sender_name << ": " << str;
                //nextBlockSize = 0;
                SendToClient(str, sender_name);
            }
            else if(type == "userinfo")
            {
                in >> user;
                qDebug() << user;
                //проверка на существование
                QString username = user.value("username").toString();
                int index = GetIdClient(user.value("descrip").toInt());
                if(!Authorization(username))
                {
                    WriteUserInfoToDB(user);
                    qDebug() << "Добавлен: " << user;

                    if(index == -1)
                    {
                        qDebug() << "Error 1";
                        return;
                    }
                    users[index].name = username;
                }
                else
                {
                    if(index == -1)
                    {
                        qDebug() << "Error 2";
                        return;
                    }
                    qDebug() << "Пользователь не добавлен, так как уже существует";
                    users[index].name = user.value("username").toString();
                    users[index].descrip = user.value("descrip").toInt();

                }
                SetClientStatus(index, 1);

                //Вынести это потом нормально, проверить на наличие в бд и потом при удачном входе разослать всем смс об этом
                str = "client connected " + users[index].name;
                SendOtherToClient(str, index);

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

void Server::SendToClient(QString str, QString username)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    QString type = "message";
    out << quint16(0) << type << QTime::currentTime() << str << username;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    int size_of_sockets = Sockets.size();
    for(int i = 0; i < size_of_sockets; i++)
    {
        Sockets[i]->write(Data);
    }
}


void Server::SendOtherToClient(QString str, int index)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    QString type = "message";
    out << quint16(0) << type << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    int size_of_sockets = Sockets.size();
    for(int i = 0; i < size_of_sockets; i++)
    {
        if(i != index && Sockets[i]->state() == QAbstractSocket::ConnectedState)
            Sockets[i]->write(Data);
    }
}


void Server::SendDescripToClient(int desctip)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    QString type = "descrip";
    out << quint16(0) << type << desctip;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    int size_of_sockets = Sockets.size();
    for(int i = 0; i < size_of_sockets; i++)
    {
        if(Sockets[i]->state() == QAbstractSocket::ConnectedState)
        {
            //qDebug() << "if";
            if(desctip == Sockets[i]->socketDescriptor())
                Sockets[i]->write(Data);
        }
        else
        {
            //qDebug() << "else";
        }
    }
}

//убрать
void Server::SendToAuth()
{

}
