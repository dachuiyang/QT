#ifndef Q_SERVICES_H
#define Q_SERVICES_H

#include <QTableView>
#include <QStandardItemModel>
#include <windows.h>

class Q_Serv
{
public:
    Q_Serv(); 

public:
    void init_list(QTableView *tv_serv);
    int get_need_size();
    int get_serv_buf();

public:
    int list_tra();

private:
    void init();

private:
    QTableView* m_tv_serv;
    QStandardItemModel* m_model_serv;

private:
    SC_HANDLE m_h_scm;     //连接控制管理器的句柄
    DWORD m_dw_buf_need;   //存储服务所需缓冲区大小
    DWORD m_dw_ret_serv;   //服务个数
    char* m_p_buf;         //存储服务数组缓冲区

    QStringList* m_p_head_lst;
    QStringList* m_p_status_lst;
};

#endif // Q_SERVICES_H
