/* **********************************************************
* ���ܣ� 
*		1������ĳһ�е���ɫ
*		2������ͷ����
*		3��˫���ɱ༭ ���༭���޸������Σ�ͨ��OnEditEnd�����޸ģ�
* �޸����ڣ�
* 		2011.11.16----------���˷�
* **********************************************************/

#if !defined(AFX_MYLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_)
#define AFX_MYLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <Afxtempl.h>
//#include <windowsx.h>
#define WM_USER_EDIT_END  WM_USER + 1001


// �������б�ؼ��б༭����Ҫ�ı༭��--------------------------------------------------------------------------------------------------------------------------------

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

// �Լ���ListControl �ؼ�------------------------------------------------------------------------------------------------------------------------------
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
	void SetItemBkColor(DWORD iItem , COLORREF color);							//���ñ�����ɫ
	void SetItemTextColor(DWORD iItem , COLORREF color);						//���ñ�����ɫ
	void DeleteAllItems();														//���ListCtrl������Item
	void My_Sort(NMHDR * pNMHDR);												//ListCtrl����
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyListCtrl();              

	//�Զ��庯��--------------------------------------------------------------------------------------------------------
	bool	SetNumber(int v_iCol);												//�����У����ֱȽ�

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

	//�Զ��庯��--------------------------------------------------------------------------------------------------------
	static	int		CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);	// ����ص�������
	static	bool	IsNumber(  LPCTSTR pszText );													// �����ж�	
	static	bool	IsNumber_Flag(  LPCTSTR pszText );												// �����ж�	
	
private:	
	// �Զ������-------------------------------------------------------------------------------------------------------	
	BOOL    m_bAsc;																//�Ƿ�˳������
	int     m_nSortedCol;														//��ǰ�������
	int		m_iOldSortedCol;													//��һ�ε�������
	int		m_nItem;															//��
	int		m_nSub;																//��
	int		m_iCount;															//������
	bool	m_bNumber[200];														//���������ã� true�����֣�false���ַ���

	COLORREF crWindowText,crWindow;												//������ɫ��������ɫ
	CMap<DWORD , DWORD& , COLORREF , COLORREF&> MapItemColor;					//������ɫ
	CMap<DWORD , DWORD& , COLORREF , COLORREF&> MapItemBkColor;					//������ɫ

	CMyListCtrlEdit    m_edit;													//

	BOOL Key_Ctrl(int& nItem,int &nSub);										//
	BOOL Key_Shift(int& nItem,int& nSub);										//
	void ShowEdit(BOOL bShow,int nItem,int nIndex,CRect rc = CRect(0,0,0,0));	//
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_)
