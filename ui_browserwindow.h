/********************************************************************************
** Form generated from reading UI file 'browserwindow.ui'
**
** Created: Tue Oct 27 02:45:22 2009
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
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTableView>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BrowserWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTableView *serverView;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QPushButton *RefreshButton;
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
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        serverView = new QTableView(centralWidget);
        serverView->setObjectName(QString::fromUtf8("serverView"));

        verticalLayout->addWidget(serverView);

        frame = new QFrame(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        RefreshButton = new QPushButton(frame);
        RefreshButton->setObjectName(QString::fromUtf8("RefreshButton"));

        horizontalLayout->addWidget(RefreshButton);


        verticalLayout->addWidget(frame);

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
        QObject::connect(RefreshButton, SIGNAL(clicked()), BrowserWindow, SLOT(RefreshClicked()));

        QMetaObject::connectSlotsByName(BrowserWindow);
    } // setupUi

    void retranslateUi(QMainWindow *BrowserWindow)
    {
        BrowserWindow->setWindowTitle(QApplication::translate("BrowserWindow", "BrowserWindow", 0, QApplication::UnicodeUTF8));
        RefreshButton->setText(QApplication::translate("BrowserWindow", "Refresh", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BrowserWindow: public Ui_BrowserWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BROWSERWINDOW_H
