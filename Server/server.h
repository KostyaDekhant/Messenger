#ifndef SERVER_H
#define SERVER_H
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

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    QTcpSocket *socket;
private:
    QVector <QTcpSocket*> Sockets;
    int Counter = 0;
    QByteArray Data;
    void SendToClient(QString str, QString username);
    void SendDescripToClient(int desctip);
    void SendOtherToClient(QString str, int index);
    void SendToAuth();
    quint16 nextBlockSize;
    QSqlDatabase db;
    bool Authorization(QString username);
    void WriteUserInfoToDB(QJsonObject user);
    int MaxPKUser();
    void CheckSocketStatus();
    void SetClientStatus(int index, int status);
    bool GetClientStatus(int index);
    int GetIdClient(int descrip);
    void DeleteSocket(int index);
    //void DeleteSocket(QTcpSocket *Socket);
    //QMap<int, QString> users;
public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
