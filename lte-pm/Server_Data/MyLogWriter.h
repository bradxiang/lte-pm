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
	static BOOL WriteText(CString v_sLog);								//Ð´logº¯Êý
	static BOOL WriteErrorText(CString v_sLog);							//Ð´log_errorº¯Êý
};

