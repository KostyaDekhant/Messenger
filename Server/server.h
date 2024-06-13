#ifndef SERVER_H
#define SERVER_H

#include <iostream>

#include <windows.h>

#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
//#include <QFileDialog>
#include <QStandardItem>

#include <QSqlRecord>


class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    QTcpSocket *socket;
private:
    //QVector <QTcpSocket*> Sockets;
    QMap <QString, QTcpSocket*> Sockets;
    quint16 nextBlockSize;
    QSqlDatabase db;
    int Counter = 0;
    QByteArray Data;

    template<typename T>
    void SendToClient(T arg, QTcpSocket* user);
    void ConfMessage(QJsonValue jVal);


    void SendDescripToClient(int desctip);
    void SendOtherToClient(QString str, int index);
    void SendOnlineUsers(QStringList stringList);
    void SendToAuth();


    bool Validlogin(QString username);
    bool CheckAuth(QJsonObject user);

    void WriteUserInfoToDB(QJsonObject user);
    int MaxPKUser();
    void CheckSocketStatus();
    void SendOnlineUsersToEverybody();
    void SetClientStatus(int index, int status);
    bool GetClientStatus(int index);
    int GetIdClient(int descrip);
    int GetDBIdClient(QString username);

    void TypeMessageDetect(QString str);


    //void DeleteSocket(QTcpSocket *Socket);
    //QMap<int, QString> users;
public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
    void slotDeleteLater();

};

#endif // SERVER_H
