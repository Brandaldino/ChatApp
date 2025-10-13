#include "timebtn.h"
#include <QMouseEvent>
#include <QDebug>


TimeBtn::TimeBtn(QWidget *parent):QPushButton(parent),_counter(10)
{
    _timer = new QTimer(this);

    connect(_timer,&QTimer::timeout,[this](){
        _counter--;
       if(_counter<=0){
           _timer->stop();
           _counter = 10;
           this->setText("获取");
           this->setEnabled(true);
           return;
       }
       this->setText(QString::number(_counter));
    });


}

TimeBtn::~TimeBtn()
{
    _timer->stop();
}

void TimeBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        // 左键释放逻辑
        qDebug()<<"MyButton was released.";
        this->setEnabled(false);
        this->setText(QString::number(_counter));
        _timer->start(1000);
        emit clicked(); // 告诉外界，我已按下并松开
    }
    // 调用基类的mouseReleaseEvent以确保正常的事件处理（如点击效果）
    QPushButton::mouseReleaseEvent(e);
}
