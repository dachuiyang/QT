#ifndef PR_M_WND_H
#define PR_M_WND_H

#include <QMainWindow>

#include "q_task.h"
#include "q_cmd.h"
#include "q_reg.h"
#include "q_serv.h"

namespace Ui
{
    class Pr_M_Wnd;
}

class Pr_M_Wnd : public QMainWindow
{
    Q_OBJECT

public:
    explicit Pr_M_Wnd(QWidget *parent = 0);
    ~Pr_M_Wnd();

private slots:
    void set_cmd_txt(QString str);

    void on_btn_cmd_clicked();

    void on_tr_reg_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_tr_reg_itemExpanded(QTreeWidgetItem *item);

    void on_btn_task_clicked();

    void on_btn_reg_clicked();

private:
    Ui::Pr_M_Wnd *ui;

private:
    Q_Task* m_p_task;
    Q_Cmd* m_p_cmd;
    Q_Serv* m_p_serv;
    Q_Reg* m_p_reg;
};

#endif // PR_M_WND_H
