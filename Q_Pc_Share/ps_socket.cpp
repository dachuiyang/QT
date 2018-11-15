#include <QDebug>
//#include "ps_socket.h"
//#include "settings.h"
#include "ps_m_wnd.h"

#define SOCK_VER 2
#define BUF_SIZE (8 * 1024)

Ps_Socket::Ps_Socket(int n_port)
{
    this->m_n_port = n_port;
    this->m_l_sock = 0;

    this->m_p_thd_pool = 0;
    this->m_b_stop_listen = false;

    this->m_client_map = new QMap<SOCKET, Client_Session*>;

    //初始化事件选择模型三件套
    this->m_n_count = 0;
    this->m_p_event_ary = 0;
    this->m_p_socket_ary = 0;

    this->m_p_event_ary = new WSAEVENT[WSA_MAXIMUM_WAIT_EVENTS]();
    this->m_p_socket_ary = new SOCKET[WSA_MAXIMUM_WAIT_EVENTS]();

    this->m_bt_ary.reset(new QByteArray);
    this->m_bt_ary->resize(BUF_SIZE);

    //初始化 开始监听
    this->init();
    this->begin_listen();
}

Ps_Socket::~Ps_Socket()
{

}

//单独开线程来接收新连接
void Ps_Socket::run()
{
    if(0 == this->m_p_thd_pool)
    {
        return;
    }

    this->m_p_thd_pool->start(this->m_heart_thd);
    this->accept_func();
}

//初始化建立socket
int Ps_Socket::init()
{
    //---------------初始化环境--------------------------
    int n_ret = 0;
    WSADATA wd = { 0 };

    n_ret = WSAStartup(MAKEWORD(SOCK_VER, 0), &wd);

    //初始化出错
    if (0 != n_ret)
    {
        return -1;
    }

    //当前版本不符合
    if (SOCK_VER != LOBYTE(wd.wVersion))
    {
        return -2;
    }

    //--------------建立socket-------------------
    SOCKET new_socket = 0;
    new_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //创建socket 失败 我的哥
    if (INVALID_SOCKET == new_socket)
    {
        new_socket = 0;
        WSACleanup();
        return -3;
    }

    this->m_l_sock = new_socket;

    return 0;
}

//绑定指定端口并监听
int Ps_Socket::begin_listen()
{
    int n_ret = 0;
    //--------------------bind---------------------------
    //建立结构体接收返回值
    sockaddr_in ser_addr = { 0 };

    ser_addr.sin_family = AF_INET;        //INTRL协议
    ser_addr.sin_port = htons(m_n_port);  //本机到网络转换

    //任意主机都可以连接
    ser_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    n_ret = bind(this->m_l_sock,                //socket
                 (SOCKADDR*)&ser_addr,   //指定端口
                 sizeof(sockaddr_in));   //结构体大小

    //如果绑定失败
    if(SOCKET_ERROR == n_ret)
    {
        goto SOCK_ERR;
    }

    //--------------------listen------------
    n_ret = listen(this->m_l_sock, SOMAXCONN);

    //监听socket加入socket数组
    this->m_p_socket_ary[m_n_count] = this->m_l_sock;
    this->m_p_event_ary[m_n_count] = WSACreateEvent();

    //关联网络事件  当前的监听socket 处理 新连接 和 关闭
    n_ret = WSAEventSelect(this->m_p_socket_ary[m_n_count],
                           this->m_p_event_ary[m_n_count],
                           FD_ACCEPT | FD_CLOSE);

    this->m_n_count++;

    //监听失败
    if (SOCKET_ERROR == n_ret)
    {
        goto SOCK_ERR;
    }

    return 0;

SOCK_ERR:
    if (INVALID_SOCKET != this->m_l_sock)
    {
        closesocket(this->m_l_sock);
    }
    WSACleanup();
    return -3;
}

void Ps_Socket::setB_stop_listen(bool value)
{
    m_b_stop_listen = value;
}

void Ps_Socket::setP_thd_pool(QThreadPool* p_thd_pool)
{
    m_p_thd_pool = p_thd_pool;
}

void Ps_Socket::set_wnd_this(PS_M_Wnd *wnd_this)
{
    this->m_wnd_this = wnd_this;

    //写在这里简直太烂了  以后再改吧

    //开启检测心跳包的线程
    this->m_heart_thd = 0;
    this->m_heart_thd = new Heart_thd(this->m_client_map);

    //绑定心跳包和主窗口的槽函数
    connect(this->m_heart_thd, &Heart_thd::sin_dis_conn,
            this->m_wnd_this, &PS_M_Wnd::slot_disconn);

    //连接信号槽
    this->conn_this();
}

void Ps_Socket::cs_remove(SOCKET h_socket)
{
    this->m_client_map->remove(h_socket);
}

//接收函数  轮循接收是否有新的socket 如果有 调用接收 success:0  err:-1
int Ps_Socket::accept_func()
{
    DWORD dw_index = 0;

    while (1)
    {
        //如果停止监听哨兵值为真
        if(m_b_stop_listen)
        {
            return 0;
        }

        //arg1 事件个数 2事件数组地址 3是否等待所有 4超时时间 5用不到
        dw_index = WSAWaitForMultipleEvents(this->m_n_count,
                                            this->m_p_event_ary,
                                            FALSE,
                                            100,
                                            FALSE);

        //如果返回值不是time_out 表示有事件发生
        if (dw_index == WSA_WAIT_TIMEOUT)
        {
            continue;
        }

        //arg1 产生事件的socket arg2:产生事件的事件对象 从已传信设置成 未传信
        //arg3:出错的事件和错误码
        WSANETWORKEVENTS wsa_net_event;
        WSAEnumNetworkEvents(this->m_p_socket_ary[dw_index - WSA_WAIT_EVENT_0],
                this->m_p_event_ary[dw_index - WSA_WAIT_EVENT_0],
                &wsa_net_event);

        //处理新连接
        if (wsa_net_event.lNetworkEvents & FD_ACCEPT)
        {
            if (wsa_net_event.iErrorCode[FD_ACCEPT_BIT] != 0)
            {
                continue; //接收出错开始下一次循环
            }

            this->ifs_accept(dw_index);
        }

        //处理新数据
        else if (wsa_net_event.lNetworkEvents & FD_READ)
        {
            //recv 错误的处理
            if (wsa_net_event.iErrorCode[FD_READ_BIT] != 0)
            {
                continue;
            }

            this->ifs_recv(dw_index);
        }

        // 处理关闭
        else if (wsa_net_event.lNetworkEvents & FD_CLOSE)
        {
            if (wsa_net_event.iErrorCode[FD_CLOSE_BIT] != 0)
            {
                continue;     //关闭出错处理
            }

            closesocket(this->m_p_socket_ary[dw_index-WSA_WAIT_EVENT_0]);
        }

        this->m_bt_ary->clear();
        this->m_bt_ary->resize(BUF_SIZE);
    }

}

//接受新连接  顺便发一个上线包
void Ps_Socket::ifs_accept(DWORD dw_index)
{
    //建立用来接收新连接的两个结构体
    SOCKET new_socket = INVALID_SOCKET;
    sockaddr_in new_ser = { 0 };

    int n_addr_len = sizeof(sockaddr_in);


    //接受新连接
    new_socket = accept(this->m_p_socket_ary[dw_index-WSA_WAIT_EVENT_0],
                       (sockaddr*)&new_ser,
                       &n_addr_len);

    //如果接收出错
    if (INVALID_SOCKET == new_socket)
    {
        qDebug() << "accept is error" ;
        return;
    }

    //如果是重复的socket continue
    if (this->m_client_map->contains(new_socket))
    {
        closesocket(new_socket);
        qDebug() << "socket is existed\n";
        return;
    }

    qDebug() << ("has a new connection\n");

    //创建新的客户端会话结构体
    Client_Session* p_client_seeion = 0;
    p_client_seeion = new Client_Session(new_socket, new_ser);

    //数据结构中加入新元素
    this->m_client_map->insert(new_socket, p_client_seeion);

    qDebug() << this->m_client_map->size() << endl;

    //创建事件对象 注册感兴趣的网络事件
    WSAEVENT new_event = WSACreateEvent();
    WSAEventSelect(new_socket, new_event, FD_READ | FD_WRITE | FD_CLOSE);

    //加入数组
    this->m_p_socket_ary[this->m_n_count] = new_socket;
    this->m_p_event_ary[this->m_n_count] = new_event;
    ++this->m_n_count;

    //发送一个上线包 让主机上线
    this->send_online(new_socket);
}

//处理数据 分发命令的接口
int Ps_Socket::ifs_recv(DWORD dw_index)
{
    int n_ret = 0;
    int n_recv_size = 0;

    Data_Head* p_head = 0;
    Client_Session* p_cs = 0;
    SOCKET h_socket = this->m_p_socket_ary[dw_index];

    //接收数据  拼包
    n_ret = recv(h_socket,
                 this->m_bt_ary->data(),
                 this->m_bt_ary->size(),
                 0);

    //如果接收出错
    if (SOCKET_ERROR == n_ret || 0 == n_ret)
    {
        qDebug() << "recv is err \n" ;
        goto SOCK_ERR;
    }

    //接收的数据加入cs缓冲区成员 清理QByteArray
    p_cs = this->get_cs(h_socket);
    p_cs->m_bt_buf->append(this->m_bt_ary->data(), n_ret);
    this->m_bt_ary->clear();

    //如果数据太短  小于数据头的长度
    if(p_cs->m_bt_buf->size() < sizeof(Data_Head))
    {
        return 0;
    }

    //没有软件签名表示 包不合法 清空 不处理
    if(!this->check_data_head(p_cs->m_bt_buf->data()))
    {
        p_cs->m_bt_buf->clear();
        return 0;
    }

    p_head = (Data_Head*)(p_cs->m_bt_buf->data());

    //取出数据长度 拼包
    n_recv_size = ((Data_Head*)p_head)->n_size;

    if(n_recv_size > 0)
    {
        if(p_cs->m_bt_buf->size() < n_recv_size)
        {
            return 0;
        }
    }

    //分发处理命令
    this->ifs_dps(p_cs);
    p_cs->m_bt_buf->clear();

    return 0;

SOCK_ERR:
    if (INVALID_SOCKET != h_socket)
    {
        closesocket(h_socket);
    }

    //emit dis_conn(); //这里要发送一个下线信号

    return -1;
}

int Ps_Socket::send_online(SOCKET h_socket)
{
    int n_ret = 0;

    if(0 == h_socket)
    {
        return -1;
    }

    //初始化 发送一个上线包
    Data_Head* p_get_info = new Data_Head(PSC_ONLINE);

    //发送命令让被控端上线
    n_ret = Net_Tools::send_buf(h_socket,
                                (char*)p_get_info,
                                sizeof(Data_Head));

    //如果发送失败
    if (SOCKET_ERROR == n_ret)
    {
        throw "send is error";
    }

    delete[] p_get_info;
    return 0;
}

void Ps_Socket::conn_this()
{
    //连接上线信号处理槽函数
    connect(this, &Ps_Socket::sin_online,
            this->m_wnd_this, &PS_M_Wnd::slot_online);

    connect(this, &Ps_Socket::sin_create_dlg,
            this->m_wnd_this, &PS_M_Wnd::slot_create_dlg);

    connect(this, &Ps_Socket::sin_recv_data,
            this->m_wnd_this, &PS_M_Wnd::slot_recv_data);
}

Client_Session* Ps_Socket::get_cs(SOCKET h_soeket)
{
    if(!this->m_client_map->contains(h_soeket))
    {
        return 0;
    }

    return this->m_client_map->value(h_soeket);
}

//处理心跳包
void Ps_Socket::ifs_heart(Client_Session* p_cs)
{
    Data_Head data_head(PSC_HEART_BEAT);
    Net_Tools::send_buf(p_cs->m_socket,
                        (char*)&data_head,
                        sizeof(Data_Head));
}

//检测数据头 如果没有 软件签名不处理
bool Ps_Socket::check_data_head(char *p_buf)
{
    int n_ret = 0;

    //对比前面有没有软件签名 'smile'
    n_ret = memcmp(PACK_FLAG,
                   ((Data_Head*)p_buf)->sz_flag,
                   PACK_FLAG_SIZE);

    if (n_ret != 0)
    {
        qDebug() << "bad online_data";
        return false;
    }

    return true;
}


int Ps_Socket::ifs_dps(Client_Session* p_cs)
{
    //初始化数据头指针 数据指针
    Data_Head* p_head = 0;
    p_head = (Data_Head*)(p_cs->m_bt_buf->data());

    //获取命令类型
    unsigned char c_cmd_type = p_head->c_type;

    //任何数据包发过来都有必要更新时间
    QTime time = QTime::currentTime();
    p_cs->m_last_time = time.msecsSinceStartOfDay();

    switch (c_cmd_type)
    {
    case PSC_HEART_BEAT:
        this->ifs_heart(p_cs);
        break;

    case PSC_ONLINE:
        this->ifs_online(p_cs);
        break;

    case PSC_CMD:
        this->ifs_cmd(p_cs);
        break;

    case PSC_REG:
        break;

    case PSC_PROC:
        break;

    case PSC_SCREEN:
        this->ifs_scr(p_cs);
        break;

    default:
        break;
    }

    return 0;
}

int Ps_Socket::ifs_online(Client_Session* p_cs)
{
    char* p_data = 0;

    SOCKET h_socket = p_cs->m_socket;
    sockaddr_in new_ser = p_cs->m_ser;

    p_data = p_cs->m_bt_buf->data() + sizeof(Data_Head);

    //深拷贝一份数据
    Line_Data* p_l = new Line_Data();
    memcpy(p_l, p_data, sizeof(Line_Data));

    //赋值ip 和 socket
    p_l->s_host_sock = h_socket;
    strcpy_s(p_l->sz_host_ip, inet_ntoa(new_ser.sin_addr));

    //发送信号 完成上线
    emit sin_online(p_l);
}

int Ps_Socket::ifs_cmd(Client_Session* p_cs)
{
    //初始化赋值 包头指针 数据指针
    char* p_data = 0;
    Data_Head* p_head = 0;

    p_head = (Data_Head*)(p_cs->m_bt_buf->data());
    p_data = p_cs->m_bt_buf->data() + sizeof(Data_Head);

    //命令id 数据包长度
    unsigned char c_cmd_id = p_head->c_id;
    int n_data_size = p_head->n_size;

    if(PSC_CMD_REPLY == c_cmd_id)
    {
        emit sin_create_dlg(p_cs, PSC_CMD_REPLY);
    }
    else if(PSC_CMD_DATA == c_cmd_id)
    {
        emit sin_recv_data(p_cs,
                           PSC_CMD_DATA,
                           QByteArray(p_data, n_data_size));
    }
}

int Ps_Socket::ifs_scr(Client_Session* p_cs)
{
    //初始化赋值 包头指针 数据指针
    char* p_data = 0;
    Data_Head* p_head = 0;

    p_head = (Data_Head*)(p_cs->m_bt_buf->data());
    p_data = p_cs->m_bt_buf->data() + sizeof(Data_Head);

    //命令id 数据包长度
    unsigned char c_cmd_id = p_head->c_id;
    int n_data_size = p_head->n_size;

    if(PSC_SCR_REPLY == c_cmd_id)
    {
        emit sin_create_dlg(p_cs, PSC_SCR_REPLY);
    }
    else if(PSC_SCR_DATA == c_cmd_id)
    {
        emit sin_recv_data(p_cs,
                           PSC_SCR_DATA,
                           QByteArray(p_data, n_data_size));
    }
}
