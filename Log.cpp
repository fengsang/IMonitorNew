#include "stdafx.h"
#include "Log.h"

BOOL CLog::WriteLog()
{
	CStdioFile file;
	if(!file.Open(m_strFilePath,CFile::modeReadWrite))     
	{     
		file.Open(m_strFilePath, CFile::modeCreate|CFile::modeReadWrite);     
	} 

	CString strData;
	file.SeekToEnd();
	for (int i=0; i<m_strLogDateArr.GetSize(); i++)
	{
		strData = m_strLogDateArr.GetAt(i);
		file.WriteString(strData);
	}
	file.Close();
	m_strLogDateArr.RemoveAll();

	return TRUE;
}

BOOL CLog::AddLog(char* pData)
{
	if (!pData)
	{
		return FALSE;
	}

	SYSTEMTIME sytemTime;
	char strTime[100] = {0};
	GetLocalTime(&sytemTime);
	sprintf_s(strTime, "%04d%02d%02d %02d:%02d:%02d %s", sytemTime.wYear, sytemTime.wMonth, 
		sytemTime.wDay, sytemTime.wHour, sytemTime.wMinute, sytemTime.wSecond, pData);

	CString strTemp(strTime);
	m_strLogDateArr.Add(strTemp);

	return FALSE;
}

CLog::CLog(CString strLogPath)
{
	m_strFilePath = strLogPath;
}

CLog::~CLog()
{
	m_strLogDateArr.RemoveAll();
}

