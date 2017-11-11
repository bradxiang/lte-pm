#pragma once


// CCOM_About 对话框

class CCOM_About : public CDialog
{
	DECLARE_DYNAMIC(CCOM_About)

public:
	CCOM_About(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCOM_About();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedAbout();
};
