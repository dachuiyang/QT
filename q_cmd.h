#ifndef Q_CMD_H
#define Q_CMD_H

#include <QThread>
#include <windows.h>

class Q_Cmd : public QThread
{
   Q_OBJECT
public:
    Q_Cmd();
    ~Q_Cmd();

public:
    virtual void run();

signals:
    void cmd_out(QString out_str);

public:
    int init();
    int init_cmd();
    int init_pipe();

public:
    int write_cmd_msg(QString out);
    int read_cmd_msg();

private:
    HANDLE m_h_cur_read;
    HANDLE m_h_cur_write;
    HANDLE m_h_cmd_read;
    HANDLE m_h_cmd_write;

    HANDLE m_h_cmd_proc;
};

#endif // Q_CMD_H
