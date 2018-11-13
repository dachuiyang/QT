#include <QDebug>
#include "pr_m_wnd.h"
#include "ui_pr_m_wnd.h"

Pr_M_Wnd::Pr_M_Wnd(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Pr_M_Wnd)
{
    ui->setupUi(this);

    this->m_p_task = 0;
    this->m_p_task = new Q_Task();
    this->m_p_task->init_proc_table(ui->tw_proc);


    this->m_p_cmd = 0;
    this->m_p_cmd = new Q_Cmd();

    //可读数据连接界面的 cmd 输出窗口
    connect(this->m_p_cmd, &Q_Cmd::cmd_out,
            this, &Pr_M_Wnd::set_cmd_txt);

    //开一条线程来循环读取
    this->m_p_cmd->start();

    this->m_p_serv = new Q_Serv();
    this->m_p_serv->init_list(ui->tv_serv);
    this->m_p_serv->list_tra();

    this->m_p_reg = 0;
    this->m_p_reg = new Q_Reg();
    this->m_p_reg->init_tv_reg(ui->tv_reg);
    this->m_p_reg->init_tw_reg(ui->tr_reg);
}

Pr_M_Wnd::~Pr_M_Wnd()
{
    delete ui;
}

void Pr_M_Wnd::set_cmd_txt(QString str)
{
    ui->te_cmd->append(str);
}

void Pr_M_Wnd::on_btn_cmd_clicked()
{
    //清空内容
    ui->te_cmd->clear();

    QString str_exec = ui->le_cmd->text();
    str_exec.append('\n');

    //写入数据
    this->m_p_cmd->write_cmd_msg(str_exec);
}

void Pr_M_Wnd::on_tr_reg_itemDoubleClicked(QTreeWidgetItem *item,
                                           int column)
{
//    //获取子项数目
//    n_count = Q_Reg::get_group_count(item);

//    //如果已经遍历过一级
//    if(n_count > 0 && item->childCount() > 0)
//    {

//    }

    Q_Reg::sec_tra(item, column);

    //先清空再遍历
    ui->tv_reg->clearSpans();
    this->m_p_reg->tra_reg_key(item, column);
}

void Pr_M_Wnd::on_tr_reg_itemExpanded(QTreeWidgetItem *item)
{
    //二级遍历
    Q_Reg::sec_tra(item, 0);

}

void Pr_M_Wnd::on_btn_task_clicked()
{
    this->m_p_task->task_List(this->m_p_task->read_proc_info,
                              this->m_p_task);
}

void Pr_M_Wnd::on_btn_reg_clicked()
{
    QString cur_path = ui->le_reg->text();

    this->m_p_reg->tra_reg(cur_path);

}
