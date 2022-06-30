#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qblepage.h"
#include "qwifipage.h"
#include "qctrlpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bt_wifi_clicked();

    void on_bt_ble_clicked();

    void on_bt_ctrl_clicked();

private:
    Ui::MainWindow *ui;
    QBlePage *BlePage;
    QWifiPage *WifiPage;
    QCtrlPage *CtrlPage;

    int firstIndex;
};
#endif // MAINWINDOW_H
