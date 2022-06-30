#include "qctrlpage.h"
#include "ui_qctrlpage.h"
#include <QDebug>
#include <QPixmap>
#include <QDebug>


char color_array[8][64]={"red","yellow","blue","cyan","green","white","purple","grey"};


QCtrlPage::QCtrlPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QCtrlPage)
{
    ui->setupUi(this);

    m_isLongPress = false;
    m_nowValue = 0;

    device = NONE;
}

QCtrlPage::~QCtrlPage()
{
    delete ui;
}

/**
 * @brief 设置ui实时状态
 * @param color
 */
void QCtrlPage::setLedState(QString color1,int level1,QString color2,int level2)
{
    QString url = QString("%1%2%3").arg(":/icon/").arg(color1).arg(".png");
    QPixmap pix(url);
    ui->lb_state_led1->setPixmap(pix);              //更新LED1颜色图标
    ui->lb_state_led1->setScaledContents(true);     //图标自适应大小
    ui->progressBar_led1->setValue(level1);         //更新LED1亮度值

    url = QString("%1%2%3").arg(":/icon/").arg(color2).arg(".png");
    pix.load(url);
    ui->lb_state_led2->setPixmap(pix);              //更新LED2颜色图标
    ui->lb_state_led2->setScaledContents(true);     //图标自适应大小
    ui->progressBar_led2->setValue(level2);         //更新LED2亮度值
}

/**
 * @brief 下位机上报数据之后在这里进行界面处理
 * @param data 数据
 */
void QCtrlPage::slotCommitData(int color1,int level1,int color2,int level2)
{
    qDebug() << "ctrl 界面收到上报" ;
    qDebug() << "color1:" << color1 <<"  level1:" << level1;
    qDebug() << "color2:" << color2 <<"  level2:" << level2;

    setLedState(color_array[color1],level1,color_array[color2],level2);        //更新UI
}



/*************   功能控制区  ***************/

//全开
void QCtrlPage::on_bt_allOpen_clicked()
{
    QString msg = "ctrl:1#";
    emit this->pushData(msg);       //发送信号,wifi界面接收信号
}

//全关
void QCtrlPage::on_bt_allClose_clicked()
{
    QString msg = "ctrl:2#";
    emit this->pushData(msg);   //发送信号,wifi界面接收信号
}

//开启自动调光
void QCtrlPage::on_bt_openAuto_clicked()
{
    QString msg = "ctrl:3#";
    emit this->pushData(msg);   //发送信号,wifi界面接收信号
}

//关闭自动调光
void QCtrlPage::on_bt_closeAuto_clicked()
{
    QString msg = "ctrl:4#";
    emit this->pushData(msg);   //发送信号,wifi界面接收信号
}

//开启手势调节
void QCtrlPage::on_bt_openGesture_clicked()
{
    QString msg = "ctrl:5#";
    emit this->pushData(msg);   //发送信号,wifi界面接收信号
}

//关闭手势调节
void QCtrlPage::on_bt_closeGesture_clicked()
{
    QString msg = "ctrl:6#";
    emit this->pushData(msg);   //发送信号,wifi界面接收信号
}


/*************   灯光控制区  ***************/

//红色
void QCtrlPage::on_bt_red_clicked()
{
    //color:0,3#   0是颜色,3是代表控制两个灯
    QString msg =QString("%1%2%3%4").arg("color:0").arg(",").arg(QString::number(device)).arg("#");
    if(device!=NONE)
        emit this->pushData(msg);   //发送信号,wifi界面接收信号
}

//黄色
void QCtrlPage::on_bt_yellow_clicked()
{
    QString msg =QString("%1%2%3%4").arg("color:1").arg(",").arg(QString::number(device)).arg("#");
    if(device!=NONE)
        emit this->pushData(msg);
}

//蓝色
void QCtrlPage::on_bt_blue_clicked()
{
    QString msg =QString("%1%2%3%4").arg("color:2").arg(",").arg(QString::number(device)).arg("#");
    if(device!=NONE)
        emit this->pushData(msg);
}

//浅蓝色
void QCtrlPage::on_bt_cyan_clicked()
{
    QString msg =QString("%1%2%3%4").arg("color:3").arg(",").arg(QString::number(device)).arg("#");
    if(device!=NONE)
        emit this->pushData(msg);
}

//绿色
void QCtrlPage::on_bt_green_clicked()
{
    QString msg =QString("%1%2%3%4").arg("color:4").arg(",").arg(QString::number(device)).arg("#");
    if(device!=NONE)
        emit this->pushData(msg);
}

//白色
void QCtrlPage::on_bt_white_clicked()
{
    QString msg =QString("%1%2%3%4").arg("color:5").arg(",").arg(QString::number(device)).arg("#");
    if(device!=NONE)
        emit this->pushData(msg);
}

//紫色
void QCtrlPage::on_bt_purple_clicked()
{
    QString msg =QString("%1%2%3%4").arg("color:6").arg(",").arg(QString::number(device)).arg("#");
    if(device!=NONE)
        emit this->pushData(msg);
}



/**
 * @brief 滑动条按下事件
 */
void QCtrlPage::on_horizontalSlider_sliderPressed()
{
    m_isLongPress = true;
}

/**
 * @brief 滑动条释放事件
 */
void QCtrlPage::on_horizontalSlider_sliderReleased()
{
    m_nowValue = ui->horizontalSlider->value();
    qDebug() << "release," << m_nowValue;

    //level:25,1#   25代表亮度，1代表控制LED1
    QString msg = QString("%1%2%3%4%5").arg("level:").arg(QString::number(m_nowValue)) \
                                    .arg(",").arg(QString::number(device)).arg("#");
    if(device!=NONE)
        emit this->pushData(msg);

    m_isLongPress = false;
}

/**
 * @brief 滑动条值改变事件
 * @param value 滑动条当前值
 */
void QCtrlPage::on_horizontalSlider_valueChanged(int value)
{
    m_nowValue = value;
    ui->lb_value2->setNum(value);

    if(m_isLongPress)
        return ;

    //qDebug() << "change," << m_nowValue;

    if(device!=NONE)
    {
        QString msg = QString("%1%2%3%4%5").arg("level:").arg(QString::number(m_nowValue)) \
                                        .arg(",").arg(QString::number(device)).arg("#");
        emit this->pushData(msg);
    }

}

/**
 * @brief 选中LED1框发生改变
 * @param arg1 当前状态值
 */
void QCtrlPage::on_checkBox_1_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked) //选中
    {
        device = LED1;
        if(ui->checkBox_2->isChecked())
            device = ALL;
    }
    else    //Qt::Unchecked
    {
        device = LED2;
        if(!ui->checkBox_2->isChecked())
            device = NONE;
    }
    //qDebug() << device;
}

/**
 * @brief 选中LED2框发生改变
 * @param arg1 当前状态值
 */
void QCtrlPage::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked) //选中
    {
        device = LED2;
        if(ui->checkBox_1->isChecked())
            device = ALL;
    }
    else    // Qt::Unchecked
    {
        device = LED1;
        if(!ui->checkBox_1->isChecked())
            device = NONE;
    }
    //qDebug() << device;
}
