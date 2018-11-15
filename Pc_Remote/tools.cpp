#include <TCHAR.h>
#include "tools.h"

//#define SE_DEBUG_PRIVILEGE 20

//���庯��ԭ��
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

    //����id ��ý��̾��
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
    DWORD dw_id = 0;     //����id
    HWND hwnd = 0;       //���ھ��
    HANDLE handle = 0;   //���̾��
    bool b_ret = false;

    //���ݱ����ȡ���ھ��
    hwnd = FindWindow(NULL, buf);

    if (hwnd == NULL)
    {
        return false;
    }

    //���ݴ��ھٰ��ȡ����id
    GetWindowThreadProcessId(hwnd, &dw_id);

    //����id ��ý��̾��
    b_ret = id_get_handle(dw_id, handle);

    return b_ret;
}

bool hwnd_get_hangle(HWND hwnd, HANDLE& ret_handle/*out*/)
{
    DWORD dw_id = 0;     //����id
    bool b_ret = false;

    if (0 == hwnd)
    {
        return false;
    }

    //���ݴ���hwnd��ȡ����id
    GetWindowThreadProcessId(hwnd, &dw_id);

    //����id ��ý��̾��
    b_ret = id_get_handle(dw_id, ret_handle);

    return b_ret;
}

/*
ret(-1) ��������
ret(-2) Ŀ���������ռ�ʧ��
ret(-3) Ŀ�����д��dll����ʧ��
ret(-4) ����Զ���߳�ʧ��
*/
int inject(HANDLE h_proc, const TCHAR* sz_dll_path,
           bool b_inherit /*= false*/, DWORD dw_mem_size /*= 0x1000*/)
{
    //��׳�Լ��
    if (0 == h_proc || 0 == sz_dll_path)
    {
        return -1;
    }

    //2 ��Ŀ����̿ռ��У�����һ���ڴ棬��Ŵ�ע���dll·��
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

    //3 ��Ŀ����� ����Ŀռ� д��ע��dll·��
    B_ret = WriteProcessMemory(h_proc,
                               lp_addr,
                               sz_dll_path,
                               (_tcsclen(sz_dll_path) + 1) * sizeof(TCHAR),
                               &dw_writed_bytes);

    //д����̿ռ�ʧ��
    if (!B_ret)
    {
        return -3;
    }


    //4 ����Զ���̣߳�ʹ��LoadLibrary������Ŀ��dll.
    HANDLE h_remote_thd = 0;

    h_remote_thd = CreateRemoteThread(h_proc,
                                      NULL,
                                      0,
                                      (LPTHREAD_START_ROUTINE)LoadLibrary,
                                      lp_addr,
                                      0,
                                      0);


    //����Զ���߳�ʧ��
    if (0 == h_remote_thd)
    {

        return -4;
    }

    return 0;
}

//��һ����Ȩ����
BOOL EnablePriv()
{
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))

        //����������취����ִ��ExitWindowsEXע����ʱ����ԣ�
        //�����������ػ��Ͳ����ˣ��ĳ����¿��Խ��
        if (!OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            TOKEN_PRIVILEGES tkp;

            LookupPrivilegeValue(NULL, SE_DEBUG_NAME,
                &tkp.Privileges[0].Luid);//�޸Ľ���Ȩ��

            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp,
                NULL, NULL);//֪ͨϵͳ�޸Ľ���Ȩ��

            return((GetLastError() == ERROR_SUCCESS));
        }

    return TRUE;
}

int set_privilege()
{
    HANDLE h_token;
    TOKEN_PRIVILEGES token_state;
    LUID luid_privilege;

    //��ȡ�������� printf("SetPrivilege Error\n");
    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,
                         &h_token)||!LookupPrivilegeValue(NULL,
                                    SE_DEBUG_NAME, &luid_privilege))
    {
        return -1;
    }

    token_state.PrivilegeCount = 1;
    token_state.Privileges[0].Luid = luid_privilege;
    token_state.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    //��������Ȩ�ޣ�ע��ú���Ҳ���Ըı��̵߳�Ȩ�ޣ����hTokenָ��һ���̵߳�����
    //printf("AdjustTokenPrivilege Errro\n");
    if(!AdjustTokenPrivileges(h_token, FALSE, &token_state, NULL, NULL, NULL))
    {
        return -2;
    }

    // �ڶ�����Ȩ����
    ////װ��DLL
    //HMODULE hModule = ::LoadLibrary(TEXT("NTDLL.DLL"));
    ////printf("LoadLibrary error\n");
    //if(hModule == NULL)
    //{
    //    return -1;
    //}

    ////�õ����������ĵ�ַ
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

