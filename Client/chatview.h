#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QTime>

class ChatView:public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget* parent = nullptr);
    void appendChatItem(QWidget* item); // 尾插
    void prependChatItem(QWidget* item); // 头插
    void insertChatItem(QWidget *before, QWidget *item); // 任意位置插入
    void removeAllItem();
protected:
    bool eventFilter(QObject* o,QEvent *e) override;
    void paintEvent(QPaintEvent* event) override;
private slots:
    void onVScrollBarMoved(int min, int max);
private:
    void initStyleSheet();
    QVBoxLayout* m_pVl;
    QScrollArea* m_pScrollArea;
    bool isAppended;
};

#endif // CHATVIEW_H
