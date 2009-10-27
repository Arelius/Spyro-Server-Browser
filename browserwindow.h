#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>

namespace Ui {
    class BrowserWindow;
}

class BrowserWindow : public QMainWindow {
    Q_OBJECT
private:
    class AsyncServerQuery* Query;
public:
    BrowserWindow(QWidget *parent = 0);
    ~BrowserWindow();

protected:
    void changeEvent(QEvent *e);
public slots:
    void TestClicked();
    void ServerInfoReceived(struct SteamServerInfo Info);

private:
    Ui::BrowserWindow *ui;

};

#endif // BROWSERWINDOW_H
