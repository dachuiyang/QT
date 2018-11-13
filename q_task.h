#ifndef Q_TASK_H
#define Q_TASK_H

#include <windows.h>
#include <Tlhelp32.h>
#include <QTableWidget>

typedef PROCESSENTRY32 Proc_Info;

//定义类成员回调函数 处理读取的每个进程
class Q_Task;
typedef int (Q_Task::*Dps_Proc)(Proc_Info&);

class Q_Task
{
public:
    Q_Task();

public:
    void init_proc_table(QTableWidget* tw_proc);
    void init_mod_table(QTableWidget* tw_proc);

public:
    int read_proc_info(Proc_Info & proc_info);

public:
    static bool task_List(Dps_Proc dps_proc, Q_Task* p_this);

private:
    QTableWidget* m_p_tw_proc;
    QTableWidget* m_p_tw_mod;
};

#endif // Q_TASK_H
