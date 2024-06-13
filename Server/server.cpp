#include <server.h>


struct User
{
    QString name;
    int descrip;
    int id;
}users[100];

Server::Server()
{
    setlocale(LC_ALL, "ru");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    if(this->listen(QHostAddress::Any, 27015))
    {
        qDebug() << "start";
    }
    else
    {
        qDebug() << "Error: ";
    }
    nextBlockSize = 0;
    db = QSqlDatabase::addDatabase("QSQLITE", "SQLITE");
    //db.setDatabaseName("./../users.db"); //C:/qt projects/
    db.setDatabaseName("./../messenger_db.db"); //C:/qt projects/
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
    connect(timer, &QTimer::timeout, this, &Server::SendOnlineUsersToEverybody);
    //timer->start(2000);
    //db.close();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::slotDeleteLater);

    //connect(socket, &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);
    //QString descrip = socketDescriptor;
    //Sockets.push_back(socket); //Сделать для сокетов имена, к примеру в list или map

    qDebug() << "client connected " << socketDescriptor;

    /*Sockets.insert(,socket);
    SendDescripToClient(socketDescriptor);
    users[Counter++].descrip = socketDescriptor;*/

    //Counter++;
}

void Server::slotDeleteLater()
{
    socket = (QTcpSocket*)sender();
    qDebug() << "Удалён " << socket << " элемент";
    Sockets.erase(std::remove(Sockets.begin(), Sockets.end(), socket), Sockets.end());
    socket->deleteLater();
}

//возможно, беда с новыми пользователями, они чёт отключаются с ошибкой
void Server::CheckSocketStatus()
{
    /*
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
    */
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
    query.prepare("SELECT COALESCE(MAX(pk_user), 0) FROM user");
    int max = 0;
    if (query.exec()) {
        query.next();
        max = query.value(0).toInt();
    }
    return max;
}

void Server::WriteUserInfoToDB(QJsonObject user)
{
    qDebug() << "Json file " << user;
    QSqlQuery query(db);
    int max_pk = MaxPKUser()+1;
    query.prepare("INSERT INTO user (pk_user, name, is_online, password) VALUES (:pk_user, :name, :is_online, :password)"); //:pk_user,
    query.bindValue(":pk_user", max_pk);
    query.bindValue(":name", user["username"].toString());
    query.bindValue(":is_online", 1);
    query.bindValue(":password", user["password"].toString());
    //
    if (query.exec())
        qDebug() << "Добавление пользователя прошло успешно.";
    else {
        qDebug() << "Error executing query: " << query.lastError().text();
    }
}

//Id среди подключенных пользователей, потом это надо будет убрать или изменить
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

//Брать инфу с базы, узнаём айди пользователя с таким ником
int Server::GetDBIdClient(QString username)
{
    int index = -1;
    QSqlQuery query(db);
    query.prepare("SELECT pk_user FROM user WHERE user.name LIKE ?");
    query.addBindValue(username);
    if (query.exec()) {
        query.next();
        index = query.value(0).toInt();
    }
    return index;
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
            in >> str;

            TypeMessageDetect(str);
            /*


            */


            nextBlockSize = 0;


            break;
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}


void Server::TypeMessageDetect(QString str)
{
    QJsonDocument jDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jObj = jDoc.object();
    qDebug() << "JSON: " << jObj;
    QString type = jObj["type"].toString();

    if(type == "message")
    {
        ConfMessage(jObj["value"]);
    }
    else if(type == "auth")
    {
        bool flag = false;
        QJsonObject temp = jObj["value"].toObject();
        if(!Validlogin(temp["username"].toString()))
        {
            qDebug() << "Пользователь " << temp["username"].toString() << "уже зарегистрирован!";
            if(!CheckAuth(jObj["value"].toObject()))
            {
                qDebug() << "Не правильно введены логин или пароль!";
            }
            else
            {
                qDebug() << "Авторизация прошла успешно!";
                flag = true;
            }
        }
        else
        {
            qDebug() << "Пользователь " << temp["username"].toString() << "ещё не зарегистрирован!";
        }

        QJsonObject temp_obj;
        temp_obj.insert("type", "auth");
        temp_obj.insert("value", flag);
        qDebug() << temp_obj["value"].toBool();

        QJsonDocument jDoc = QJsonDocument(temp_obj);
        QString jStr = QString(jDoc.toJson());
        SendToClient(jStr, socket);
        /*in >> user;
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

        //id из бд
        users[index].id = GetDBIdClient(users[index].name);

        //Вынести это потом нормально, проверить на наличие в бд и потом при удачном входе разослать всем смс об этом
        str = "client connected " + users[index].name;
        SendOtherToClient(str, index);


        */
    }
    else if(type == "signup")
    {
        bool flag = false;
        QJsonObject temp = jObj["value"].toObject();
        if(!Validlogin(temp["username"].toString()))
        {
            qDebug() << "Пользователь " << temp["username"].toString() << "уже зарегистрирован!";
        }
        else
        {
            qDebug() << "Регистрация прошла успешно!";
            WriteUserInfoToDB(jObj["value"].toObject());
            flag = true;
        }
        QJsonObject temp_obj;
        temp_obj.insert("type", "signup");
        temp_obj.insert("value", flag);
        qDebug() << temp_obj["value"].toBool();

        QJsonDocument jDoc = QJsonDocument(temp_obj);
        QString jStr = QString(jDoc.toJson());
        SendToClient(jStr, socket);
    }
    else
    {
        qDebug() << "Неизвестный тип данных: " << type;
    }
}


bool Server::CheckAuth(QJsonObject user)
{

    QSqlQuery query(db);
    query.prepare("SELECT name, password FROM user WHERE name == :username");
    query.bindValue(":username", user["username"].toString());
    query.exec();
    QSqlRecord rec = query.record();
    query.next();
    if (query.value(rec.indexOf("password")).toString() == user["password"].toString()) {
        qDebug() << "Успешная авторизация " << user["username"].toString();
         return true; //авториз
    }
    else {
        qDebug() << "Ошибка при авторизации " <<  user["username"].toString();
    }
    return false;//не авториз
}


bool Server::Validlogin(QString username)
{
    QSqlQuery query(db);
    query.prepare("SELECT name FROM user WHERE user.name LIKE ?");
    query.addBindValue(username);
    if (query.exec())
        if(query.next())
        {
            qDebug() << username << " есть в базе данных";

            return false;
        }
        else
        {
            qDebug() << username << " отсутствует в базе данных";
        }
    else {
        qDebug() << "Error executing query: " << query.lastError().text();
    }
    return true;
}

void Server::ConfMessage(QJsonValue jVal)
{

}


//убрать или переделать
void Server::SendOnlineUsersToEverybody()
{
    /*int size_of_sockets = Sockets.size();

    QStringList stringlist;
    for(int i = 0; i <  size_of_sockets; i++)
    {
        if(Sockets[i]->state() == QAbstractSocket::ConnectedState && users[i].name != "")
            stringlist << users[i].name;
    }
    if(!stringlist.size())
        return;
    SendOnlineUsers(stringlist);
    qDebug() << stringlist;
    qDebug() << "Список отправлен";
    */
}


template<typename T>
void Server::SendToClient(T arg, QTcpSocket* user)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    out << quint16(0) << arg;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    if (user == nullptr) user = this->socket;
    user->write(Data);
}

//три метода убрать тупо
void Server::SendOtherToClient(QString str, int index)
{
    /*
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
    }*/
}

void Server::SendOnlineUsers(QStringList stringList)
{
    /*
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    QString type = "who's_online";
    out << quint16(0) << type << stringList;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    int size_of_sockets = Sockets.size();
    for(int i = 0; i < size_of_sockets; i++)
    {
        if(Sockets[i]->state() == QAbstractSocket::ConnectedState && users[i].name != "")
            Sockets[i]->write(Data);
    }
    */
}

void Server::SendDescripToClient(int desctip)
{
    /*
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
    */
}

//убрать
void Server::SendToAuth()
{

}
