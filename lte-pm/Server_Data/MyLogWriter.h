#pragma once
//#include "StdAfx.h"
#include   <afxmt.h> 

class  CMyLogWriter 
{
public:
	CMyLogWriter(void);
	~CMyLogWriter(void);
public:
	static CCriticalSection WriteLock;
public:
	static BOOL WriteText(CString v_sLog);								//дlog����
	static BOOL WriteErrorText(CString v_sLog);							//дlog_error����
};

