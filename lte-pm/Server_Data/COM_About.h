#pragma once


// CCOM_About �Ի���

class CCOM_About : public CDialog
{
	DECLARE_DYNAMIC(CCOM_About)

public:
	CCOM_About(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCOM_About();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedAbout();
};
