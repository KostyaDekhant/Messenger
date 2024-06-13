#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QTcpSocket>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFile>
//#include <QFileDialog>
#include <QStandardItem>

#include <QWidget>

#include <QCryptographicHash>

//#include <mainwindow.h>

namespace Ui {
class Authorization;
}

class Authorization : public QWidget
{
    Q_OBJECT

public:
    explicit Authorization(QWidget *parent = nullptr);
    ~Authorization();

private:
    Ui::Authorization *ui;
    QJsonObject userData;
    bool successful_auth = false;
    QTcpSocket *socket;
    quint16 nextBlockSize = 0;

    bool isSignUp = false;


    void Login();

    void Signup();

signals:
    void auth_signal(QJsonObject userData);
    void signup_signal(QJsonObject userData);
public slots:
    void slotReadyRead();
    void authslot();
    void success_slot(bool isSuccess);
private slots:
    void on_Login_bttn_clicked();
    void on_sign_up_bttn_clicked();
};

#endif // AUTHORIZATION_H
