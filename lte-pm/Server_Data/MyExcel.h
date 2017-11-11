// MyExcel.h: interface for the CMyExcel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYEXCEL_H__3D77D0A0_54FA_4786_B890_1263111D8922__INCLUDED_)
#define AFX_MYEXCEL_H__3D77D0A0_54FA_4786_B890_1263111D8922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//***********************************************************************************************************
class CMyExcel  
{
public:

	CMyExcel();															//构造函数
	virtual	~CMyExcel();												//析构函数

	//自定义函数-----------------------------------------------------------------------------------
	void	Show(CListCtrl *v_cList);									//输出CSV文件，并打开文件
	void	Show_Name(CListCtrl *v_cList,CString v_sFile_Name,bool v_bFile_Open);	//输出CSV文件
	bool	Get_CSV(CString v_sFile,CListCtrl *v_cList,bool v_bHead);	//导入CSV文件
	bool	do_events();												//

private:
	int m_iRow;

};

#endif // !defined(AFX_MYEXCEL_H__3D77D0A0_54FA_4786_B890_1263111D8922__INCLUDED_)
