// IMonitor.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "IMonitor.h"
#include "Log.h"
char ExePlace[1024] = {0};/*保存地址的数组*/
char ExeDir[1024] = {0};
HWND hWnd = NULL;
CLog g_log(".\\ProtectClientLog.log");

BOOL CRelativeTime::ResetRelativeTime()		//重新开始计时
{
	BOOL bQueryFrequency = QueryPerformanceFrequency(&m_frequency);						//每秒跳动次数
	BOOL bStartPerformanceCount = QueryPerformanceCounter(&m_startPerformanceCount);	//测前跳动次数
	return (bQueryFrequency && bStartPerformanceCount);
}
double CRelativeTime::GetRelativeTime()		//结束计时
{
	BOOL bEndPerformanceCount = 0;
	bEndPerformanceCount = QueryPerformanceCounter(&m_endPerformanceCount);		//测后跳动次数
	//时间差（转为秒）
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

bool getProcess(const char *procressName)               //此函数进程名不区分大小写  
{  
	char pName[MAX_PATH];                               //和PROCESSENTRY32结构体中的szExeFile字符数组保持一致，便于比较  
	strcpy_s(pName,procressName);                         //拷贝数组  
	CharLowerBuffA(pName,MAX_PATH);						 //将名称转换为小写 
	CString csWName;
	CharToUnicode(pName, &csWName);
	PROCESSENTRY32 currentProcess;                      //存放快照进程信息的一个结构体  
	currentProcess.dwSize = sizeof(currentProcess);     //在使用这个结构之前，先设置它的大小  
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//给系统内的所有进程拍一个快照  

	if (hProcess == INVALID_HANDLE_VALUE)  
	{  
		printf("CreateToolhelp32Snapshot()调用失败!\n");  
		return false;  
	}  

	BOOL bMore=Process32First(hProcess,&currentProcess);        //获取第一个进程信息  
	int iii = 0;
	while(bMore)  
	{  
		CharLowerBuff(currentProcess.szExeFile,MAX_PATH);       //将进程名转换为小写  
		int iResult = StrCmpW(currentProcess.szExeFile, csWName.GetBuffer(100));
		csWName.ReleaseBuffer(100);
		if (iResult == 0)          //比较是否存在此进程  
		{  
			CloseHandle(hProcess);                              //清除hProcess句柄  
			return TRUE;  
		}  
		bMore=Process32Next(hProcess,&currentProcess);          //遍历下一个  
		iii++;
	}  

	CloseHandle(hProcess);  //清除hProcess句柄  
	return FALSE;  
}  

BOOL ProtectProcess(char* processName)
{
	if (processName == NULL)
	{
		return FALSE;
	}

	//如果没有找到的话，拉起当前目录下的IPCLIENT.exe
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
	char name[12] = "iGoogleM";//存放路径。
	//添加开机启动项到注册表。
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
	//隐藏DOS窗口

	GetModuleFileNameA(0,ExePlace,1024);/*获取当前运行全路径*/
	char *p = strrchr(ExePlace, '\\');
	memcpy(ExeDir, ExePlace, strlen(ExePlace)-strlen(p));
	puts(ExePlace);/*输出获取到的路径*/
	hWnd = FindWindowA("ConsoleWindowClass",ExePlace);
	ShowWindow(hWnd, SW_HIDE);//隐藏窗口

	//加入自动启动功能
	addreg(ExePlace);

	CRelativeTime rlTime;
	double dbRunTime;
	CRelativeTime rlTimeLog;
	double dTimeLog;
	while (TRUE)
	{
		dbRunTime = rlTime.GetRelativeTime();
		if (dbRunTime >= 10)//10S轮训检测下进程是否在，不在拉起来
		{
			//保护程序
			ProtectProcess("ipclient.exe");
			g_log.WriteLog();
			rlTime.ResetRelativeTime();
		}

		Sleep(100);
	}
	return 0;
}

