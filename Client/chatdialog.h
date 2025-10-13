#ifndef CHATDIALOG_H
#define CHATDIALOG_H
#include "global.h"
#include "statewidget.h"
#include "userdata.h"

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void addChatUserList();
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void handleGlobalMousePress(QMouseEvent* event);
private:
    void ShowSearch(bool bsearch = false);
    void AddLBGroup(StateWidget* lb);
    void ClearLabelState(StateWidget* lb);
    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;
    QList<StateWidget*> _lb_list;
    QWidget* _last_widget;
public slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString &str);
    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply);
};

#endif // CHATDIALOG_H
