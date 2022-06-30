#include "qblepage.h"
#include "ui_qblepage.h"
#include <QSerialPortInfo>
#include <QString>
#include <QDebug>

extern QString g_ip;
extern int g_port;      //wifi界面中定义

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

QBlePage::QBlePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QBlePage)
{
    ui->setupUi(this);

    serial = new QSerialPort;   //创建端口对象

    //信号与槽函数关联
    connect(serial, &QSerialPort::readyRead, this, &QBlePage::slotReadyRead);

    //初始化串口设置选项
    initSerial();
    isConnectFlag = false;      //连接标志位为false
}

QBlePage::~QBlePage()
{
    delete ui;
}

/**
 * @brief 初始化串口设置选项
 */
void QBlePage::initSerial()
{
    QString description;
    QString manufacturer;
    QString serialNumber;

    //获取可以用的串口
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();

    //输出当前系统可以使用的串口个数
    qDebug() << "Total numbers of ports: " << serialPortInfos.count();


    //将所有可以使用的串口设备添加到ComboBox中
   for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
   {
       QStringList list;
       description = serialPortInfo.description();
       manufacturer = serialPortInfo.manufacturer();
       serialNumber = serialPortInfo.serialNumber();
       list << serialPortInfo.portName()
       << (!description.isEmpty() ? description : blankString)
       << (!manufacturer.isEmpty() ? manufacturer : blankString)
       << (!serialNumber.isEmpty() ? serialNumber : blankString)
       << serialPortInfo.systemLocation()
       << (serialPortInfo.vendorIdentifier() ? QString::number(serialPortInfo.vendorIdentifier(), 16) : blankString)
       << (serialPortInfo.productIdentifier() ? QString::number(serialPortInfo.productIdentifier(), 16) : blankString);
       ui->comboBox_serialPort->addItem(list.first(), list);
   }


   //设置波特率
   ui->comboBox_baudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
   ui->comboBox_baudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
   ui->comboBox_baudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
   ui->comboBox_baudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
   ui->comboBox_baudRate->setCurrentIndex(3);

    //设置数据位
    ui->comboBox_dataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->comboBox_dataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->comboBox_dataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->comboBox_dataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->comboBox_dataBits->setCurrentIndex(3);

    //设置奇偶校验位
    ui->comboBox_parity->addItem(tr("None"), QSerialPort::NoParity);
    ui->comboBox_parity->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->comboBox_parity->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->comboBox_parity->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->comboBox_parity->addItem(tr("Space"), QSerialPort::SpaceParity);

    //设置停止位
    ui->comboBox_stopBit->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    ui->comboBox_stopBit->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    //添加流控
    ui->comboBox_flowBit->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->comboBox_flowBit->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->comboBox_flowBit->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

/**
 * @brief 开启定时器,三秒后下发ip和端口
 */
void QBlePage::starTimer()
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    timer->start(3000);
}


void QBlePage::slotTimeOut()
{
    //  ip:192.168.xxx.xxx:port:8026#;
    QString ip_port_info= QString("%1%2%3%4%5").arg("ip:").arg(g_ip).arg(",port:").arg(QString::number(g_port)).arg("#");
    serial->write(ip_port_info.toUtf8().data());
    timer->stop();
}


/**
 * @brief 控制界面发来数据，要推送到设备
 * @param data 数据
 */
void QBlePage::slotPushData(QString data)
{
    qDebug() << "ble界面收到数据："<< data;
    if(isConnectFlag)
    {
        //serial->write(data.toUtf8().data());
    }
}



/**
 * @brief 点击打开串口
 */
void QBlePage::on_bt_openSerial_clicked()
{
    //qDebug() << ui->btn_openConsole->text();
    if (ui->bt_openSerial->text() == "打开串口")
     {
        //设置串口名字
        serial->setPortName(ui->comboBox_serialPort->currentText());
        //设置波特率
        serial->setBaudRate(ui->comboBox_baudRate->currentText().toInt());
        //设置数据位
        serial->setDataBits(QSerialPort::Data8);
        //设置奇偶校验位
        serial->setParity(QSerialPort::NoParity);
        //设置停止位
        serial->setStopBits(QSerialPort::OneStop);
        //设置流控
        serial->setFlowControl(QSerialPort::NoFlowControl);

        //打开串口
        if (serial->open(QIODevice::ReadWrite))
        {
            //取消设置功能
            ui->comboBox_baudRate->setEnabled(false);
            ui->comboBox_dataBits->setEnabled(false);
            ui->comboBox_flowBit->setEnabled(false);
            ui->comboBox_parity->setEnabled(false);
            ui->comboBox_serialPort->setEnabled(false);
            ui->comboBox_stopBit->setEnabled(false);

            ui->bt_openSerial->setText("关闭串口");

            isConnectFlag = true;
        }
    }
    else
    {
        //关闭串口
        //serial->clear();
        serial->close();
        //serial->deleteLater();
        //恢复设置功能
        ui->comboBox_baudRate->setEnabled(true);
        ui->comboBox_dataBits->setEnabled(true);
        ui->comboBox_flowBit->setEnabled(true);
        ui->comboBox_parity->setEnabled(true);
        ui->comboBox_serialPort->setEnabled(true);
        ui->comboBox_stopBit->setEnabled(true);
        ui->bt_openSerial->setText(tr("打开串口"));

        isConnectFlag = false;
    }
}

/**
 * @brief 清空接收区
 */
void QBlePage::on_bt_cleanRecv_clicked()
{
    ui->textBrowser->clear();
}

/**
 * @brief 发送消息
 */
void QBlePage::on_bt_send_clicked()
{
    QString msg = ui->textEdit->toPlainText();
    if(isConnectFlag)
        serial->write(msg.toUtf8().data());
}


/**
 * @brief 设备发来消息在这里处理
 */
void QBlePage::slotReadyRead()
{
    QString temp = serial->readAll();       //读取串口发来的数据

    if(!temp.isEmpty())
    {
        msg.append(temp);
        while(msg.contains("\r\n"))        //如果包含  "\r\n"说明数据帧接收完毕
        {   //123\r\n12
            QStringList list = msg.split("\r\n");   //以 "\r\n" 切割成两半 "123"  "12"
            ui->textBrowser->append(list.at(0));    //"123"

            msg = msg.right(msg.length()-msg.indexOf("\r\n")-2);


            //下发服务器ip和端口
            if(list.at(0).contains("get_server_infomation"))
            {
                starTimer();        //启动定时器，三秒后下发ip和端口
            }
        }
    }
}
