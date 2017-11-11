// MyADOConn.h: interface for the MyADOConn class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4146) 
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" named_guids rename("EOF","adoEOF"), rename("BOF","adoBOF") 
#pragma warning(default:4146) 
using namespace ADODB; 


#if !defined(AFX_ADOCONN_H__F3CFB98F_DA44_41A8_9D6F_91CB2A69BFD2__INCLUDED_)
#define AFX_ADOCONN_H__F3CFB98F_DA44_41A8_9D6F_91CB2A69BFD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyADOConn  
{
	//------------------------------------------------------------------------------------------------------------
	//	public������
	//------------------------------------------------------------------------------------------------------------
public:	
	_ConnectionPtr m_pConnection;										// �������������ӣ�
	_RecordsetPtr m_pRecordset;											// ���������ڷ��ؼ�¼����

	//------------------------------------------------------------------------------------------------------------
	//	public������
	//------------------------------------------------------------------------------------------------------------
public:	
	CMyADOConn();														// ���캯��
	virtual ~CMyADOConn();												// ��������

	void			OnInitADOConn(int v_iSelect,CString v_sDb0,CString v_sDb1,CString v_sDb2,CString v_sDb3);		// ��ʼ�����������ݿ�
	_RecordsetPtr&	GetRecordSet(_bstr_t bstrSQL);						// ִ�в�ѯ(select)�����ؼ�¼��
	BOOL			ExecuteSQL(_bstr_t bstrSQL);						// ִ��SQL���: Insert Update delete _variant_t
	void			SetCommandTimeout(int CommandTimeout);				// ���ò�ѯ��ִ��command�ĳ�ʱʱ��(s)
	void			ExitConnect();										// �رռ�¼��������

	int				GetField_Int(CString v_sField);						// ��ȡ��int�ֶ�ֵ
	double			GetField_Double(CString v_sField);					// ��ȡ��float�ֶ�ֵ
	bool			GetField_Bool(CString v_sField);					// ��ȡ��bool�ֶ�ֵ
	CString			GetField_String(CString v_sField);					// ��ȡ��CString�ֶ�ֵ
	COleDateTime	GetField_DataTime(CString v_sField);				// ��ȡ��DataTime�ֶ�ֵ


	//------------------------------------------------------------------------------------------------------------
	//	private������
	//------------------------------------------------------------------------------------------------------------
private:	
	_com_error ReCreatError(_com_error original, CString msg );			// ��������error
	BOOL m_bFlag;														// ���ӳɹ���־
};

#endif // !defined(AFX_ADOCONN_H__F3CFB98F_DA44_41A8_9D6F_91CB2A69BFD2__INCLUDED_)
