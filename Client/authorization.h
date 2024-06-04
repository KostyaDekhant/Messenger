#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H


#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFile>
//#include <QFileDialog>
#include <QStandardItem>

#include <QWidget>

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
signals:
    void authsignal(QJsonObject userData);
public slots:
    void authslot();
private slots:
    void on_pushButton_clicked();
    void on_Login_bttn_clicked();
};

#endif // AUTHORIZATION_H
