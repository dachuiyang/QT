#include <windows.h>

enum Find_Mode
{
    ID_FIND,
    TITLE_FIND,
    HWND_FIND,
    NAME_FIND
};

bool get_proc_handle(Find_Mode find_mode, DWORD arg,
                     HANDLE& handle/*out*/);

bool id_get_handle(DWORD dw_id, HANDLE& ret_handle/*out*/);
bool title_get_handle(TCHAR* buf, HANDLE& ret_handle/*out*/);
bool hwnd_get_hangle(HWND hwnd, HANDLE& ret_handle/*out*/);


/*
功能：注入dll 到 进程
arg1:进程句柄
arg2:注入dll的路径
arg3:是否继承
arg4:目标进程申请的空间大小 （用来存放dll）

ret(-1) 参数错误                 ret(-2) 目标进程申请空间失败
ret(-3) 目标进程写入dll数据失败   ret(-4) 调用远程线程失败
*/
int inject(HANDLE h_proc, const TCHAR* sz_dll_path,
           bool b_inherit = false, DWORD dw_mem_size = 0x1000);

//两种提权方法
int set_privilege();

BOOL EnablePriv();