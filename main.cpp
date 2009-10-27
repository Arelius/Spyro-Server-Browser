#include <QtGui/QApplication>
#include "browserwindow.h"
#include "ServerQuery.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BrowserWindow w;
    qRegisterMetaType<SteamServerInfo>("SteamServerInfo");        
    w.show();
    return a.exec();
}
