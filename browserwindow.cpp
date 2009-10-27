#include "browserwindow.h"
#include "ui_browserwindow.h"
#include "ServerQuery.h"
#include <QtGui/QMessageBox>

BrowserWindow::BrowserWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BrowserWindow)
{
    ui->setupUi(this);
    Query = new AsyncServerQuery();
    connect(Query, SIGNAL(ServerInfoReceived(SteamServerInfo)),
            this, SLOT(ServerInfoReceived(SteamServerInfo)));
}

BrowserWindow::~BrowserWindow()
{
    delete Query;
    delete ui;
}

void BrowserWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void BrowserWindow::TestClicked()
{
    Query->QueryServerList();
}

void BrowserWindow::ServerInfoReceived(SteamServerInfo Info)
{
    QMessageBox::information(this, "Received Server Info", Info.ConnectAddress);
}
