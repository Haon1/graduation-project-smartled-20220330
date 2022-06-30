#ifndef QBLEPAGE_H
#define QBLEPAGE_H

#include <QWidget>
#include <QSerialPort>
#include <QString>
#include <QTimer>

namespace Ui {
class QBlePage;
}

class QBlePage : public QWidget
{
    Q_OBJECT

public:
    explicit QBlePage(QWidget *parent = nullptr);
    ~QBlePage();

    void initSerial();
    void starTimer();   //开启定时器，3秒后下发ip

public slots:
    void slotPushData(QString data);
    void slotTimeOut();

private slots:
    void slotReadyRead();

    void on_bt_openSerial_clicked();

    void on_bt_cleanRecv_clicked();

    void on_bt_send_clicked();

private:
    Ui::QBlePage *ui;

    QSerialPort *serial;    //串口通信对象

    bool isConnectFlag;      //连接标志位
    QString msg;
    QTimer *timer;
};

#endif // QBLEPAGE_H
