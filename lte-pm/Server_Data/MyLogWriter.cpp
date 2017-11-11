#include "StdAfx.h"
#include "MyLogWriter.h"
#include "Server_Data.h"





//------------------------------------------------------------------------------------------------------			
//	函数功能：构造函数
//------------------------------------------------------------------------------------------------------
CCriticalSection CMyLogWriter::WriteLock;
CMyLogWriter::CMyLogWriter(void)
{
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：析构函数
//------------------------------------------------------------------------------------------------------
CMyLogWriter::~CMyLogWriter(void)
{
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：写日志文件；(log.txt)
//------------------------------------------------------------------------------------------------------
BOOL CMyLogWriter::WriteText(CString v_sLog)
{
	CString v_sText;
	CTime v_tCurrent=CTime::GetCurrentTime();
	v_sText=v_tCurrent.Format("%Y-%m-%d %H:%M:%S");
	if(v_sLog.GetLength()!=0)
		v_sText="【"+v_sText+"】 "+v_sLog+"\r\n";						//日期+日志+回车换行
	else
		v_sText="\r\n";													//回车换行(空一行)

	WriteLock.Lock();	
	try
	{
		CFile v_fWriter;
		if(v_fWriter.Open(theApp.m_sPath+"\\log.txt" , CFile::modeCreate | CFile::modeNoTruncate  | CFile::modeWrite))
		{
			v_fWriter.SeekToEnd();
			v_fWriter.Write(v_sText,v_sText.GetLength());
			v_fWriter.Close();
		}
		WriteLock.Unlock();	
		return TRUE;
	}
	catch (...)
	{
		WriteLock.Unlock();	
		return FALSE;
	}	
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：写出错信息文件；(log_error.txt)
//------------------------------------------------------------------------------------------------------
BOOL CMyLogWriter::WriteErrorText(CString v_sLog)
{
	CString v_sText;
	CTime v_tCurrent=CTime::GetCurrentTime();
	v_sText=v_tCurrent.Format("%Y-%m-%d %H:%M:%S");
	v_sText="【"+v_sText+"】 "+v_sLog+"\r\n";							//日期+日志+回车换行

	WriteLock.Lock();
	try
	{
		CFile v_fWriter;
		if(v_fWriter.Open(theApp.m_sPath+"\\log_error.txt" , CFile::modeCreate | CFile::modeNoTruncate  | CFile::modeWrite))
		{
			v_fWriter.SeekToEnd();
			v_fWriter.Write(v_sText,v_sText.GetLength());
			v_fWriter.Close();
		}
		WriteLock.Unlock();	
		return TRUE;
	}
	catch (...)
	{
		WriteLock.Unlock();	
		return FALSE;
	}
}

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------