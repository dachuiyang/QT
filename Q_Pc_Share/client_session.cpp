#include "client_session.h"

Client_Session::Client_Session()
{
    this->m_socket = 0;
    this->m_ser = {0};
    this->m_last_time = 0;

    this->m_cmd_dlg = 0;
    this->m_scr_dlg = 0;
    this->m_serv_dlg = 0;

    this->m_bt_buf.reset(new QByteArray);
}

Client_Session::Client_Session(SOCKET h_sock, sockaddr_in new_ser)
{
    this->m_socket = 0;
    this->m_ser = {0};
    this->m_ser = new_ser;
    this->m_socket = h_sock;

    this->m_last_time = 0;

    this->m_cmd_dlg = 0;
    this->m_scr_dlg = 0;
    this->m_serv_dlg = 0;

    this->m_bt_buf.reset(new QByteArray);
}

Client_Session::~Client_Session()
{
    if(0 != this->m_socket)
    {
        closesocket(this->m_socket);
    }

    this->m_socket = 0;
    this->m_ser = {0};

    if(0 != this->m_cmd_dlg)
    {
        this->m_cmd_dlg->close();
    }
    if(0 != this->m_scr_dlg)
    {
        this->m_scr_dlg->close();
    }
    if(0 != this->m_serv_dlg)
    {
        this->m_serv_dlg->close();
    }

    this->m_cmd_dlg.clear();
    this->m_scr_dlg.clear();
    this->m_serv_dlg.clear();

//    this->m_dps_thd->setB_stop_recv(true);
    //this->m_dps_thd.clear();

    this->m_last_time = 0;
}
