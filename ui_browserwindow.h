/********************************************************************************
** Form generated from reading UI file 'browserwindow.ui'
**
** Created: Mon Oct 26 17:42:28 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BROWSERWINDOW_H
#define UI_BROWSERWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BrowserWindow
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BrowserWindow)
    {
        if (BrowserWindow->objectName().isEmpty())
            BrowserWindow->setObjectName(QString::fromUtf8("BrowserWindow"));
        BrowserWindow->resize(600, 400);
        centralWidget = new QWidget(BrowserWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(360, 170, 113, 32));
        BrowserWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BrowserWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 22));
        BrowserWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BrowserWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        BrowserWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(BrowserWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        BrowserWindow->setStatusBar(statusBar);

        retranslateUi(BrowserWindow);
        QObject::connect(pushButton, SIGNAL(clicked()), BrowserWindow, SLOT(TestClicked()));

        QMetaObject::connectSlotsByName(BrowserWindow);
    } // setupUi

    void retranslateUi(QMainWindow *BrowserWindow)
    {
        BrowserWindow->setWindowTitle(QApplication::translate("BrowserWindow", "BrowserWindow", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("BrowserWindow", "Test", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BrowserWindow: public Ui_BrowserWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BROWSERWINDOW_H
