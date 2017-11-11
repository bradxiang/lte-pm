#pragma once
#include "mylistctrl.h"
#include "afxwin.h"


// CSys_PARM 对话框

class CSys_PARM : public CDialogEx
{
	DECLARE_DYNAMIC(CSys_PARM)

public:
	CSys_PARM(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSys_PARM();

// 对话框数据
	enum { IDD = IDD_SYSTEM_PARM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFileMonitor();
	afx_msg void OnBnClickedEndMonitor();
	virtual BOOL OnInitDialog();
	void AddListText(CString context);
	void DeleteList();
public:
	CMyListCtrl m_List;

	list<CString> file_queue_;
	list<CString> sql_queue_;
	list<CString> basePm_queue_;
	list<CString> calculatePm_queue_;
	list<CString> networkPm_queue_;
	list<CString> hwPm_queue_;
	list<CString> ztePm_queue_;
	list<CString> regionPm_queue_;
	list<CString> hour_basePm_queue_;
	list<CString> hour_calculatePm_queue_;

	BOOL     flag;
	CString PmLTEDayFTP;
	CString PmLTEFTPTime;
	CString PmLTEMonth;
	CString PmLTETimeData;
	CString PmLTEDay;
	
	CEdit m_PmLTEMonth;
	CEdit m_PmLTEDayFTP;
	CEdit m_PmLTEFTPTime;
	CEdit m_PmLTETimeData;
	CEdit m_PmLTEDay;

	CButton m_cFileMonitor;
	CButton m_cEndMonitor;
	CButton m_cSQLMonitor;
	CButton m_cBasePmMonitor;
	CButton m_cCalculateMonitor;
	CButton m_cNetworkMonitor;
	CButton m_cRegionMonitor;
	CButton m_cHourCalculatePmMonitor;
	CButton m_cHourBasePmMonitor;
	CButton m_cHWMonitor;
	CButton m_cZTEMonitor;
	afx_msg void OnBnClickedSqlqueue();
	afx_msg void OnBnClickedBasepmqueue();
	afx_msg void OnBnClickedCalpmqueue();
	afx_msg void OnBnClickedNetworkpmqueue();
	afx_msg void OnBnClickedRegionpmqueue();
	afx_msg void OnBnClickedHourbasepmqueue();
	afx_msg void OnBnClickedHourcalpmqueue();
	afx_msg void OnBnClickedHwpmqueue();
	afx_msg void OnBnClickedZtepmqueue();

};
