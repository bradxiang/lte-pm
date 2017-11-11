/* **********************************************************
* 功能： 
*		1、设置某一行的颜色
*		2、按表头排序
*		3、双击可编辑 （编辑后修改已屏蔽，通过OnEditEnd函数修改）
* 修改日期：
* 		2011.11.16----------王克锋
* **********************************************************/

#if !defined(AFX_MYLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_)
#define AFX_MYLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <Afxtempl.h>
//#include <windowsx.h>
#define WM_USER_EDIT_END  WM_USER + 1001


// 下面是列表控件中编辑所需要的编辑框--------------------------------------------------------------------------------------------------------------------------------

// ListCtrlEdit.h : header file
//
class CMyListCtrlEdit : public CEdit
{
// Construction
public:
	CMyListCtrlEdit();

// Attributes
public:
	void  SetCtrlData(DWORD dwData);
	DWORD GetCtrlData();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEdit)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyListCtrlEdit();
	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DWORD m_dwData;
	BOOL m_bExchange;
};

// 自己的ListControl 控件------------------------------------------------------------------------------------------------------------------------------
// MyListCtrl.h : header file
class CMyListCtrl : public CListCtrl
{
// Construction
public:
	CMyListCtrl();

// Attributes
public:

// Operations
public:
	void SetItemBkColor(DWORD iItem , COLORREF color);							//设置背景颜色
	void SetItemTextColor(DWORD iItem , COLORREF color);						//设置背景颜色
	void DeleteAllItems();														//清除ListCtrl的所有Item
	void My_Sort(NMHDR * pNMHDR);												//ListCtrl排序
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyListCtrl();              

	//自定义函数--------------------------------------------------------------------------------------------------------
	bool	SetNumber(int v_iCol);												//设置列：数字比较

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListCtrl)
	afx_msg void	OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void	OnInsertitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnParentNotify(UINT message, LPARAM lParam);
	//}}AFX_MSG
	afx_msg LRESULT OnEditEnd(WPARAM ,LPARAM lParam = FALSE);
	afx_msg void	OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

	//自定义函数--------------------------------------------------------------------------------------------------------
	static	int		CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);	// 排序回调函函数
	static	bool	IsNumber(  LPCTSTR pszText );													// 数字判断	
	static	bool	IsNumber_Flag(  LPCTSTR pszText );												// 数字判断	
	
private:	
	// 自定义变量-------------------------------------------------------------------------------------------------------	
	BOOL    m_bAsc;																//是否顺序排序
	int     m_nSortedCol;														//当前排序的列
	int		m_iOldSortedCol;													//上一次的排序列
	int		m_nItem;															//行
	int		m_nSub;																//列
	int		m_iCount;															//计算器
	bool	m_bNumber[200];														//数字列设置： true：数字；false：字符；

	COLORREF crWindowText,crWindow;												//文字颜色、背景颜色
	CMap<DWORD , DWORD& , COLORREF , COLORREF&> MapItemColor;					//文字颜色
	CMap<DWORD , DWORD& , COLORREF , COLORREF&> MapItemBkColor;					//背景颜色

	CMyListCtrlEdit    m_edit;													//

	BOOL Key_Ctrl(int& nItem,int &nSub);										//
	BOOL Key_Shift(int& nItem,int& nSub);										//
	void ShowEdit(BOOL bShow,int nItem,int nIndex,CRect rc = CRect(0,0,0,0));	//
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_)
