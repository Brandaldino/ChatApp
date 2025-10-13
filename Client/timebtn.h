#ifndef TIMEBTN_H
#define TIMEBTN_H

#include <QPushButton>
#include <QTimer>


class TimeBtn:public QPushButton
{
public:
    TimeBtn(QWidget* parent = nullptr);
    ~TimeBtn();
    void mouseReleaseEvent(QMouseEvent *e) override;
private:
    QTimer* _timer;
    int _counter;
};

#endif // TIMEBTN_H
