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
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *pushButton;

    void setupUi(QWidget *Authorization)
    {
        if (Authorization->objectName().isEmpty())
            Authorization->setObjectName("Authorization");
        Authorization->resize(321, 251);
        gridLayout_2 = new QGridLayout(Authorization);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        lineEdit_2 = new QLineEdit(Authorization);
        lineEdit_2->setObjectName("lineEdit_2");

        gridLayout->addWidget(lineEdit_2, 6, 0, 1, 1);

        label_3 = new QLabel(Authorization);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 5, 0, 1, 1);

        label_2 = new QLabel(Authorization);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        label = new QLabel(Authorization);
        label->setObjectName("label");
        QFont font;
        font.setPointSize(30);
        label->setFont(font);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        lineEdit = new QLineEdit(Authorization);
        lineEdit->setObjectName("lineEdit");

        gridLayout->addWidget(lineEdit, 4, 0, 1, 1);

        pushButton = new QPushButton(Authorization);
        pushButton->setObjectName("pushButton");

        gridLayout->addWidget(pushButton, 7, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(Authorization);

        QMetaObject::connectSlotsByName(Authorization);
    } // setupUi

    void retranslateUi(QWidget *Authorization)
    {
        Authorization->setWindowTitle(QCoreApplication::translate("Authorization", "Form", nullptr));
        label_3->setText(QCoreApplication::translate("Authorization", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        label_2->setText(QCoreApplication::translate("Authorization", "\320\233\320\276\320\263\320\270\320\275", nullptr));
        label->setText(QCoreApplication::translate("Authorization", "\320\220\320\262\321\202\320\276\321\200\320\270\320\267\320\260\321\206\320\270\321\217", nullptr));
        pushButton->setText(QCoreApplication::translate("Authorization", "\320\222\320\276\320\271\321\202\320\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Authorization: public Ui_Authorization {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTHORIZATION_H
