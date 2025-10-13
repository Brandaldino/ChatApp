#ifndef FINDSUCESSDLG_H
#define FINDSUCESSDLG_H

#include "userdata.h"

#include <QDialog>
#include <memory>

namespace Ui {
class FindSucessDlg;
}

class FindSucessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSucessDlg(QWidget *parent = nullptr);
    ~FindSucessDlg();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);
private:
    Ui::FindSucessDlg *ui;
    std::shared_ptr<SearchInfo> _si;
    QWidget* _parent;

private slots:
    void on_add_friend_btn_clicked();
};

#endif // FINDSUCESSDLG_H
