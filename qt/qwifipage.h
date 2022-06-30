#ifndef QWIFIPAGE_H
#define QWIFIPAGE_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QNetworkInterface>

namespace Ui {
class QWifiPage;
}

class QWifiPage : public QWidget
{
    Q_OBJECT

public:
    explicit QWifiPage(QWidget *parent = nullptr);
    ~QWifiPage();

    QHostAddress getHostIPV4Address();  //获取本机ipv4地址

signals:
    void commitData(int,int,int,int);      //下位机上报数据到WIFI，wifi界面发送这个信号给ctrl界面处理

public slots:
        void slotPushData(QString data);        //ctrl界面发送对应信号，由这个函数进行下发

private slots:
    void slotNewConnect();


    void slotReadyRead();


    void slotError(QAbstractSocket::SocketError error);


    void on_bt_listen_clicked();


    void on_bt_clean_recv_clicked();


    void on_bt_clean_send_clicked();


    void on_bt_send_clicked();



private:
    Ui::QWifiPage *ui;
    QTcpServer *Server;
    QTcpSocket *Client;
    QHostAddress ip;
    int port;


    bool isConnectFlag;

};

#endif // QWIFIPAGE_H
