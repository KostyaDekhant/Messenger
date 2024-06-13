#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include <authorization.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFile>
//#include <QFileDialog>
#include <QStandardItem>


#include <QScrollBar>
#include <QMessageBox>

#include <QVBoxLayout>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

    void on_Auth_bttn_clicked();

    void on_addChat_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray Data;
    quint16 nextBlockSize;
    int descriptor;
    int currentChat = 1;//-1


    QWidget *user_widget;
    QVBoxLayout *user_layout;

    QList<QPushButton*> buttonList;

    template<typename T>
    void SendToServer(T arg);
    void SendUserDataToServer(QJsonObject userData);
    void OnlineIcons(QStringList stringList);


    void TypeMessageDetect(QString str);

    QJsonDocument doc;
    QJsonParseError docError;
    QString globPath;
    Authorization *auth;
    QJsonObject user;
public slots:
    void slotReadyRead();
    void auth_slot(QJsonObject userData);
    void signup_slot(QJsonObject userData);
    void onButtonClicked();

    //void SendSocketToServer();
    //void ClientDisconnected(QTcpSocket socket);
signals:
    void authsignal(bool isSuccess);
    void signupsignal(bool isSuccess);
    void authslotChanged();

};
#endif // MAINWINDOW_H
