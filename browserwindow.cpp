#include "browserwindow.h"
#include "ui_browserwindow.h"
#include "ServerQuery.h"
#include "ServerTableModel.h"
#include <QtGui/QMessageBox>

BrowserWindow::BrowserWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BrowserWindow)
{
    ui->setupUi(this);
    Query = new AsyncServerQuery();

    ServerModel = new ServerTableModel();
    ui->serverView->setModel(ServerModel);

    connect(Query, SIGNAL(ServerInfoReceived(SteamServerInfo)),
            ServerModel, SLOT(AddServerInfo(SteamServerInfo)));
}

BrowserWindow::~BrowserWindow()
{
    delete ServerModel;
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

void BrowserWindow::RefreshClicked()
{
    Query->QueryServerList();
}
