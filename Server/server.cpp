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
    db.setDatabaseName("C:/qt projects/users.db"); //C:/qt projects/
    if(db.open())
    {
        qDebug() << "База данных подключена успешно.";
        //bool flag = Authorization(db, "lol", );
    }
    else
    {
        qDebug() << "Что-то пошло не так: " << db.lastError();
    }

    db.close();
}

bool Server::Authorization(QSqlDatabase db, QString username) //, QString password)
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
    return false;
}


void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    Sockets.push_back(socket);
    qDebug() << "client connected " << socketDescriptor;
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
            QTime time;
            in >> time >> str;
            nextBlockSize = 0;
            qDebug() << str;
            SendToClient(str);
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
