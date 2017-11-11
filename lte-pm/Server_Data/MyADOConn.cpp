// ADOConn.cpp: implementation of the ADOConn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyADOConn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMyADOConn::CMyADOConn()
{
	m_bFlag = FALSE;
}

CMyADOConn::~CMyADOConn()
{
}

//------------------------------------------------------------------------------------------------------------
//	��ʼ�����ݿ�����
//	�������:  v_iSelect(���ݿ�ѡ��)       v_sDb0            v_sDb1      v_sDb2     v_sDb3
//				 0:   SQL���ݿ�       ���ݿ��������ַ/����  ���ݿ�����  �û�����   ����
//               1:   ACCESS���ݿ�                           ���ݿ�����  �û�����   ����
//               2:   ODBC����Դ            ���ݿ�Դ                     �û�����   ����
//	������:  �������ݿ�ɹ������Connection����
//------------------------------------------------------------------------------------------------------------
void  CMyADOConn::OnInitADOConn(int v_iSelect,CString v_sDb0,CString v_sDb1,CString v_sDb2,CString v_sDb3)
{
	_bstr_t strConnect;
	::CoInitialize(NULL);												// ��ʼ��OLE/COM�⻷�� 
	try
	{
		m_pConnection.CreateInstance("ADODB.Connection");				// ����Connection����
		switch(v_iSelect)												// �������ݿ����ͣ����������ַ�����������BSTR�ͻ���_bstr_t����
		{
		case 0:															// SQL ���ݿ�
			strConnect = "Provider=SQLOLEDB;Server="+v_sDb0+";Database="+v_sDb1+";uid="+v_sDb2+";pwd="+v_sDb3;	
			break;
		case 1:															// ACCESS ���ݿ�
			strConnect = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source="+v_sDb1+";jet oledb:database password="+v_sDb3;	
			break;
		case 2:															// ODBC ���ʷ�ʽ
			strConnect = "Data Source="+v_sDb0+";uid="+v_sDb2+";pwd="+v_sDb3;	
			break;
		}
		m_pConnection->Open(strConnect,"","",adModeUnknown);			// �������ݿ�
		m_bFlag = TRUE;
	}
	catch(_com_error e)													// ��׽�쳣
	{
		m_bFlag = FALSE;
		throw ReCreatError(e,"�쳣�����ݿ����� ("+v_sDb1+") ");			// �׳�������Ϣ���쳣	
	}
}

//------------------------------------------------------------------------------------------------------------
//	ִ�в�ѯ,���ؼ�¼��: ��Ӧselect���
//	������bstrSQL��SQL����
//------------------------------------------------------------------------------------------------------------
_RecordsetPtr&  CMyADOConn::GetRecordSet(_bstr_t bstrSQL)
{
	try
	{
		m_pRecordset.CreateInstance(__uuidof(Recordset));				// ������¼������
		m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
		// ȡ�ñ��еļ�¼
	}
	catch(_com_error e)													// ��׽�쳣
	{
		m_bFlag = FALSE;
		CString v_sTemp=(LPCSTR)bstrSQL;
		throw ReCreatError(e,"�쳣�����ݿ��ѯ ("+v_sTemp+") ");		//	�׳�������Ϣ���쳣
	}
	return m_pRecordset;												// ���ؼ�¼��
}

//------------------------------------------------------------------------------------------------------------
//	��ȡ��int�ֶ�ֵ
//	������v_sField���ֶ�
//	���أ�int
//------------------------------------------------------------------------------------------------------------
int CMyADOConn::GetField_Int(CString v_sField)
{
	_variant_t Var;
	int vi;
	Var=m_pRecordset->GetCollect((_variant_t)v_sField);
	if (Var.vt != VT_NULL)
		vi=Var.intVal;
	else
		vi=0;
	return vi;
}

//------------------------------------------------------------------------------------------------------------
//	��ȡ��bool�ֶ�ֵ
//	������v_sField���ֶ�
//	���أ�bool
//------------------------------------------------------------------------------------------------------------
bool CMyADOConn::GetField_Bool(CString v_sField)
{
	_variant_t Var;
	bool vb;
	Var=m_pRecordset->GetCollect((_variant_t)v_sField);
	if (Var.vt != VT_NULL)
		vb=(bool)Var.boolVal;
	else
		vb=false;
	return vb;
}

//------------------------------------------------------------------------------------------------------------
//	��ȡ��float�ֶ�ֵ
//	������v_sField���ֶ�
//	���أ�double
//------------------------------------------------------------------------------------------------------------
double CMyADOConn::GetField_Double(CString v_sField)
{
	_variant_t Var;
	double vd;
	Var=m_pRecordset->GetCollect((_variant_t)v_sField);
	if (Var.vt != VT_NULL)
		vd=Var.dblVal;
	else
		vd=0;
	return vd;
}

//------------------------------------------------------------------------------------------------------------
//	��ȡ��CString�ֶ�ֵ
//	������v_sField���ֶ�
//	���أ�CString
//------------------------------------------------------------------------------------------------------------
CString CMyADOConn::GetField_String(CString v_sField)
{
	_variant_t Var;
	CString vs;
	Var=m_pRecordset->GetCollect((_variant_t)v_sField);
	if (Var.vt != VT_NULL)
		vs=Var.bstrVal;
	else
		vs="";
	return vs;
}

//------------------------------------------------------------------------------------------------------------
//	��ȡ��COleDateTime�ֶ�ֵ
//	������v_sField���ֶ�
//	���أ�COleDateTime
//------------------------------------------------------------------------------------------------------------
COleDateTime CMyADOConn::GetField_DataTime(CString v_sField)
{
	_variant_t Var;
	COleDateTime vd;
	Var=m_pRecordset->GetCollect((_variant_t)v_sField);
	if (Var.vt != VT_NULL)
		vd=Var.date;
	else
		vd=COleDateTime::GetCurrentTime();								//Ĭ�ϣ���ǰ����
	return vd;
}

//------------------------------------------------------------------------------------------------------------
//	ִ��SQL��䣬��ӦInsert Update delete���
//	������bstrSQL��SQL����
//------------------------------------------------------------------------------------------------------------
BOOL CMyADOConn::ExecuteSQL(_bstr_t bstrSQL)
{
	try
	{
		// Connection�����Execute����:(_bstr_t CommandText, VARIANT * RecordsAffected, long Options ) 
		// ����CommandText�������ִ���ͨ����SQL���
		// ����RecordsAffected�ǲ�����ɺ���Ӱ�������, 
		// ����Options��ʾCommandText�����ͣ�adCmdText-�ı����adCmdTable-������adCmdProc-�洢���̣�adCmdUnknown-δ֪
		m_pConnection->Execute(bstrSQL,NULL,adCmdText);
		return true;
	}
	catch(_com_error e)
	{
		CString v_sTemp=(LPCSTR)bstrSQL;
		throw ReCreatError(e,"�쳣�����ݿ�ִ�� ("+v_sTemp+") ");		//	�׳�������Ϣ���쳣
	}
}

//------------------------------------------------------------------------------------------------------------
//	�رռ�¼��������
//------------------------------------------------------------------------------------------------------------
void CMyADOConn::ExitConnect()
{
	try
	{
		if(m_bFlag)
		{
			if (m_pRecordset != NULL)									// �رռ�¼��������
				m_pRecordset->Close();
			m_pConnection->Close();
			::CoUninitialize();											// �ͷŻ���
		}
	}
	catch(_com_error e)
	{
		throw ReCreatError(e,"�쳣�����ݿ�Ͽ�");						//	�׳�������Ϣ���쳣
	}
}

//------------------------------------------------------------------------------------------------------------
//	���ò�ѯ��ִ��command�ĳ�ʱʱ��(s)
//	������CommandTimeout��ʱʱ�䣬��λ��
//------------------------------------------------------------------------------------------------------------
void CMyADOConn::SetCommandTimeout(int CommandTimeout)
{
	m_pConnection->CommandTimeout=CommandTimeout;						// ����CommandTimeout��ʱʱ��
}

//------------------------------------------------------------------------------------------------------------
//	��������com_error��Ϣ����ȷ�����ַ�����Ϣ���뵽�쳣��Ϣ��Description�
//	������original�쳣��Ϣ��msgȷ�����ַ�����Ϣ
//------------------------------------------------------------------------------------------------------------
_com_error CMyADOConn::ReCreatError(_com_error original, CString msg )
{
	CComQIPtr< ICreateErrorInfo > spCEI;								// ICreateErrorInfo
	CreateErrorInfo( &spCEI );											// ������ICreateErrorInfo
	spCEI->SetGUID( original.GUID() );									// ���ã����������GUID       
	spCEI->SetSource( original.Source() );								// ���ã����������Source
	spCEI->SetDescription(  T2BSTR((LPCTSTR)msg));						// ���ã����������Description
	// ���������ͬʱ�ṩ�˰����ļ�����ô�Ϳ��ԣ�
	spCEI->SetHelpContext( original.HelpContext());                     // ���ã������ļ��������
	spCEI->SetHelpFile( original.HelpFile() );                          // ���ã������ļ����ļ���
	//��ֵ
	CComQIPtr < IErrorInfo > spErrInfo = spCEI;							// �� ICreateErrorInfo ==��IErrorInfo 
	//����
	_com_error ex(original.Error(),spErrInfo,true);						// ���ɣ��µ�_com_error(��ȷ�����ַ�����Ϣ)
	return ex;															// ���أ�_com_error
}

//------------------------------------------------------------------------------------------------------------
// ����
//------------------------------------------------------------------------------------------------------------
