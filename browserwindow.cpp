#include "browserwindow.h"
#include "ui_browserwindow.h"
#include "ServerQuery.h"

BrowserWindow::BrowserWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BrowserWindow)
{
    ui->setupUi(this);
}

BrowserWindow::~BrowserWindow()
{
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
    TestServerQuery();
}
