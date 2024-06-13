/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QFormLayout *formLayout_2;
    QFrame *frame_4;
    QGridLayout *gridLayout;
    QFrame *frame_2;
    QFormLayout *formLayout;
    QPushButton *Auth_bttn;
    QFrame *frame_3;
    QGridLayout *gridLayout_3;
    QLineEdit *username_lb;
    QPushButton *addChat;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QLineEdit *lineEdit;
    QTextBrowser *textBrowser;
    QMenuBar *menubar;
    QMenu *menu;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(544, 440);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        formLayout_2 = new QFormLayout(centralwidget);
        formLayout_2->setObjectName("formLayout_2");
        frame_4 = new QFrame(centralwidget);
        frame_4->setObjectName("frame_4");
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_4);
        gridLayout->setObjectName("gridLayout");
        frame_2 = new QFrame(frame_4);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        formLayout = new QFormLayout(frame_2);
        formLayout->setObjectName("formLayout");
        Auth_bttn = new QPushButton(frame_2);
        Auth_bttn->setObjectName("Auth_bttn");

        formLayout->setWidget(0, QFormLayout::SpanningRole, Auth_bttn);

        frame_3 = new QFrame(frame_2);
        frame_3->setObjectName("frame_3");
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        gridLayout_3 = new QGridLayout(frame_3);
        gridLayout_3->setObjectName("gridLayout_3");
        username_lb = new QLineEdit(frame_3);
        username_lb->setObjectName("username_lb");

        gridLayout_3->addWidget(username_lb, 0, 0, 1, 1);

        addChat = new QPushButton(frame_3);
        addChat->setObjectName("addChat");

        gridLayout_3->addWidget(addChat, 1, 0, 1, 1);


        formLayout->setWidget(1, QFormLayout::SpanningRole, frame_3);

        scrollArea = new QScrollArea(frame_2);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 147, 187));
        scrollArea->setWidget(scrollAreaWidgetContents);

        formLayout->setWidget(2, QFormLayout::SpanningRole, scrollArea);


        gridLayout->addWidget(frame_2, 0, 0, 1, 1);


        formLayout_2->setWidget(0, QFormLayout::LabelRole, frame_4);

        frame = new QFrame(centralwidget);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName("gridLayout_2");
        pushButton_2 = new QPushButton(frame);
        pushButton_2->setObjectName("pushButton_2");

        gridLayout_2->addWidget(pushButton_2, 2, 1, 1, 1);

        pushButton = new QPushButton(frame);
        pushButton->setObjectName("pushButton");

        gridLayout_2->addWidget(pushButton, 0, 0, 1, 2);

        lineEdit = new QLineEdit(frame);
        lineEdit->setObjectName("lineEdit");

        gridLayout_2->addWidget(lineEdit, 2, 0, 1, 1);

        textBrowser = new QTextBrowser(frame);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        gridLayout_2->addWidget(textBrowser, 1, 0, 1, 2);


        formLayout_2->setWidget(0, QFormLayout::FieldRole, frame);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 544, 25));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        menu->setTearOffEnabled(true);
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        Auth_bttn->setText(QCoreApplication::translate("MainWindow", "\320\220\320\262\321\202\320\276\321\200\320\270\320\267\320\260\321\206\320\270\321\217", nullptr));
        addChat->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214 \321\207\320\260\321\202", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", ">", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        menu->setTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
