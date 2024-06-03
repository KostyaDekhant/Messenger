#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
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
    QByteArray Data;
    void SendToClient(QString str);
    quint16 nextBlockSize;
    QSqlDatabase db;
    bool Authorization(QSqlDatabase db, QString username);
public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
