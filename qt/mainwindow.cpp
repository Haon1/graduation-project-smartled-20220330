#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //建立子窗体
    BlePage = new QBlePage(this);
    WifiPage = new QWifiPage(this);
    CtrlPage = new QCtrlPage(this);

    //把窗体添加到  stackedWidget中,用于切换面板
    ui->stackedWidget->addWidget(WifiPage);
    ui->stackedWidget->addWidget(BlePage);
    ui->stackedWidget->addWidget(CtrlPage);
    ui->stackedWidget->setCurrentWidget(WifiPage);

    firstIndex = ui->stackedWidget->currentIndex();
    //qDebug() << firstIndex;

    //绑定wifi界面与ctrl界面的信号与槽，建立通信方式   数据下发
    connect(CtrlPage,&QCtrlPage::pushData,WifiPage,&QWifiPage::slotPushData);

    //绑定wifi界面与ctrl界面的信号与槽，建立通信方式   接收数据上报
    connect(WifiPage,&QWifiPage::commitData,CtrlPage,&QCtrlPage::slotCommitData);

    //绑定蓝牙界面与ctrl界面的信号与槽，建立通信方式   数据下发
    //connect(CtrlPage,&QCtrlPage::pushData,BlePage,&QBlePage::slotPushData);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete BlePage;
    delete WifiPage;
    delete CtrlPage;
}

/**
 * @brief 点击wifi
 */
void MainWindow::on_bt_wifi_clicked()
{
    ui->stackedWidget->setCurrentIndex(firstIndex);
}

/**
 * @brief 点击蓝牙
 */
void MainWindow::on_bt_ble_clicked()
{
    ui->stackedWidget->setCurrentIndex(firstIndex+1);
}

/**
 * @brief 点击控制
 */
void MainWindow::on_bt_ctrl_clicked()
{
    ui->stackedWidget->setCurrentIndex(firstIndex+2);
}
