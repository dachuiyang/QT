#include <QList>
#include <QDebug>
#include <QModelIndex>
#include "cmd_dlg.h"
#include "ps_m_wnd.h"
#include "ui_ps_m_wnd.h"
#include "settings.h"
#include "client_session.h"

#define PORT_NUM 9999
#define COL_NUM 7

PS_M_Wnd* wnd_this = 0;

enum Em_Host_Col
{
    COL_IP,
    //    COL_NAME,
    //    COL_OS,
    COL_VER,
    COL_CORE,
    COL_CAM,
    COL_SOCKET
};

//    {("计算机名"),      160 },
//    {("操作系统"),		128 },

//主机网格的数组
Col_Data host_data_ary[] =
{
	{ "IP",		    200 },
	{ "客户端版本",  80 },
	{ "CPU",			30 },
	{ "摄像头",      50 },
	{ "Socket",      50 }
};

PS_M_Wnd::PS_M_Wnd(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PS_M_Wnd)
{
    wnd_this = this;

    ui->setupUi(this);

    this->m_tv_model = 0;

    this->init_table();

    this->m_p_ps_sock = 0;
    this->m_p_ps_sock = new Ps_Socket(PORT_NUM);

    this->m_p_ps_sock->set_wnd_this(this);
    this->m_p_ps_sock->setP_thd_pool(
                const_cast<QThreadPool*>(QThreadPool::globalInstance()));

    this->m_p_ps_sock->start();
}

PS_M_Wnd::~PS_M_Wnd()
{
    delete ui;
}


//上线处理槽函数
void PS_M_Wnd::slot_online(Line_Data *p_data)
{
    QString str_ip = QString(p_data->sz_host_ip);
    //    QString str_name = QString(p_data->line_data.sz_host_name);
    //    QString str_os = QString
    QString str_ver = QString::number(p_data->c_host_ver,10);
    QString str_cpu =  QString::number(p_data->c_host_cpu, 10);

    QString str_cam;
    if(0 == p_data->c_is_camera)
    {
        str_cam = QString("无");
    }
    else
    {
        str_cam = QString("有");
    }

    QString str_Socket = QString::number(p_data->s_host_sock, 10);

    //各项插入list
    QList<QStandardItem*> row_lst;
    row_lst << new QStandardItem(str_ip)
            << new QStandardItem(str_ver)
            << new QStandardItem(str_cpu)
            << new QStandardItem(str_cam)
            << new QStandardItem(str_Socket);

    //添加新行
    this->m_tv_model->appendRow(row_lst);

    delete p_data;
}

//断线处理槽函数
void PS_M_Wnd::slot_disconn(Client_Session *p_cs)
{
    QModelIndex row_index;
    int n_socket = 0;

    int n_count = this->m_tv_model->rowCount();

    for(int n_index = 0; n_index < n_count; n_index++)
    {
         //遍历取出对应socket
         row_index = this->m_tv_model->index(n_index, COL_SOCKET);
         n_socket = this->m_tv_model->data(row_index).toInt();

         if(p_cs->m_socket == n_socket)
         {
             this->m_tv_model->removeRow(n_index);
             update();
         }
    }

    //停止接收 关闭socket移除数据
    this->m_p_ps_sock->cs_remove(p_cs->m_socket);
    //delete p_cs;
}

void PS_M_Wnd::slot_create_dlg(Client_Session *p_cs, int n_type)
{
    switch (n_type)
    {
    case PSC_CMD_REPLY:
    {
        if(0 == p_cs->m_cmd_dlg.data())
        {
            p_cs->m_cmd_dlg.reset(new Cmd_Dlg(p_cs->m_socket));
        }
        p_cs->m_cmd_dlg->show();

        //发送一个回复包 表示窗口已经就位 让被控端开启线程
        Data_Head data_head(PSC_CMD, PSC_CMD_REPLY);
        Net_Tools::send_buf(p_cs->m_socket,
                            (char*)&data_head,
                            sizeof(Data_Head));

    }
        break;

    case PSC_SCR_REPLY:
    {
        if(0 == p_cs->m_scr_dlg.data())
        {
            p_cs->m_scr_dlg.reset(new Scr_Dlg(p_cs->m_socket));
        }
        p_cs->m_scr_dlg->show();

        //发送一个回复包 表示窗口已经就位 可以发数据了
        Data_Head data_head(PSC_SCREEN, PSC_SCR_REPLY);
        Net_Tools::send_buf(p_cs->m_socket,
                            (char*)&data_head,
                            sizeof(Data_Head));
    }
        break;

    default:
        break;
    }
}

void PS_M_Wnd::slot_recv_data(Client_Session *p_cs,
                              int n_type,
                              QByteArray buf)
{
    switch (n_type)
    {
        case PSC_CMD_DATA:
        {
            p_cs->m_cmd_dlg->append_data(buf);
        }
        break;

        case PSC_SCR_DATA:
        {
            p_cs->m_scr_dlg->dps_data(buf);
        }
        break;

    default:
        break;
    }
}

//获取选中的行对应的socket
SOCKET PS_M_Wnd::get_sel_sock()
{
    //获取选中的行列表 获取第一个
    QModelIndexList row_lst =
            ui->tv_host->selectionModel()->selectedRows();
    int n_row = row_lst.first().row();
    
    qDebug() << ui->tv_host->currentIndex().row() << endl;
    qDebug() << n_row << endl;
    
    QModelIndex index = this->m_tv_model->index(n_row, COL_SOCKET);
    SOCKET h_socket = 0;
    h_socket = this->m_tv_model->data(index).toInt();
    
    return h_socket;
}

//初始化 tv_host控件
void PS_M_Wnd::init_table()
{
    //model赋值
    this->m_tv_model.reset(new QStandardItemModel());

    //设置不可编辑
    ui->tv_host->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //设置选中时为整行选中
    ui->tv_host->setSelectionBehavior(QAbstractItemView::SelectRows);

    //绑定model和view
    ui->tv_host->setModel(this->m_tv_model.data());

    QStringList head_list;

    //    head_list << host_data_ary[COL_IP].sz_title
    //              << host_data_ary[COL_NAME].sz_title
    //              << host_data_ary[COL_OS].sz_title
    //              << host_data_ary[COL_VER].sz_title
    //              << host_data_ary[COL_CORE].sz_title
    //              << host_data_ary[COL_CAM].sz_title;

    //    this->m_tv_model->setHorizontalHeaderLabels(head_list);

    for(int n_index = 0; n_index < COL_NUM; n_index++)
    {
        head_list << host_data_ary[n_index].sz_title;
        ui->tv_host->setColumnWidth(n_index,
                                    host_data_ary[n_index].n_width);
    }

    this->m_tv_model->setHorizontalHeaderLabels(head_list);
}

void PS_M_Wnd::on_act_cmd_triggered()
{
    if(0 == this->m_tv_model->rowCount())
    {
        return;
    }

    //获取选中行的socket 获取当前行cs
    SOCKET h_sock = this->get_sel_sock();
    Client_Session* p_cs =
            this->m_p_ps_sock->get_cs(h_sock);

    if(0 != p_cs->m_cmd_dlg)
    {
        p_cs->m_cmd_dlg->show();
        return;
    }

    //发送数据 让被控端初始化cmd
    Data_Head data_head(PSC_CMD, PSC_CMD_INIT);
    Net_Tools::send_buf(h_sock,
                        (char*)&data_head,
                        sizeof(Data_Head));
}

void PS_M_Wnd::on_act_scr_triggered()
{
    if(0 == this->m_tv_model->rowCount())
    {
        return;
    }

    //获取选中行的socket 获取当前行cs
    SOCKET h_sock = this->get_sel_sock();
    Client_Session* p_cs =
            this->m_p_ps_sock->get_cs(h_sock);

    if(0 != p_cs->m_scr_dlg)
    {
        p_cs->m_scr_dlg->show();
        return;
    }

    //发送数据 让被控端初始化cmd
    Data_Head data_head(PSC_SCREEN, PSC_SCR_INIT);
    Net_Tools::send_buf(h_sock,
                        (char*)&data_head,
                        sizeof(Data_Head));
}
