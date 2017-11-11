#include "StdAfx.h"
#include "MyLogWriter.h"
#include "Server_Data.h"





//------------------------------------------------------------------------------------------------------			
//	�������ܣ����캯��
//------------------------------------------------------------------------------------------------------
CCriticalSection CMyLogWriter::WriteLock;
CMyLogWriter::CMyLogWriter(void)
{
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���������
//------------------------------------------------------------------------------------------------------
CMyLogWriter::~CMyLogWriter(void)
{
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�д��־�ļ���(log.txt)
//------------------------------------------------------------------------------------------------------
BOOL CMyLogWriter::WriteText(CString v_sLog)
{
	CString v_sText;
	CTime v_tCurrent=CTime::GetCurrentTime();
	v_sText=v_tCurrent.Format("%Y-%m-%d %H:%M:%S");
	if(v_sLog.GetLength()!=0)
		v_sText="��"+v_sText+"�� "+v_sLog+"\r\n";						//����+��־+�س�����
	else
		v_sText="\r\n";													//�س�����(��һ��)

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
//	�������ܣ�д������Ϣ�ļ���(log_error.txt)
//------------------------------------------------------------------------------------------------------
BOOL CMyLogWriter::WriteErrorText(CString v_sLog)
{
	CString v_sText;
	CTime v_tCurrent=CTime::GetCurrentTime();
	v_sText=v_tCurrent.Format("%Y-%m-%d %H:%M:%S");
	v_sText="��"+v_sText+"�� "+v_sLog+"\r\n";							//����+��־+�س�����

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