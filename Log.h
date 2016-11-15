#include "stdafx.h"

class CLog
{
public:
	BOOL WriteLog();
	BOOL AddLog(char* pData);
public:
	CLog(CString strLogPath);
	~CLog();
private:
	CString m_strFilePath;
	CStringArray m_strLogDateArr;
};