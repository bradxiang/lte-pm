// Sys_PARM.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server_Data.h"
#include "Sys_PARM.h"
#include "afxdialogex.h"
#include "Server_DataDlg.h"

// CSys_PARM �Ի���

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


// CSys_PARM ��Ϣ�������


BOOL CSys_PARM::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//�б��ʼ�����ͻ�����Ϣ
	m_List.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_List.InsertColumn(0,"���",LVCFMT_LEFT,40);				//
	m_List.InsertColumn(1,"����",LVCFMT_CENTER,940);			   //

	m_PmLTEMonth.SetWindowTextA(PmLTEMonth+"����");
	m_PmLTEDayFTP.SetWindowTextA(PmLTEDayFTP+"��");
	m_PmLTEFTPTime.SetWindowTextA(PmLTEFTPTime);
	m_PmLTETimeData.SetWindowTextA(PmLTETimeData);
	m_PmLTEDay.SetWindowTextA(PmLTEDay+"��");
	m_cEndMonitor.EnableWindow(false);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSys_PARM::AddListText(CString context)
{
	int		v_iIndex;												    //��ʱ����
	CString v_sTemp;													//��ʱ����
	v_iIndex = m_List.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_List.SetItemText(v_iIndex,0," "+v_sTemp);					       //���
	m_List.SetItemText(v_iIndex,1,context);						       //����
}

void CSys_PARM::DeleteList()
{
	int		nCount;										//��ʱ����
	nCount = m_List.GetItemCount();
	for(int vi=0;vi<nCount;vi++)											
	m_List.DeleteItem(0);								           

}

void CSys_PARM::OnBnClickedFileMonitor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"�����ļ�����",LVCFMT_CENTER,940);			   //
	iterator = file_queue_.begin();
	while(iterator != file_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}

void CSys_PARM::OnBnClickedEndMonitor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"Bulk insert���",LVCFMT_CENTER,940);			   //
	iterator = sql_queue_.begin();
	while(iterator != sql_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedBasepmqueue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"15���ӻ������ܽӿ���������",LVCFMT_CENTER,940);			   //
	iterator = basePm_queue_.begin();
	while(iterator != basePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedCalpmqueue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"15�������ȼ������ܽӿ���������",LVCFMT_CENTER,940);			   //
	iterator = calculatePm_queue_.begin();
	while(iterator != calculatePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedNetworkpmqueue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"ȫ�����ܽӿ���������",LVCFMT_CENTER,940);			   //
	iterator = networkPm_queue_.begin();
	while(iterator != networkPm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedRegionpmqueue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"�������ܽӿ���������",LVCFMT_CENTER,940);			   //
	iterator = regionPm_queue_.begin();
	while(iterator != regionPm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedHourbasepmqueue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"Сʱ���Ȼ������ܽӿ���������",LVCFMT_CENTER,940);			   //
	iterator = hour_basePm_queue_.begin();
	while(iterator != hour_basePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedHourcalpmqueue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"Сʱ���ȼ������ܽӿ���������",LVCFMT_CENTER,940);			   //
	iterator = hour_calculatePm_queue_.begin();
	while(iterator != hour_calculatePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedHwpmqueue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"��Ϊ�ӿ���������",LVCFMT_CENTER,940);			   //
	iterator = hwPm_queue_.begin();
	while(iterator != hwPm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}


void CSys_PARM::OnBnClickedZtepmqueue()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	m_List.InsertColumn(1,"���˽ӿ���������",LVCFMT_CENTER,940);			   //
	iterator = ztePm_queue_.begin();
	while(iterator != ztePm_queue_.end())
	{
		AddListText(*iterator);
		iterator++;
	}
}
