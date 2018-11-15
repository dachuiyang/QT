#include "q_cmd.h"
#include <QDebug>
#define CMD_LINE "cmd.exe"

Q_Cmd::Q_Cmd()
{
    this->init();
}

Q_Cmd::~Q_Cmd()
{
    //关闭4个读写句柄 和cmd进程句柄
    CloseHandle(this->m_h_cmd_read);
    CloseHandle(this->m_h_cmd_write);
    CloseHandle(this->m_h_cur_read);
    CloseHandle(this->m_h_cur_write);

    CloseHandle(this->m_h_cmd_proc);
}

void Q_Cmd::run()
{
    this->read_cmd_msg();
}

int Q_Cmd::init()
{
    //初始化4个读写句柄
    this->m_h_cur_read = 0;
    this->m_h_cur_write = 0;
    this->m_h_cmd_read = 0;
    this->m_h_cmd_write = 0;

    //初始化cmd进程的进程句柄
    this->m_h_cmd_proc = 0;

    //初始化管道和cmd
    this->init_pipe();
    this->init_cmd();

    return 0;
}

int Q_Cmd::init_cmd()
{
    bool b_ret = false;

    //初始化命令行参数  安全描述符 进程信息结构体
    char sz_cmd_line[MAX_PATH] = { 0 };
    strcpy_s(sz_cmd_line, MAX_PATH, CMD_LINE);

    PROCESS_INFORMATION pi = {0};

    STARTUPINFOA si = {0};
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESTDHANDLES; //使用标准输入输出句柄
    si.hStdInput = this->m_h_cmd_read;
    si.hStdError = this->m_h_cmd_write;
    si.hStdOutput = this->m_h_cmd_write;

    //创建cmd
    b_ret = CreateProcessA(NULL,
                           sz_cmd_line,
                           NULL,
                           NULL,
                           TRUE,
                           CREATE_NO_WINDOW,
                           NULL,
                           NULL,
                           &si,
                           &pi);

    //赋值cmd进程句柄 给成员函数
    this->m_h_cmd_proc = 0;
    this->m_h_cmd_proc = pi.hProcess;

    return 0;
}

int Q_Cmd::init_pipe()
{
    bool b_ret = false;

    //初始化安全结构体
    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    //创建双向管道 沟通cmd 和 当前进程
    b_ret = CreatePipe(&m_h_cur_read, &m_h_cmd_write, &sa, 0);
    b_ret = CreatePipe(&m_h_cmd_read, &m_h_cur_write, &sa, 0);

    return 0;
}

int Q_Cmd::write_cmd_msg(QString out)
{
    bool b_ret = false;
    DWORD dw_write_byte = 0;

    //qDebug() << out.toStdString().c_str() << endl;
    //qDebug() << out.size() << endl;

    b_ret = WriteFile(this->m_h_cur_write,
                      out.toStdString().c_str(),
                      out.size(),
                      &dw_write_byte,
                      0);

    return 0;
}

int Q_Cmd::read_cmd_msg()
{
    bool b_ret = false;

    DWORD dw_avl_byte = 0;    //可读字节数
    DWORD dw_cur_byte = 0;    //当前读取字节总数
    DWORD dw_read_byte = 0;   //成功读取字节数
    DWORD dw_unread_byte = 0; //为基于消息管道设置 此刻不适用

    while(1)
    {
        //轮询可读内容
        b_ret = PeekNamedPipe
                (m_h_cur_read,
                 0,
                 0,
                 &dw_read_byte,
                 &dw_avl_byte,
                 &dw_unread_byte);

        //如果没有可读内容
        if(dw_avl_byte <= 0)
        {
            continue;
        }

        //申请初始化缓冲区
        char* buf = new char[dw_avl_byte + 1];
        memset(buf, 0, dw_avl_byte + 1);

        dw_cur_byte = 0;

        //循环读取 直到读完
        while (dw_cur_byte < dw_avl_byte)
        {
            ReadFile(m_h_cur_read,
                     buf + dw_cur_byte,
                     dw_avl_byte,
                     &dw_read_byte,
                     0);

            dw_cur_byte += dw_read_byte;
        }

        //发送信号给主线程  输出到主窗口
        emit cmd_out(QString::fromLocal8Bit(buf));

        //子线程不能直接操作 UI 发信号让主线程操作UI比较稳
        //ui->te_cmd->append(QString(buf));

        delete[] buf;
    }

}


