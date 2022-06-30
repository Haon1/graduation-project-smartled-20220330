#include "qwifipage.h"
#include "ui_qwifipage.h"
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QAbstractSocket>


#include <QDebug>

QString g_ip;
int g_port;


QWifiPage::QWifiPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QWifiPage)
{
    ui->setupUi(this);

    //创建TCP通信句柄
    Server = new QTcpServer(this);
    //绑定信号与槽
    connect(Server,SIGNAL(newConnection()),this,SLOT(slotNewConnect()));

    ip = getHostIPV4Address();
    port = 8026;
    g_ip = ip.toString();
    g_port = port;


    ui->le_ip->setText(ip.toString());
    ui->le_port->setText(QString::number(port));
    isConnectFlag = false;      //连接标志位为false
}

QWifiPage::~QWifiPage()
{
    delete ui;
    delete Server;
}



/**
 * @brief 连接设备成功后，会跳转到此
 */
void QWifiPage::slotNewConnect()
{
    isConnectFlag = true;
    //弹小窗提示
    //QMessageBox::warning(this,tr("结果提示"),tr("设备连接成功"),QMessageBox::Ok);
    //连接
    Client = Server->nextPendingConnection();
    //绑定信号与槽，链接之后会接收消息，
    connect(Client,SIGNAL(readyRead()),this,SLOT(slotReadyRead()));     //有新消息

    /*  检测设备是否断开*/
    connect(Client,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&QWifiPage::slotError);
}

/**
 * @brief 当设备有消息发来，会跳转到这里
 */
void QWifiPage::slotReadyRead()
{
    QString msg = Client->readAll();

    ui->textBrowser->append(msg);

    //上报数据帧  5,100,3,50#
    if(msg.endsWith("#"))   //判断是否以 #结尾
    {
        QStringList list = msg.split(",");  //切割成  5 100#
        list[3].chop(1);        //100#  从后面删掉一个字符 变成 100

        //处理完上报消息之后， 发送信号给 ctrl界面 处理
        emit this->commitData(list[0].toUInt(),list[1].toUInt(),list[2].toUInt(),list[3].toUInt());
    }
}

/**
 * @brief 设备连接发生错误跳转到这
 */
void QWifiPage::slotError(QAbstractSocket::SocketError error)
{
    //断开连接
    if(error==QAbstractSocket::RemoteHostClosedError)
    {
        qDebug() << error;
        isConnectFlag = false;
        //QMessageBox::warning(this,tr("结果提示"),tr("设备断开！"),QMessageBox::Ok);
    }

}


/**
 * @brief 获取本机Ipv4地址，如果有多个，则返回第一个有效地址，如果没有则返回环回地址
 * @return Ipv4地址
 */
QHostAddress QWifiPage::getHostIPV4Address()
{
    foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
            if ( hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address() )
                return hostAddress;

    return QHostAddress::LocalHost;
}


/**
 * @brief Wifi界面点击开始监听后,会跳转到此函数
 */
void QWifiPage::on_bt_listen_clicked()
{
    //如果按钮上的字是 开始监听，那就开启监听
    if(ui->bt_listen->text()=="开始监听")
    {
        Server->listen(ip,port);
        if(Server->isListening())
        {
            QMessageBox::warning(this,tr("结果提示"),tr("绑定成功，正在监听"),QMessageBox::Ok);
            //监听成功后，把按钮文字改成停止监听
            ui->bt_listen->setText("停止监听");
        }
        else
        {
            QMessageBox::warning(this,tr("结果提示"),tr("绑定失败！"),QMessageBox::Ok);
        }
    }
    else //停止监听
    {
        Server->close();
        ui->bt_listen->setText("开始监听");
    }

}

/**
 * @brief 清空接收区
 */
void QWifiPage::on_bt_clean_recv_clicked()
{
    ui->textBrowser->clear();
}

/**
 * @brief 清空发送区
 */
void QWifiPage::on_bt_clean_send_clicked()
{
    ui->textEdit->clear();
}

/**
 * @brief 推送消息给设备
 */
void QWifiPage::on_bt_send_clicked()
{
    QString msg = ui->textEdit->toPlainText();
    if(isConnectFlag)
    {
        if(msg.length() !=0 )
            Client->write(msg.toUtf8().data());
    }
    else
    {
        QMessageBox::warning(this,tr("结果提示"),tr("未建立通信，不能进行此操作！"),QMessageBox::Ok);
    }

}

/**
 * @brief 控制界面发来数据，要推送到设备
 * @param data 数据
 */
void QWifiPage::slotPushData(QString data)
{
    qDebug() << "wifi界面收到数据："<< data;
    if(isConnectFlag)
    {
        Client->write(data.toUtf8().data());
    }
}
