#pragma once
#include "afxwin.h"


// CExit_Confirm �Ի���

class CExit_Confirm : public CDialog
{
	DECLARE_DYNAMIC(CExit_Confirm)

public:
	CExit_Confirm(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExit_Confirm();

// �Ի�������
	enum { IDD = IDD_EXIT_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	//------------------------------------------------------------------------------------------------------
	//	��������	
	//------------------------------------------------------------------------------------------------------
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();

	//------------------------------------------------------------------------------------------------------
	//	��������	
	//------------------------------------------------------------------------------------------------------
	CEdit	m_cPassword;												//����
};

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
