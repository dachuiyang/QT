#include <TCHAR.h>
#include "tools.h"

//#define SE_DEBUG_PRIVILEGE 20

//定义函数原型
//typedef int (__stdcall *Func_RtlAdjustPrivilege)
//            (ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);

bool get_proc_handle(Find_Mode find_mode, DWORD arg,
                    HANDLE& ret_handle/*out*/)
{
    bool b_ret = false;
    HANDLE handle = 0;

    if (ID_FIND == find_mode)
    {
        b_ret = id_get_handle(arg, handle);
    }
    else if (TITLE_FIND == find_mode)
    {
        b_ret = title_get_handle((TCHAR*)arg, handle);
    }
    else if (HWND_FIND == find_mode)
    {
        b_ret = hwnd_get_hangle((HWND)arg, handle);
    }

    if (false == b_ret)
    {
        return false;
    }

    ret_handle = handle;

    return true;
}

bool id_get_handle(DWORD dw_id, HANDLE& ret_handle/*out*/)
{
    HANDLE handle = 0;

    //根据id 获得进程句柄
    handle = OpenProcess(PROCESS_ALL_ACCESS,
                         false,
                         dw_id);

    if (0 == handle)
    {
        return false;
    }

    ret_handle = handle;
    return true;
}

bool title_get_handle(TCHAR* buf, HANDLE& ret_handle/*out*/)
{
    DWORD dw_id = 0;     //进程id
    HWND hwnd = 0;       //窗口句柄
    HANDLE handle = 0;   //进程句柄
    bool b_ret = false;

    //根据标题获取窗口句柄
    hwnd = FindWindow(NULL, buf);

    if (hwnd == NULL)
    {
        return false;
    }

    //根据窗口举办获取进程id
    GetWindowThreadProcessId(hwnd, &dw_id);

    //根据id 获得进程句柄
    b_ret = id_get_handle(dw_id, handle);

    return b_ret;
}

bool hwnd_get_hangle(HWND hwnd, HANDLE& ret_handle/*out*/)
{
    DWORD dw_id = 0;     //进程id
    bool b_ret = false;

    if (0 == hwnd)
    {
        return false;
    }

    //根据窗口hwnd获取进程id
    GetWindowThreadProcessId(hwnd, &dw_id);

    //根据id 获得进程句柄
    b_ret = id_get_handle(dw_id, ret_handle);

    return b_ret;
}

/*
ret(-1) 参数错误
ret(-2) 目标进程申请空间失败
ret(-3) 目标进程写入dll数据失败
ret(-4) 调用远程线程失败
*/
int inject(HANDLE h_proc, const TCHAR* sz_dll_path,
           bool b_inherit /*= false*/, DWORD dw_mem_size /*= 0x1000*/)
{
    //健壮性检查
    if (0 == h_proc || 0 == sz_dll_path)
    {
        return -1;
    }

    //2 在目标进程空间中，开辟一块内存，存放待注入的dll路径
    LPVOID lp_addr = 0;

    lp_addr = VirtualAllocEx(h_proc,
                             0,
                             dw_mem_size,
                             MEM_COMMIT,
                             PAGE_EXECUTE_READWRITE);

    if (0 == lp_addr)
    {
        return -2;
    }

    BOOL B_ret = FALSE;
    DWORD dw_writed_bytes = 0;

    //3 向目标进程 申请的空间 写入注入dll路径
    B_ret = WriteProcessMemory(h_proc,
                               lp_addr,
                               sz_dll_path,
                               (_tcsclen(sz_dll_path) + 1) * sizeof(TCHAR),
                               &dw_writed_bytes);

    //写入进程空间失败
    if (!B_ret)
    {
        return -3;
    }


    //4 调用远程线程，使用LoadLibrary，加载目标dll.
    HANDLE h_remote_thd = 0;

    h_remote_thd = CreateRemoteThread(h_proc,
                                      NULL,
                                      0,
                                      (LPTHREAD_START_ROUTINE)LoadLibrary,
                                      lp_addr,
                                      0,
                                      0);


    //创建远程线程失败
    if (0 == h_remote_thd)
    {

        return -4;
    }

    return 0;
}

//另一种提权方法
BOOL EnablePriv()
{
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))

        //用上面这个办法，在执行ExitWindowsEX注销的时候可以，
        //但是重启，关机就不行了，改成以下可以解决
        if (!OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            TOKEN_PRIVILEGES tkp;

            LookupPrivilegeValue(NULL, SE_DEBUG_NAME,
                &tkp.Privileges[0].Luid);//修改进程权限

            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp,
                NULL, NULL);//通知系统修改进程权限

            return((GetLastError() == ERROR_SUCCESS));
        }

    return TRUE;
}

int set_privilege()
{
    HANDLE h_token;
    TOKEN_PRIVILEGES token_state;
    LUID luid_privilege;

    //获取进程令牌 printf("SetPrivilege Error\n");
    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,
                         &h_token)||!LookupPrivilegeValue(NULL,
                                    SE_DEBUG_NAME, &luid_privilege))
    {
        return -1;
    }

    token_state.PrivilegeCount = 1;
    token_state.Privileges[0].Luid = luid_privilege;
    token_state.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    //提升进程权限，注意该函数也可以改变线程的权限，如果hToken指向一个线程的令牌
    //printf("AdjustTokenPrivilege Errro\n");
    if(!AdjustTokenPrivileges(h_token, FALSE, &token_state, NULL, NULL, NULL))
    {
        return -2;
    }

    // 第二种提权方法
    ////装载DLL
    //HMODULE hModule = ::LoadLibrary(TEXT("NTDLL.DLL"));
    ////printf("LoadLibrary error\n");
    //if(hModule == NULL)
    //{
    //    return -1;
    //}

    ////得到导出函数的地址
    //Func_RtlAdjustPrivilege RtlAdjustPrivilege =
    //        (Func_RtlAdjustPrivilege)GetProcAddress(hModule,
    //                                "Func_RtlAdjustPrivilege");

    //// printf("GetProcAddress error \n");
    //if(RtlAdjustPrivilege == NULL)
    //{
    //    return -2;
    //}

    //RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, 1, 0, NULL);

    return 0;
}

