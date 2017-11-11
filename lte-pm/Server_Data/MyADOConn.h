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
	//	public：变量
	//------------------------------------------------------------------------------------------------------------
public:	
	_ConnectionPtr m_pConnection;										// 变量：用于连接；
	_RecordsetPtr m_pRecordset;											// 变量：用于返回记录集；

	//------------------------------------------------------------------------------------------------------------
	//	public：函数
	//------------------------------------------------------------------------------------------------------------
public:	
	CMyADOConn();														// 构造函数
	virtual ~CMyADOConn();												// 析构函数

	void			OnInitADOConn(int v_iSelect,CString v_sDb0,CString v_sDb1,CString v_sDb2,CString v_sDb3);		// 初始化—连接数据库
	_RecordsetPtr&	GetRecordSet(_bstr_t bstrSQL);						// 执行查询(select)，返回记录集
	BOOL			ExecuteSQL(_bstr_t bstrSQL);						// 执行SQL语句: Insert Update delete _variant_t
	void			SetCommandTimeout(int CommandTimeout);				// 设置查询、执行command的超时时间(s)
	void			ExitConnect();										// 关闭记录集和连接

	int				GetField_Int(CString v_sField);						// 获取：int字段值
	double			GetField_Double(CString v_sField);					// 获取：float字段值
	bool			GetField_Bool(CString v_sField);					// 获取：bool字段值
	CString			GetField_String(CString v_sField);					// 获取：CString字段值
	COleDateTime	GetField_DataTime(CString v_sField);				// 获取：DataTime字段值


	//------------------------------------------------------------------------------------------------------------
	//	private：函数
	//------------------------------------------------------------------------------------------------------------
private:	
	_com_error ReCreatError(_com_error original, CString msg );			// 重新生成error
	BOOL m_bFlag;														// 连接成功标志
};

#endif // !defined(AFX_ADOCONN_H__F3CFB98F_DA44_41A8_9D6F_91CB2A69BFD2__INCLUDED_)
