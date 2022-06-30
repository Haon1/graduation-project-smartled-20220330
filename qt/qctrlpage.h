#ifndef QCTRLPAGE_H
#define QCTRLPAGE_H

#include <QWidget>
#include <QString>

#define NONE    0
#define LED1    1
#define LED2    2
#define ALL     3

namespace Ui {
class QCtrlPage;
}

class QCtrlPage : public QWidget
{
    Q_OBJECT

public:
    explicit QCtrlPage(QWidget *parent = nullptr);
    ~QCtrlPage();
    void setLedState(QString color1,int level1,QString color2,int level2);

signals:
    void pushData(QString data);

public slots:
    void slotCommitData(int color1,int level1, int color2, int level2);

private slots:
    void on_horizontalSlider_sliderPressed();

    void on_horizontalSlider_sliderReleased();

    void on_horizontalSlider_valueChanged(int value);

    void on_bt_allOpen_clicked();

    void on_bt_allClose_clicked();

    void on_bt_openAuto_clicked();

    void on_bt_closeAuto_clicked();

    void on_bt_openGesture_clicked();

    void on_bt_closeGesture_clicked();

    void on_bt_red_clicked();

    void on_bt_yellow_clicked();

    void on_bt_blue_clicked();

    void on_bt_green_clicked();

    void on_bt_white_clicked();

    void on_bt_cyan_clicked();

    void on_bt_purple_clicked();

    void on_checkBox_1_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

private:
    Ui::QCtrlPage *ui;

    int m_nowValue;

    bool m_isLongPress;     //是否长按进度条

    int device;         //要下发的设备号


};

#endif // QCTRLPAGE_H
