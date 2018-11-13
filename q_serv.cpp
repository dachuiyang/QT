#include "q_serv.h"
#include <QDebug>
#include <QList>

enum Em_Serv_Head
{
    SERV_ID,
    SERV_NAME,
    SERV_DISPLAY,
    SERV_STATUS
};

enum Em_Serv_Status
{
    SERV_STOP,
    SERV_STARTING,
    SERV_STOPING,
    SERV_RUNING,
    SERV_CONTINUE,
    SERV_PAUSING,
    SERV_PAUSE
};

char* sz_head_ary[] =
{
    ("服务标识符"),
    ("服务名"),
    ("显示名称"),
    ("服务状态")
};

char* sz_status_ary[] =
{
    ("服务已停止"),
    ("服务正在启动"),
    ("服务正在停止"),
    ("服务正在运行"),
    ("服务即将继续"),
    ("服务暂停"),
    ("服务已暂停"),
};

Q_Serv::Q_Serv()
{
     this->init();
}

//返回所需缓冲区大小
int Q_Serv::get_need_size()
{
    DWORD dw_buf_size = 0;     // 传入的缓冲长度


    // 建立了一个到服务控制管理器的连接，并打开指定的数据库
    m_h_scm = OpenSCManager(NULL,  // 目标计算机名 null则为本地
                            NULL,  //打开SERVICES_ACTIVE_DATABASE
                            SC_MANAGER_ENUMERATE_SERVICE //权限
                            | SC_MANAGER_CONNECT);

    //如果打开出错
    if(0 == m_h_scm)
    {
        return -1;
    }

    // 获取需要的缓冲区大小
    EnumServicesStatusEx(m_h_scm,
                         SC_ENUM_PROCESS_INFO,
                         SERVICE_WIN32,
                         SERVICE_STATE_ALL,
                         NULL,
                         dw_buf_size,
                         &m_dw_buf_need,
                         &m_dw_ret_serv,
                         NULL,
                         NULL);

    return m_dw_buf_need;
}

int Q_Serv::get_serv_buf()
{
    bool b_ret = false;
    DWORD dw_buf_size = 0;  // 传入的缓冲长度

    //多存放1个服务信息的长度
    dw_buf_size = m_dw_buf_need
                + sizeof(ENUM_SERVICE_STATUS_PROCESS);

    m_p_buf = new char[dw_buf_size];
    memset(m_p_buf, 0, dw_buf_size);

    // 获取服务信息
    b_ret = EnumServicesStatusEx(m_h_scm,
                                 SC_ENUM_PROCESS_INFO,
                                 SERVICE_WIN32,
                                 SERVICE_STATE_ALL,
                                 (LPBYTE)m_p_buf,
                                 dw_buf_size,
                                 &m_dw_buf_need,
                                 &m_dw_ret_serv,
                                 NULL,
                                 NULL);

    //如果获取失败
    if(FALSE == b_ret)
    {
        ::CloseServiceHandle(m_h_scm);
        delete m_p_buf;
        return -1;
    }

    // 关闭打开的服务句柄
    b_ret = ::CloseServiceHandle(m_h_scm);
    if(b_ret == FALSE)  //如果关闭失败
    {
        return -2;
    }

    return 0;
}

int Q_Serv::list_tra()
{
    this->get_need_size();
    this->get_serv_buf();

    //初始化服务结构体指针
    LPENUM_SERVICE_STATUS_PROCESS p_serv_info = 0;

    //指向获取的服务缓冲区 (存储服务信息结构体)
    p_serv_info = (LPENUM_SERVICE_STATUS_PROCESS)m_p_buf;

    for(int n_index = 0; n_index < m_dw_ret_serv; n_index++)
    {
        QList<QStandardItem*> row_item;

        //进程标识符
        int n_id = (int)(p_serv_info->ServiceStatusProcess.dwProcessId);
        QStandardItem* p_id_item = 0;
        p_id_item = new QStandardItem(QString::number(n_id));

        //服务名
        QStandardItem* p_name_item = 0;
        QString name = QString::fromStdWString(p_serv_info->lpServiceName);
        p_name_item = new QStandardItem(name);

        //显示名称
        QString dis_name = QString::fromStdWString(p_serv_info->lpDisplayName);
        QStandardItem* p_dis_item = 0;
        p_dis_item = new QStandardItem(dis_name);

        //状态
        DWORD dw_status = p_serv_info->ServiceStatusProcess.dwCurrentState;
        QString status_str = m_p_status_lst->at(dw_status - 1);
        QStandardItem* p_status_item = 0;
        p_status_item = new QStandardItem(status_str);

        row_item << p_id_item      //标识符
                 << p_name_item    //服务名
                 << p_dis_item     //显示名称
                 << p_status_item; //状态

        //添加行数据
        this->m_model_serv->appendRow(row_item);
        p_serv_info++;
    }

    return 0;
}


void Q_Serv::init()
{
    m_tv_serv = 0;
    m_model_serv = 0;

    m_h_scm = 0;
    m_dw_buf_need = 0;      // 需要的缓冲长度
    m_dw_ret_serv = 0;      // 返回的服务个数

    m_p_head_lst = 0;
    m_p_status_lst = 0;

    m_p_status_lst = new QStringList();
    m_p_head_lst = new QStringList();

    //控件标题列表
    *m_p_head_lst << sz_head_ary[SERV_ID]
                  << sz_head_ary[SERV_NAME]
                  << sz_head_ary[SERV_DISPLAY]
                  << sz_head_ary[SERV_STATUS];

    //服务状态列表
    *m_p_status_lst << sz_status_ary[SERV_STOP]
                    << sz_status_ary[SERV_STARTING]
                    << sz_status_ary[SERV_STOPING]
                    << sz_status_ary[SERV_RUNING]
                    << sz_status_ary[SERV_CONTINUE]
                    << sz_status_ary[SERV_PAUSING]
                    << sz_status_ary[SERV_PAUSE];

}

void Q_Serv::init_list(QTableView *tv_serv)
{
    if(0 == tv_serv)
    {
        return;
    }

    m_tv_serv = tv_serv;

    m_model_serv = new QStandardItemModel();

    m_model_serv->setColumnCount(m_p_head_lst->count());
    m_model_serv->setHorizontalHeaderLabels(*m_p_head_lst);

    tv_serv->setModel(m_model_serv);
    tv_serv->setColumnWidth(0, 100);
    tv_serv->setColumnWidth(1, 180);
    tv_serv->setColumnWidth(2, 320);
    tv_serv->setColumnWidth(3, 200);
}
