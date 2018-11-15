#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <winsock2.h>
#include <QSharedPointer>
#include "scr_dlg.h"
#include "cmd_dlg.h"
#include "serv_dlg.h"
#include "dps_thd.h"

//客户端信息结构体
struct Client_Session
{
    Client_Session();
    Client_Session(SOCKET h_sock, sockaddr_in new_ser);
    ~Client_Session();

    //socket相关
    SOCKET m_socket;
    sockaddr_in m_ser;

    //最后一次的时间 心跳包用
    int m_last_time;

    //窗口相关
    QSharedPointer<Cmd_Dlg> m_cmd_dlg;   //cmd窗口
    QSharedPointer<Scr_Dlg> m_scr_dlg;   //屏幕窗口
    QSharedPointer<Serv_Dlg> m_serv_dlg; //服务窗口

    QSharedPointer<QByteArray> m_bt_buf;
};

#endif // CLIENT_SESSION_H
