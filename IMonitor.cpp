// IMonitor.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "IMonitor.h"
#include "Log.h"
char ExePlace[1024] = {0};/*�����ַ������*/
char ExeDir[1024] = {0};
HWND hWnd = NULL;
CLog g_log(".\\ProtectClientLog.log");

BOOL CRelativeTime::ResetRelativeTime()		//���¿�ʼ��ʱ
{
	BOOL bQueryFrequency = QueryPerformanceFrequency(&m_frequency);						//ÿ����������
	BOOL bStartPerformanceCount = QueryPerformanceCounter(&m_startPerformanceCount);	//��ǰ��������
	return (bQueryFrequency && bStartPerformanceCount);
}
double CRelativeTime::GetRelativeTime()		//������ʱ
{
	BOOL bEndPerformanceCount = 0;
	bEndPerformanceCount = QueryPerformanceCounter(&m_endPerformanceCount);		//�����������
	//ʱ��תΪ�룩
	return (double)(m_endPerformanceCount.QuadPart-m_startPerformanceCount.QuadPart)/m_frequency.QuadPart;
}

CRelativeTime::CRelativeTime()
{
	ResetRelativeTime();
}
CRelativeTime::~CRelativeTime()
{

}

int CharToUnicode(char *pchIn, CString *pstrOut)
{
	int nLen;
	int nLen = 0;
	WCHAR *ptch;
	if(pchIn == NULL)
	{
		return 0;
	}
	nLen = MultiByteToWideChar(CP_ACP, 0, pchIn, -1, NULL, 0);
	ptch = new WCHAR[nLen];
	MultiByteToWideChar(CP_ACP, 0, pchIn, -1, ptch, nLen);
	pstrOut->Format(_T("%s"), ptch);
	delete [] ptch;
	return nLen;
}

bool getProcess(const char *procressName)               //�˺��������������ִ�Сд  
{  
	char pName[MAX_PATH];                               //��PROCESSENTRY32�ṹ���е�szExeFile�ַ����鱣��һ�£����ڱȽ�  
	strcpy_s(pName,procressName);                         //��������  
	CharLowerBuffA(pName,MAX_PATH);						 //������ת��ΪСд 
	CString csWName;
	CharToUnicode(pName, &csWName);
	PROCESSENTRY32 currentProcess;                      //��ſ��ս�����Ϣ��һ���ṹ��  
	currentProcess.dwSize = sizeof(currentProcess);     //��ʹ������ṹ֮ǰ�����������Ĵ�С  
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//��ϵͳ�ڵ����н�����һ������  

	if (hProcess == INVALID_HANDLE_VALUE)  
	{  
		printf("CreateToolhelp32Snapshot()����ʧ��!\n");  
		return false;  
	}  

	BOOL bMore=Process32First(hProcess,&currentProcess);        //��ȡ��һ��������Ϣ  
	int iii = 0;
	while(bMore)  
	{  
		CharLowerBuff(currentProcess.szExeFile,MAX_PATH);       //��������ת��ΪСд  
		int iResult = StrCmpW(currentProcess.szExeFile, csWName.GetBuffer(100));
		csWName.ReleaseBuffer(100);
		if (iResult == 0)          //�Ƚ��Ƿ���ڴ˽���  
		{  
			CloseHandle(hProcess);                              //���hProcess���  
			return TRUE;  
		}  
		bMore=Process32Next(hProcess,&currentProcess);          //������һ��  
		iii++;
	}  

	CloseHandle(hProcess);  //���hProcess���  
	return FALSE;  
}  

BOOL ProtectProcess(char* processName)
{
	if (processName == NULL)
	{
		return FALSE;
	}

	//���û���ҵ��Ļ�������ǰĿ¼�µ�IPCLIENT.exe
	if (!getProcess(processName))
	{
		ShellExecuteA(hWnd, "open", ".\\IPClient.exe", NULL, NULL, SW_HIDE);

		g_log.AddLog("Pull up ipclient.exe process Again\n");
	}
	return TRUE;
}
LONG addreg(char *p){
	HKEY hkey;
	LONG ReturnNum;
	DWORD dwType = REG_SZ;
	DWORD dwSize;
	dwSize=strlen(p);
	char name[12] = "iGoogleM";//���·����
	//��ӿ��������ע���
	ReturnNum=RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,KEY_ALL_ACCESS|KEY_WOW64_32KEY,&hkey);

	if(ReturnNum!=0)
		return -1;
	LONG lResult = RegSetValueExA(hkey, name,(DWORD)0,dwType, (BYTE*)p,dwSize);

	RegCloseKey(hkey);
	return lResult;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//����DOS����

	GetModuleFileNameA(0,ExePlace,1024);/*��ȡ��ǰ����ȫ·��*/
	char *p = strrchr(ExePlace, '\\');
	memcpy(ExeDir, ExePlace, strlen(ExePlace)-strlen(p));
	puts(ExePlace);/*�����ȡ����·��*/
	hWnd = FindWindowA("ConsoleWindowClass",ExePlace);
	ShowWindow(hWnd, SW_HIDE);//���ش���

	//�����Զ���������
	addreg(ExePlace);

	CRelativeTime rlTime;
	double dbRunTime;
	CRelativeTime rlTimeLog;
	double dTimeLog;
	while (TRUE)
	{
		dbRunTime = rlTime.GetRelativeTime();
		if (dbRunTime >= 10)//10S��ѵ����½����Ƿ��ڣ�����������
		{
			//��������
			ProtectProcess("ipclient.exe");
			g_log.WriteLog();
			rlTime.ResetRelativeTime();
		}

		Sleep(100);
	}
	return 0;
}

