#ifndef PS_SOCKET_H
#define PS_SOCKET_H

#include <QThread>
#include <QThreadPool>
#include <QByteArray>
#include "heart_thd.h"

class PS_M_Wnd;

class Ps_Socket:public QThread
{
    Q_OBJECT

public:
    Ps_Socket(int n_port);
    ~Ps_Socket();

public:
    virtual void run();

public:
    int init();
    int begin_listen();
    int accept_func();

public:
    void ifs_accept(DWORD dw_index);
    int ifs_recv(DWORD dw_index);
    void ifs_close();

public:
    int send_online(SOCKET h_socket);

public:
    Client_Session *get_cs(SOCKET h_soeket);

public:
    void setB_stop_listen(bool value);
    void setP_thd_pool(QThreadPool* p_thd_pool);
    void set_wnd_this(PS_M_Wnd* wnd_this);

public:
    void cs_remove(SOCKET h_socket);

private:  //信号和主窗口槽函数
    void  conn_this();

public:
    bool check_data_head(char* p_buf);

public:
    int ifs_dps(Client_Session* p_cs);   //分发处理接口

    void ifs_heart(Client_Session* p_cs);   //心跳包的处理函数
    int ifs_online(Client_Session *p_cs);   //上线包的处理函数

    int ifs_cmd(Client_Session* p_cs);
    int ifs_scr(Client_Session *p_cs);

public:
    void setB_stop_recv(bool b_stop_recv);

signals:
    void sin_online(Line_Data* p_data);
    void sin_create_dlg(Client_Session* p_cs, int type);
    void sin_recv_data(Client_Session* p_cs, int type, QByteArray buf);


private:
    int m_n_port;          //监听的端口
    bool m_b_stop_listen;  //停止监听的哨兵值

private:
    SOCKET m_l_sock;       //监听的socket

private:
    int m_n_count;
    SOCKET* m_p_socket_ary;
    WSAEVENT* m_p_event_ary;

private:
    QSharedPointer<QByteArray> m_bt_ary;

private:
    QMap<SOCKET, Client_Session*>* m_client_map;

private:
    QThreadPool* m_p_thd_pool;
    Heart_thd* m_heart_thd;

private:
    PS_M_Wnd* m_wnd_this;
};

#endif // PS_SOCKET_H
