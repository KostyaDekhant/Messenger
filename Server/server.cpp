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
    //connect(timer, &QTimer::timeout, this, &Server::CheckSocketStatus);
    connect(timer, &QTimer::timeout, this, &Server::SendOnlineUsersToEverybody);
    timer->start(2000);
    //db.close();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::slotDeleteLater);

    qDebug() << "client connected " << socketDescriptor;
}

void Server::slotDeleteLater()
{
    socket = (QTcpSocket*)sender();
    qDebug() << "Удалён " << socket << " элемент";
    Sockets.erase(std::remove(Sockets.begin(), Sockets.end(), socket), Sockets.end());
    socket->deleteLater();
}

//Статус сокетов, уже вряд ли надо
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

//Узнать статус пользователя
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

//Установить статус пользователя
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

//Максимальное значение pk_user
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

//Запись пользователя в бд при успешной регистрации
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

//индекс пользователя в бд
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

//чтение данных
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
            nextBlockSize = 0;
            break;
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

//Определение типа сообщения
void Server::TypeMessageDetect(QString str)
{
    QJsonDocument jDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jObj = jDoc.object();
    qDebug() << "JSON: " << jObj;
    QString type = jObj["type"].toString();

    if(type == "message")
    {
        ConfMessage(jObj["value"].toObject());
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

        if(flag)
        {
            int id = GetDBIdClient(temp["username"].toString());
            qDebug() << "ID " << id;
            temp_obj.insert("id", id);
            Sockets.insert(temp["username"].toString(), socket); //Список пользователей
        }
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

        if(flag)
        {
            int id = GetDBIdClient(temp["username"].toString());
            qDebug() << "ID " << id;
            temp_obj.insert("id", id);
            Sockets.insert(temp["username"].toString(), socket);
        }

        qDebug() << temp_obj["value"].toBool();

        QJsonDocument jDoc = QJsonDocument(temp_obj);
        QString jStr = QString(jDoc.toJson());
        SendToClient(jStr, socket);

    }
    else if (type == "open_chat")
    {
        QJsonObject temp = jObj["value"].toObject();


        int id = findOrCreateChatId(temp["sender"].toString(), temp["recipient"].toString());
        qDebug() << "Id чата: " << id;

        AcceptReqForDialogs(id);
    }
    else
    {
        qDebug() << "Неизвестный тип данных: " << type;
    }
}

//Авторизация
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

//Подтверждение логина (есть ли он в бд или нет)
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

//Отправка и запись сообщений в бд
void Server::ConfMessage(QJsonObject message)
{
    QSqlQuery query_chatp(db), query_mess(db);
    //Выбор чата
    query_chatp.prepare("SELECT 'chat_user'.rowid, 'chat_user'.'pk_chat', 'chat_user'.'pk_user' FROM 'user', 'chat_user', "
                        "'chat' WHERE 'user'.'name' == :username AND 'chat_user'.'pk_user' == 'user'.'pk_user' "
                        "AND 'chat_user'.'pk_chat' == 'chat'.'pk_chat' AND 'chat'.'pk_chat' == :chatID");
    query_chatp.bindValue(":chatID", message["pk_chat"].toInt());
    query_chatp.bindValue(":username", message["sender"].toString());
    query_chatp.exec();

    //Запись сообщения
    query_mess.prepare("INSERT INTO 'messages' ('message', 'time', 'pk_user', 'pk_chat') VALUES (:message, CURRENT_TIMESTAMP, :user, :chat)");
    query_mess.bindValue(":message", message["text"].toString());
    QSqlRecord rec = query_chatp.record();

    query_chatp.next();
    query_mess.bindValue(":chat", query_chatp.value(rec.indexOf("pk_chat")).toInt());
    query_mess.bindValue(":user", query_chatp.value(rec.indexOf("pk_user")).toInt());

    if (query_mess.exec()) {
        int id = query_chatp.lastInsertId().toInt();
        QSqlQuery getmess(db);
        getmess.prepare("SELECT 'messages'.'message', 'messages'.'time', 'messages'.'pk_user' FROM 'messages' "
                        "JOIN 'user' ON 'messages'.'pk_user' == 'user'.'pk_user' WHERE 'messages'.rowid == :id");
        getmess.bindValue(":id", id);
        getmess.exec();

        QSqlRecord mess = getmess.record();
        getmess.next();

        QJsonObject temp = message;

        //Берём сообщение, отправителя и время
        temp.insert("text", getmess.value(mess.indexOf("message")).toString());
        temp.insert("time", getmess.value(mess.indexOf("time")).toString());
        temp.insert("sender", getmess.value(mess.indexOf("pk_user")).toString());

        qDebug() << "Сообщение записано в бд!";

        SendSignalToUpdateDialog(temp);
    }
    else qDebug() << "Ошибка при сохранении сообщения! " << query_mess.lastError() << " " << query_mess.lastQuery();

}

//Отправка сигнала на обновление чата
void Server::SendSignalToUpdateDialog(QJsonObject message)
{
    QJsonObject jResObj;
    jResObj.insert("type", "message");
    jResObj.insert("value", message);

    //выбор пользователей чата
    QSqlQuery chatp(db);
    chatp.prepare("SELECT user.'name', 'user'.'pk_user' FROM 'user', 'chat_user' JOIN 'chat' ON 'chat_user'.'pk_chat' == 'chat'.'pk_chat' "
                  "WHERE 'chat'.'pk_chat' == :chatID AND 'user'.'pk_user' == 'chat_user'.'pk_user'");
    chatp.bindValue(":chatID", message["pk_chat"].toInt());
    chatp.exec();
    QSqlRecord rec = chatp.record();

    //отправка сообщений участникам чата
    while (chatp.next()) {
        auto it = Sockets.value(chatp.value(rec.indexOf("name")).toString());
        QJsonDocument jReqDoc = QJsonDocument{jResObj};
        QString jReqStr = QString(jReqDoc.toJson());
        SendToClient(jReqStr, it);
    }
}

//Подтвердить отправку сообщений чата
void Server::AcceptReqForDialogs(int chatID)
{
    QSqlQuery query(db);
    //Берём сообщения указанного чата
    query.prepare("SELECT DISTINCT messages.message, messages.time, user.name FROM user, "
                  "messages JOIN chat ON messages.pk_chat = :chatId WHERE messages.pk_user == user.pk_user ORDER BY time ASC");
    query.bindValue(":chatId", chatID);
    if (query.exec()) qDebug() << "Successful get messages from DB!";
    else {
        qDebug() << "Error get messages from DB";
        return;
    }

    QSqlRecord rec = query.record();
    QJsonArray jArray;

    //Добавление сообщений в массив Json
    while(query.next()) {
        QJsonObject temp;
        temp.insert("time", query.value(rec.indexOf("time")).toString());
        temp.insert("sender", query.value(rec.indexOf("name")).toString());
        temp.insert("text", query.value(rec.indexOf("message")).toString());
        jArray.append(temp);
    }
    QJsonObject jObj;
    jObj.insert("type", "load_dialog");
    jObj.insert("id_chat", chatID);
    jObj.insert("value", jArray);

    QJsonDocument jDoc = QJsonDocument{jObj};
    QString jStr = QString(jDoc.toJson());

    SendToClient(jStr);
}

//Поиск или создание нового чата
int Server::findOrCreateChatId(const QString& user1, const QString& user2) {
    QSqlQuery query(db);

    // Проверяем наличие чата с данными пользователями
    query.prepare("SELECT chat.pk_chat FROM chat "
                  "JOIN chat_user ON chat.pk_chat = chat_user.pk_chat "
                  "JOIN user ON chat_user.pk_user = user.pk_user "
                  "WHERE user.name = :user1 AND chat.pk_chat IN "
                  "(SELECT chat.pk_chat FROM chat JOIN chat_user ON chat.pk_chat = chat_user.pk_chat "
                  "JOIN user ON chat_user.pk_user = user.pk_user "
                  "WHERE user.name = :user2)");

    query.bindValue(":user1", user1);
    query.bindValue(":user2", user2);

    if (query.exec() && query.next()) {
        // Если чат уже существует, выводим его id
        int chatId = query.value(0).toInt();
        qDebug() << "Чат с пользователями " << user1 << " и " << user2 << " уже существует:" << chatId;
        return chatId;
    } else {
        // Если чата нет, создаем новый чат
        query.prepare("INSERT INTO chat (name, isPersonal) VALUES ('New Chat', 1)");
        if (query.exec()) {
            int newChatId = query.lastInsertId().toInt();

            // Добавляем пользователей в новый чат
            query.prepare("INSERT INTO chat_user (pk_chat, pk_user) "
                          "SELECT :chatId, user.pk_user FROM user WHERE user.name IN (:user1, :user2)");
            query.bindValue(":chatId", newChatId);
            query.bindValue(":user1", user1);
            query.bindValue(":user2", user2);

            if (query.exec()) {
                qDebug() << "Новый чат создан, пользователи: " << user1 << " и " << user2 << ", id:" << newChatId;
                return newChatId;
            }
        }
    }
    return -1;
}

void Server::SendOnlineUsersToEverybody()
{
    QStringList stringlist;
    // обходим QMap и сохраняем ключи в список
    QMapIterator<QString, QTcpSocket*> i(Sockets);
    while (i.hasNext()) {
        i.next();
        stringlist.append(i.key());
    }

    if(!stringlist.size())
        return;

    QJsonObject jObj;

    QJsonArray jsonArray;
    for (const QString &str : stringlist) {
        jsonArray.append(str);
    }

    jObj.insert("type", "who's_online");
    jObj.insert("value", jsonArray);
    QJsonDocument jDoc = QJsonDocument{jObj};
    QString jStr = QString(jDoc.toJson());

    SendToClient(jStr);
    qDebug() << stringlist;
    qDebug() << "Список отправлен";

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
