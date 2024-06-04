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

    void on_pushButton_3_clicked();

    void on_Auth_bttn_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray Data;
    void SendToServer(QString str);
    void SendUserDataToServer(QJsonObject userData);
    quint16 nextBlockSize;

    QJsonDocument doc;
    QJsonParseError docError;
    QString globPath;
    Authorization *auth;
    QJsonObject user;
public slots:
    void slotReadyRead();
    void authslot(QJsonObject userData);
signals:
    void authsignal();
    void authslotChanged();
};
#endif // MAINWINDOW_H
