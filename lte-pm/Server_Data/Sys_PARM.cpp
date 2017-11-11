// Sys_PARM.cpp : 实现文件
//

#include "stdafx.h"
#include "Server_Data.h"
#include "Sys_PARM.h"
#include "afxdialogex.h"
#include "Server_DataDlg.h"

// CSys_PARM 对话框

IMPLEMENT_DYNAMIC(CSys_PARM, CDialogEx)

CSys_PARM::CSys_PARM(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSys_PARM::IDD, pParent)
{

}

CSys_PARM::~CSys_PARM()
{
	file_queue_.clear();
	flag =false;
	Sleep(1000);
}

void CSys_PARM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_EDIT1, m_PmLTEMonth);
	DDX_Control(pDX, IDC_EDIT2, m_PmLTEDayFTP);
	DDX_Control(pDX, IDC_EDIT4, m_PmLTEFTPTime);
	DDX_Control(pDX, IDC_EDIT5, m_PmLTETimeData);
	DDX_Control(pDX, IDC_EDIT3, m_PmLTEDay);
	DDX_Control(pDX, IDC_MONITOR, m_cFileMonitor);
	DDX_Control(pDX, IDC_MONITOR2, m_cEndMonitor);
	DDX_Control(pDX, IDC_SQLQUEUE, m_cSQLMonitor);
	DDX_Control(pDX, IDC_BASEPMQUEUE, m_cBasePmMonitor);
	DDX_Control(pDX, IDC_CALPMQUEUE, m_cCalculateMonitor);
	DDX_Control(pDX, IDC_NETWORKPMQUEUE, m_cNetworkMonitor);
	DDX_Control(pDX, IDC_REGIONPMQUEUE, m_cRegionMonitor);
	DDX_Control(pDX, IDC_HOURBASEPMQUEUE, m_cHourBasePmMonitor);
	DDX_Control(pDX, IDC_HOURCALPMQUEUE2, m_cHourCalculatePmMonitor);
	DDX_Control(pDX, IDC_HWPMQUEUE, m_cHWMonitor);
	DDX_Control(pDX, IDC_ZTEPMQUEUE, m_cZTEMonitor);
}


BEGIN_MESSAGE_MAP(CSys_PARM, CDialogEx)
	ON_BN_CLICKED(IDC_MONITOR, &CSys_PARM::OnBnClickedFileMonitor)
	ON_BN_CLICKED(IDC_MONITOR2, &CSys_PARM::OnBnClickedEndMonitor)
	ON_BN_CLICKED(IDC_SQLQUEUE, &CSys_PARM::OnBnClickedSqlqueue)
	ON_BN_CLICKED(IDC_BASEPMQUEUE, &CSys_PARM::OnBnClickedBasepmqueue)
	ON_BN_CLICKED(IDC_CALPMQUEUE, &CSys_PARM::OnBnClickedCalpmqueue)
	ON_BN_CLICKED(IDC_NETWORKPMQUEUE, &CSys_PARM::OnBnClickedNetworkpmqueue)
	ON_BN_CLICKED(IDC_REGIONPMQUEUE, &CSys_PARM::OnBnClickedRegionpmqueue)
	ON_BN_CLICKED(IDC_HOURBASEPMQUEUE, &CSys_PARM::OnBnClickedHourbasepmqueue)
	ON_BN_CLICKED(IDC_HOURCALPMQUEUE2, &CSys_PARM::OnBnClickedHourcalpmqueue)
	ON_BN_CLICKED(IDC_HWPMQUEUE, &CSys_PARM::OnBnClickedHwpmqueue)
	ON_BN_CLICKED(IDC_ZTEPMQUEUE, &CSys_PARM::OnBnClickedZtepmqueue)
END_MESSAGE_MAP()


// CSys_PARM 消息处理程序


BOOL CSys_PARM::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO: 在此添加额外的初始化代码
	//列表初始化：客户端信息
	m_List.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_List.InsertColumn(0,"序号",LVCFMT_LEFT,40);				//
	m_List.InsertColumn(1,"名称",LVCFMT_CENTER,940);			   //

	m_PmLTEMonth.SetWindowTextA(PmLTEMonth+"个月");
	m_PmLTEDayFTP.SetWindowTextA(PmLTEDayFTP+"天");
	m_PmLTEFTPTime.SetWindowTextA(PmLTEFTPTime);
	m_PmLTETimeData.SetWindowTextA(PmLTETimeData);
	m_PmLTEDay.SetWindowTextA(PmLTEDay+"天");
	m_cEndMonitor.EnableWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSys_PARM::AddListText(CString context)
{
	int		v_iIndex;												    //临时变量
	CString v_sTemp;													//临时变量
	v_iIndex = m_List.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_List.SetItemText(v_iIndex,0," "+v_sTemp);					       //序号
	m_List.SetItemText(v_iIndex,1,context);						       //内容
}

void CSys_PARM::DeleteList()
{
	int		nCount;										//临时变量
	nCount = m_List.GetItemCount();
	for(int vi=0;vi<nCount;vi++)											
	m_List.DeleteItem(0);								           

}

void CSys_PARM::OnBnClickedFileMonitor()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"处理文件名称",LVCFMT_CENTER,940);			   //
	iterator = file_queue_.begin();
	while(iterator != file_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}

void CSys_PARM::OnBnClickedEndMonitor()
{
	// TODO: 在此添加控件通知处理程序代码
	m_cFileMonitor.EnableWindow(true);
	m_cSQLMonitor.EnableWindow(true);
	m_cBasePmMonitor.EnableWindow(true);
	m_cCalculateMonitor.EnableWindow(true);
	m_cNetworkMonitor.EnableWindow(true);
	m_cRegionMonitor.EnableWindow(true);
	m_cHourBasePmMonitor.EnableWindow(true);
	m_cHourCalculatePmMonitor.EnableWindow(true);
	m_cHWMonitor.EnableWindow(true);
	m_cZTEMonitor.EnableWindow(true);
	m_cEndMonitor.EnableWindow(false);
	DeleteList();
}


void CSys_PARM::OnBnClickedSqlqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"Bulk insert语句",LVCFMT_CENTER,940);			   //
	iterator = sql_queue_.begin();
	while(iterator != sql_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedBasepmqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"15分钟基本性能接口数据名称",LVCFMT_CENTER,940);			   //
	iterator = basePm_queue_.begin();
	while(iterator != basePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedCalpmqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"15分钟粒度计算性能接口数据名称",LVCFMT_CENTER,940);			   //
	iterator = calculatePm_queue_.begin();
	while(iterator != calculatePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedNetworkpmqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"全网性能接口数据名称",LVCFMT_CENTER,940);			   //
	iterator = networkPm_queue_.begin();
	while(iterator != networkPm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedRegionpmqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"区域性能接口数据名称",LVCFMT_CENTER,940);			   //
	iterator = regionPm_queue_.begin();
	while(iterator != regionPm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedHourbasepmqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"小时粒度基本性能接口数据名称",LVCFMT_CENTER,940);			   //
	iterator = hour_basePm_queue_.begin();
	while(iterator != hour_basePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedHourcalpmqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"小时粒度计算性能接口数据名称",LVCFMT_CENTER,940);			   //
	iterator = hour_calculatePm_queue_.begin();
	while(iterator != hour_calculatePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedHwpmqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"华为接口数据名称",LVCFMT_CENTER,940);			   //
	iterator = hwPm_queue_.begin();
	while(iterator != hwPm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedZtepmqueue()
{
	// TODO: 在此添加控件通知处理程序代码
	list<CString>::iterator iterator;
	m_cFileMonitor.EnableWindow(false);
	m_cSQLMonitor.EnableWindow(false);
	m_cBasePmMonitor.EnableWindow(false);
	m_cCalculateMonitor.EnableWindow(false);
	m_cNetworkMonitor.EnableWindow(false);
	m_cRegionMonitor.EnableWindow(false);
	m_cHourBasePmMonitor.EnableWindow(false);
	m_cHourCalculatePmMonitor.EnableWindow(false);
	m_cHWMonitor.EnableWindow(false);
	m_cZTEMonitor.EnableWindow(false);
	m_cEndMonitor.EnableWindow(true);
	//DeleteList();
	m_List.DeleteColumn(1);
	m_List.InsertColumn(1,"中兴接口数据名称",LVCFMT_CENTER,940);			   //
	iterator = ztePm_queue_.begin();
	while(iterator != ztePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}
