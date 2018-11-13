#include "q_task.h"
#include <QFont>
#include <QString>
#include <QHeaderView>
#include "tools.h"


char* tw_head_ary[] =
{
   "进程ID",
   "         进程名      ",
   "父进程ID",
   "线程数",
   "进程全路径"
};

enum Em_Tw_head
{
   PROC_ID,
   PROC_NAME,
   PROC_PARENT_ID,
   THREAD_CNT,
   PROC_DIR
};

typedef BOOL (*PFN_QueryFullName)(
  HANDLE hProcess,
  DWORD  dwFlags,
  LPSTR  lpExeName,
  PDWORD lpdwSize
);

Q_Task::Q_Task()
{
    set_privilege();
}

void Q_Task::init_proc_table(QTableWidget *p_tw_proc)
{
    this->m_p_tw_proc = p_tw_proc;

    //设置标题容器
    QStringList tw_head_list;

    tw_head_list.append(QString(tw_head_ary[PROC_ID]));
    tw_head_list.append(QString(tw_head_ary[PROC_NAME]));
    tw_head_list.append(QString(tw_head_ary[PROC_PARENT_ID]));
    tw_head_list.append(QString(tw_head_ary[THREAD_CNT]));
    tw_head_list.append(QString(tw_head_ary[PROC_DIR]));

    int n_tw_head = tw_head_list.count();

    this->m_p_tw_proc->setRowCount(200);
    this->m_p_tw_proc->setColumnCount(n_tw_head);

    //设置列标签
    this->m_p_tw_proc->setHorizontalHeaderLabels(tw_head_list);

    //设置行和列的大小设为与内容相匹配
    this->m_p_tw_proc->resizeColumnsToContents();
    this->m_p_tw_proc->resizeRowsToContents();

    //设置表头不可点击
    //tw_proc->horizontalHeader()->setClickable(false);

    //设置表头字体加粗
    QFont font = this->m_p_tw_proc->horizontalHeader()->font();
    font.setBold(true);
    this->m_p_tw_proc->horizontalHeader()->setFont(font);

    //设置充满表宽度
    this->m_p_tw_proc->horizontalHeader()->setStretchLastSection(true);

    //设置选择行为时每次选择一行
    this->m_p_tw_proc->setSelectionBehavior(
                QAbstractItemView::SelectRows);

    //设置不可编辑
    this->m_p_tw_proc->setEditTriggers(
                QAbstractItemView::NoEditTriggers);

    //设置选中背景色
    this->m_p_tw_proc->setStyleSheet(
                "selection-background-color:lightblue;");

    //设置表头背景色
    this->m_p_tw_proc->horizontalHeader()->setStyleSheet(
                "QHeaderView::section{background:skyblue;}");

}

//读取进程信息 插入表格
int Q_Task::read_proc_info(Proc_Info &proc_info)
{
    //下标值 用来做遍历的次序
    static int n_index = 0;

    DWORD dw_ret = 0;
    HANDLE h_proc = 0;
    char sz_proc_path[MAX_PATH] = {0};

    //根据ID打开进程
    h_proc = OpenProcess(
                    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                    FALSE,
                    proc_info.th32ProcessID);

    HMODULE hModule =  GetModuleHandleA("kernel32.dll");
    if (hModule == NULL){
        return 0;
    }

    PFN_QueryFullName pfn =
            (PFN_QueryFullName)GetProcAddress(hModule,
                                              "QueryFullProcessImageNameA");

    if (0 == pfn)
    {
        return 0;
    }

    pfn(h_proc,
        0,
        sz_proc_path,
        &dw_ret);

    QString str_t_cnt = QString("%1").arg(proc_info.cntThreads);
    QString str_proc_id = QString("%1").arg(proc_info.th32ProcessID);
    QString str_proc_fid = QString("%1").arg(proc_info.th32ParentProcessID);
    QString str_proc_name = QString::fromWCharArray(proc_info.szExeFile);


    QTableWidgetItem* name_item =
            new QTableWidgetItem(str_proc_name);

    QTableWidgetItem* t_cnt_item =
            new QTableWidgetItem(QString(str_t_cnt));

    QTableWidgetItem* id_item =
            new QTableWidgetItem(QString(str_proc_id));

    QTableWidgetItem* father_id_item =
            new QTableWidgetItem(str_proc_fid);

    QTableWidgetItem* proc_path_item =
            new QTableWidgetItem(QString(sz_proc_path));

    this->m_p_tw_proc->setItem(n_index, PROC_ID, id_item);
    this->m_p_tw_proc->setItem(n_index, THREAD_CNT, t_cnt_item);
    this->m_p_tw_proc->setItem(n_index, PROC_PARENT_ID, father_id_item);
    this->m_p_tw_proc->setItem(n_index, PROC_NAME, name_item);
    this->m_p_tw_proc->setItem(n_index, PROC_DIR, proc_path_item);

    n_index++;

    return 0;
}

bool Q_Task::task_List(Dps_Proc dps_proc, Q_Task* p_this)
{
    HANDLE h_Proc_Snap = 0;       //进程快照句柄
    PROCESSENTRY32 pe32 = { 0 };  //进程信息结构图

    //'获取系统中所有进程的快照'
    h_Proc_Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == h_Proc_Snap)
    {
        return false;
    }

    //'在使用结构体之前填写其大小'
    pe32.dwSize = sizeof(Proc_Info);

    //遍历快照的每个进程 获取失败返回
    if (!Process32First(h_Proc_Snap, &pe32))
    {
        return false;
    }

    do
    {
        (p_this->*dps_proc)(pe32);  //回调函数处理

        // qDebug() << QString::fromStdWString(pe32.szExeFile) << endl;
        // qDebug() << QString::fromWCharArray(pe32.szExeFile) << endl;

    } while (Process32Next(h_Proc_Snap, &pe32));

    //'清理快照对象'
    CloseHandle(h_Proc_Snap);
    return true;
}
