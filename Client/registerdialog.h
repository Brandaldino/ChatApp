#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_get_code_clicked();
    void slot_reg_mod_finish(ReqId req_id,QString res,ErrorCodes err);

    void on_sure_btn_clicked();
    void on_return_btn_clicked();

    void on_cancel_btn_clicked();

private:
    void initHttpHandles();
    void showTip(QString str, bool is_ok);
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
    void changeTipPage();
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVarifyValid();
    Ui::RegisterDialog *ui;
    QMap<ReqId,std::function<void(const QJsonObject &)>> _handlers;
    QMap<TipErr, QString> _tip_errs;

    QTimer* _countdown_timer;
    int _countdown;

 signals:
     void sigSwitchLogin();
};

#endif // REGISTERDIALOG_H
