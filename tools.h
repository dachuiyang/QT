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
���ܣ�ע��dll �� ����
arg1:���̾��
arg2:ע��dll��·��
arg3:�Ƿ�̳�
arg4:Ŀ���������Ŀռ��С ���������dll��

ret(-1) ��������                 ret(-2) Ŀ���������ռ�ʧ��
ret(-3) Ŀ�����д��dll����ʧ��   ret(-4) ����Զ���߳�ʧ��
*/
int inject(HANDLE h_proc, const TCHAR* sz_dll_path,
           bool b_inherit = false, DWORD dw_mem_size = 0x1000);

//������Ȩ����
int set_privilege();

BOOL EnablePriv();