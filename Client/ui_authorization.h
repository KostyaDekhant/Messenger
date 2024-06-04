/********************************************************************************
** Form generated from reading UI file 'authorization.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTHORIZATION_H
#define UI_AUTHORIZATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Authorization
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLineEdit *password_line;
    QLabel *Password_lb;
    QLabel *Login_lb;
    QLabel *Auth_lb;
    QLineEdit *login_line;
    QPushButton *Login_bttn;

    void setupUi(QWidget *Authorization)
    {
        if (Authorization->objectName().isEmpty())
            Authorization->setObjectName("Authorization");
        Authorization->resize(321, 251);
        gridLayout_2 = new QGridLayout(Authorization);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        password_line = new QLineEdit(Authorization);
        password_line->setObjectName("password_line");

        gridLayout->addWidget(password_line, 6, 0, 1, 1);

        Password_lb = new QLabel(Authorization);
        Password_lb->setObjectName("Password_lb");

        gridLayout->addWidget(Password_lb, 5, 0, 1, 1);

        Login_lb = new QLabel(Authorization);
        Login_lb->setObjectName("Login_lb");

        gridLayout->addWidget(Login_lb, 2, 0, 1, 1);

        Auth_lb = new QLabel(Authorization);
        Auth_lb->setObjectName("Auth_lb");
        QFont font;
        font.setPointSize(30);
        Auth_lb->setFont(font);

        gridLayout->addWidget(Auth_lb, 1, 0, 1, 1);

        login_line = new QLineEdit(Authorization);
        login_line->setObjectName("login_line");

        gridLayout->addWidget(login_line, 4, 0, 1, 1);

        Login_bttn = new QPushButton(Authorization);
        Login_bttn->setObjectName("Login_bttn");

        gridLayout->addWidget(Login_bttn, 7, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(Authorization);

        QMetaObject::connectSlotsByName(Authorization);
    } // setupUi

    void retranslateUi(QWidget *Authorization)
    {
        Authorization->setWindowTitle(QCoreApplication::translate("Authorization", "Form", nullptr));
        Password_lb->setText(QCoreApplication::translate("Authorization", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        Login_lb->setText(QCoreApplication::translate("Authorization", "\320\233\320\276\320\263\320\270\320\275", nullptr));
        Auth_lb->setText(QCoreApplication::translate("Authorization", "\320\220\320\262\321\202\320\276\321\200\320\270\320\267\320\260\321\206\320\270\321\217", nullptr));
        Login_bttn->setText(QCoreApplication::translate("Authorization", "\320\222\320\276\320\271\321\202\320\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Authorization: public Ui_Authorization {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTHORIZATION_H
