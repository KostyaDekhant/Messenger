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
    void on_connectBttn_clicked();

    void on_send_msg_bttn_clicked();

    void on_lineEdit_returnPressed();

    void on_Auth_bttn_clicked();

    void on_findUser_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray Data;
    quint16 nextBlockSize;
    int descriptor;
    int currentChat = -1;//-1


    QWidget *user_widget;
    QVBoxLayout *user_layout;

    QList<QPushButton*> buttonList;

    template<typename T>
    void SendToServer(T arg);

    void deleteLater();

    void OnlineIcons(QJsonArray stringList);

    void AcceptMessResponse(QJsonValue value);

    void TypeMessageDetect(QString str);

    void clearLayout(QVBoxLayout* layout);


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

signals:
    void authsignal(bool isSuccess);
    void signupsignal(bool isSuccess);
    void authslotChanged();

};
#endif // MAINWINDOW_H
