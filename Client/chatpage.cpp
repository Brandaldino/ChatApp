#include "chatpage.h"
#include "ui_chatpage.h"
#include "chatitembase.h"
#include "textbubble.h"
#include "picturebubble.h"

#include <QStyleOption>
#include <QPainter>
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

ChatPage::ChatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    //设置按钮样式
    ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");

    //设置图标样式
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

void ChatPage::on_send_btn_clicked() // TODO:拖动图片无法复制进chatEdit，问题可能在MessageTextEdit类
{
    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    QString userName = QStringLiteral("TheFool");
    QString usericon = ":/src/picture/head_1.jpg";

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    for(int i = 0;i<msgList.size();++i){
        QString type = msgList[i].msgFlag;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(usericon));
        QWidget* pBubble = nullptr;
        if(type == "text"){
            pBubble = new TextBubble(role,msgList[i].content);
        }
        else if(type == "image"){
            pBubble = new PictureBubble(QPixmap(msgList[i].content),role);
        }
        else if(type == "file"){

        }
        if(pBubble != nullptr){
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }
}
