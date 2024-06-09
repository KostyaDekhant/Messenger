#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auth = new Authorization;
    connect(this, &MainWindow::authsignal, auth, &Authorization::authslot);
    connect(auth, &Authorization::authsignal, this, &MainWindow::authslot);

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    //connect(socket, &QTcpSocket::disconnected,  this, &MainWindow::SendSocketToServer)
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    nextBlockSize = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    socket->connectToHost("127.0.0.1", 2323);

}

void MainWindow::SendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    QString msg_type = "message";
    out << quint16(0) << msg_type << QTime::currentTime() << str << user.value("descrip").toInt();
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
    ui->lineEdit->clear();
}

void MainWindow::SendUserDataToServer(QJsonObject userData)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    QString msg_type = "userinfo";
    out << quint16(0) << msg_type << userData;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    socket->write(Data);
}


void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_7);
    if(in.status() == QDataStream::Ok)
    {
        while(true)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }
            QString str, sender_name;
            QTime time;
            QString type = "";
            in >> type;
            if(type == "message")
            {
                in >> time >> str >> sender_name;
                QString full_msg;
                qDebug() << sender_name << " " << user["username"].toString();
                if(sender_name != user["username"].toString()) //левая сторона, чужие смс
                {
                    full_msg = time.toString()+ " " + sender_name + ": " + str;
                    ui->textBrowser->insertHtml("<br><p style=\"text-align:left;\"><font size=\"5\"><b>" + str + "</b></font></p>"
                                                "<p style=\"text-align:left;\"><font size=\"1\">" + time.toString() + "</font></p>");

                }
                else //правая сторона, свои смс
                {
                    full_msg = str + " ";

                    ui->textBrowser->insertHtml(
                                                "<br><p style=\"text-align:right;\"><font size=\"5\"><b>" + str + "</b></font></p>"
                                                "<p style=\"text-align:right;\"><font size=\"1\">" + time.toString() + "</font></p>");
                }
                ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
            }
            else if (type == "descrip")
            {
                in >> descriptor;
            }
            nextBlockSize = 0;

        }
    }
    else
    {
        ui->textBrowser->append("read error: ");
    }
}

void MainWindow::authslot(QJsonObject userData)
{
    auth->close();
    userData["descrip"] = descriptor;
    SendUserDataToServer(userData);
    user = userData;
    ui->Auth_bttn->setEnabled(false);
    this->setWindowTitle(user["username"].toString());
    this->show();
}


void MainWindow::on_pushButton_2_clicked()
{
    SendToServer(ui->lineEdit->text());
}


void MainWindow::on_lineEdit_returnPressed()
{
    SendToServer(ui->lineEdit->text());
}

void MainWindow::on_Auth_bttn_clicked()
{
    this->hide();
    auth->show();
    emit authsignal();
}

