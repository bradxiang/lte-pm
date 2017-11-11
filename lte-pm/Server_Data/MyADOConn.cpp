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
//	初始化数据库连接
//	输入参数:  v_iSelect(数据库选择)       v_sDb0            v_sDb1      v_sDb2     v_sDb3
//				 0:   SQL数据库       数据库服务器地址/名称  数据库名称  用户名称   密码
//               1:   ACCESS数据库                           数据库名称  用户名称   密码
//               2:   ODBC数据源            数据库源                     用户名称   密码
//	输出结果:  连接数据库成功，获得Connection对象
//------------------------------------------------------------------------------------------------------------
void  CMyADOConn::OnInitADOConn(int v_iSelect,CString v_sDb0,CString v_sDb1,CString v_sDb2,CString v_sDb3)
{
	_bstr_t strConnect;
	::CoInitialize(NULL);												// 初始化OLE/COM库环境 
	try
	{
		m_pConnection.CreateInstance("ADODB.Connection");				// 创建Connection对象
		switch(v_iSelect)												// 根据数据库类型，设置连接字符串，必须是BSTR型或者_bstr_t类型
		{
		case 0:															// SQL 数据库
			strConnect = "Provider=SQLOLEDB;Server="+v_sDb0+";Database="+v_sDb1+";uid="+v_sDb2+";pwd="+v_sDb3;	
			break;
		case 1:															// ACCESS 数据库
			strConnect = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source="+v_sDb1+";jet oledb:database password="+v_sDb3;	
			break;
		case 2:															// ODBC 访问方式
			strConnect = "Data Source="+v_sDb0+";uid="+v_sDb2+";pwd="+v_sDb3;	
			break;
		}
		m_pConnection->Open(strConnect,"","",adModeUnknown);			// 连接数据库
		m_bFlag = TRUE;
	}
	catch(_com_error e)													// 捕捉异常
	{
		m_bFlag = FALSE;
		throw ReCreatError(e,"异常：数据库连接 ("+v_sDb1+") ");			// 抛出设置信息的异常	
	}
}

//------------------------------------------------------------------------------------------------------------
//	执行查询,返回记录集: 对应select语句
//	参数：bstrSQL：SQL命令
//------------------------------------------------------------------------------------------------------------
_RecordsetPtr&  CMyADOConn::GetRecordSet(_bstr_t bstrSQL)
{
	try
	{
		m_pRecordset.CreateInstance(__uuidof(Recordset));				// 创建记录集对象
		m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
		// 取得表中的记录
	}
	catch(_com_error e)													// 捕捉异常
	{
		m_bFlag = FALSE;
		CString v_sTemp=(LPCSTR)bstrSQL;
		throw ReCreatError(e,"异常：数据库查询 ("+v_sTemp+") ");		//	抛出设置信息的异常
	}
	return m_pRecordset;												// 返回记录集
}

//------------------------------------------------------------------------------------------------------------
//	获取：int字段值
//	参数：v_sField：字段
//	返回：int
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
//	获取：bool字段值
//	参数：v_sField：字段
//	返回：bool
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
//	获取：float字段值
//	参数：v_sField：字段
//	返回：double
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
//	获取：CString字段值
//	参数：v_sField：字段
//	返回：CString
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
//	获取：COleDateTime字段值
//	参数：v_sField：字段
//	返回：COleDateTime
//------------------------------------------------------------------------------------------------------------
COleDateTime CMyADOConn::GetField_DataTime(CString v_sField)
{
	_variant_t Var;
	COleDateTime vd;
	Var=m_pRecordset->GetCollect((_variant_t)v_sField);
	if (Var.vt != VT_NULL)
		vd=Var.date;
	else
		vd=COleDateTime::GetCurrentTime();								//默认：当前日期
	return vd;
}

//------------------------------------------------------------------------------------------------------------
//	执行SQL语句，对应Insert Update delete语句
//	参数：bstrSQL：SQL命令
//------------------------------------------------------------------------------------------------------------
BOOL CMyADOConn::ExecuteSQL(_bstr_t bstrSQL)
{
	try
	{
		// Connection对象的Execute方法:(_bstr_t CommandText, VARIANT * RecordsAffected, long Options ) 
		// 其中CommandText是命令字串，通常是SQL命令。
		// 参数RecordsAffected是操作完成后所影响的行数, 
		// 参数Options表示CommandText的类型：adCmdText-文本命令；adCmdTable-表名；adCmdProc-存储过程；adCmdUnknown-未知
		m_pConnection->Execute(bstrSQL,NULL,adCmdText);
		return true;
	}
	catch(_com_error e)
	{
		CString v_sTemp=(LPCSTR)bstrSQL;
		throw ReCreatError(e,"异常：数据库执行 ("+v_sTemp+") ");		//	抛出设置信息的异常
	}
}

//------------------------------------------------------------------------------------------------------------
//	关闭记录集和连接
//------------------------------------------------------------------------------------------------------------
void CMyADOConn::ExitConnect()
{
	try
	{
		if(m_bFlag)
		{
			if (m_pRecordset != NULL)									// 关闭记录集和连接
				m_pRecordset->Close();
			m_pConnection->Close();
			::CoUninitialize();											// 释放环境
		}
	}
	catch(_com_error e)
	{
		throw ReCreatError(e,"异常：数据库断开");						//	抛出设置信息的异常
	}
}

//------------------------------------------------------------------------------------------------------------
//	设置查询、执行command的超时时间(s)
//	参数：CommandTimeout超时时间，单位秒
//------------------------------------------------------------------------------------------------------------
void CMyADOConn::SetCommandTimeout(int CommandTimeout)
{
	m_pConnection->CommandTimeout=CommandTimeout;						// 设置CommandTimeout超时时间
}

//------------------------------------------------------------------------------------------------------------
//	重新配置com_error信息：把确定的字符串信息加入到异常信息的Description里；
//	参数：original异常信息，msg确定的字符串信息
//------------------------------------------------------------------------------------------------------------
_com_error CMyADOConn::ReCreatError(_com_error original, CString msg )
{
	CComQIPtr< ICreateErrorInfo > spCEI;								// ICreateErrorInfo
	CreateErrorInfo( &spCEI );											// 产生：ICreateErrorInfo
	spCEI->SetGUID( original.GUID() );									// 设置：发生错误的GUID       
	spCEI->SetSource( original.Source() );								// 设置：发生错误的Source
	spCEI->SetDescription(  T2BSTR((LPCTSTR)msg));						// 设置：发生错误的Description
	// 如果你的组件同时提供了帮助文件，那么就可以：
	spCEI->SetHelpContext( original.HelpContext());                     // 设置：帮助文件的主题号
	spCEI->SetHelpFile( original.HelpFile() );                          // 设置：帮助文件的文件名
	//赋值
	CComQIPtr < IErrorInfo > spErrInfo = spCEI;							// 把 ICreateErrorInfo ==〉IErrorInfo 
	//生成
	_com_error ex(original.Error(),spErrInfo,true);						// 生成：新的_com_error(含确定的字符串信息)
	return ex;															// 返回：_com_error
}

//------------------------------------------------------------------------------------------------------------
// 结束
//------------------------------------------------------------------------------------------------------------
