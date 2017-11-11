#pragma once
#include "afxwin.h"


// CExit_Confirm 对话框

class CExit_Confirm : public CDialog
{
	DECLARE_DYNAMIC(CExit_Confirm)

public:
	CExit_Confirm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExit_Confirm();

// 对话框数据
	enum { IDD = IDD_EXIT_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//------------------------------------------------------------------------------------------------------
	//	函数定义	
	//------------------------------------------------------------------------------------------------------
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();

	//------------------------------------------------------------------------------------------------------
	//	变量定义	
	//------------------------------------------------------------------------------------------------------
	CEdit	m_cPassword;												//密码
};

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
