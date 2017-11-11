
// Server_DataDlg.cpp : ʵ���ļ�
#include "stdafx.h"
#include "Server_Data.h"
#include "Server_DataDlg.h"
#include "Exit_Confirm.h"												//�˳�ȷ��
#include "COM_About.h"
#include "MyTran.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CServer_DataDlg �Ի���
//------------------------------------------------------------------------------------------------------			
//	�������ܣ����캯��
//------------------------------------------------------------------------------------------------------
CServer_DataDlg::CServer_DataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServer_DataDlg::IDD, pParent),TelnetManager((LPVOID)this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	v_sServer_Socket=NULL;												//�ɼ���SOCKET
	v_iServer_Sum=0;													//�ɼ��˼�����
	v_bCommand_Flag=0;													//��λ��ָ��ͱ��
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���������
//------------------------------------------------------------------------------------------------------
CServer_DataDlg::~CServer_DataDlg()
{
	v_lConnect.clear();													//�������
	v_lCommand.clear();													//�������
	if(v_sServer_Socket!=NULL)											//�ɼ���SOCKET��Ч������
		closesocket(v_sServer_Socket);									//�رղɼ���SOCKET
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���Ļ����
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECORD4, m_cList_Bug);
	DDX_Control(pDX, IDC_LIST_RECORD3, m_cList_Log);
	DDX_Control(pDX, IDOK, m_cStart);
	DDX_Control(pDX, IDOK2, m_cStop);
	DDX_Control(pDX, IDC_LIST4, m_cList_Client);
	DDX_Control(pDX, IDC_STATIC1, m_cState);
	DDX_Control(pDX, IDC_CHECK1, m_cLog);
	DDX_Control(pDX, IDC_STATIC2, m_cPort);
	DDX_Control(pDX, IDC_LIST5, m_cList_Command);
	DDX_Control(pDX, IDC_BUTTON4, m_cServer);
	DDX_Control(pDX, IDC_STATIC3, m_cAAA);
	DDX_Control(pDX, IDC_STATIC_FILECOUNT, m_cFileCount);
	DDX_Control(pDX, IDC_STATIC_PRCTIME, m_cGTime);
	DDX_Control(pDX, IDC_STATIC_FILECOUNT2, m_cTest);
	DDX_Control(pDX, IDC_STATIC4, m_cTest_File);
	DDX_Control(pDX, IDC_CHECK_MONITOR, m_cinformation_monitor);
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���Ϣӳ��
//------------------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CServer_DataDlg, CDialog)	
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CServer_DataDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDOK2, &CServer_DataDlg::OnBnClickedOk2)
	ON_BN_CLICKED(IDC_FILE2, &CServer_DataDlg::OnBnClickedFile2)
	ON_BN_CLICKED(IDC_FILE3, &CServer_DataDlg::OnBnClickedFile3)
	ON_BN_CLICKED(IDC_OUTPUT2, &CServer_DataDlg::OnBnClickedOutput2)
	ON_BN_CLICKED(IDC_BUTTON1, &CServer_DataDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_SYSTEM_PARM, &CServer_DataDlg::OnBnClickedSystemParm)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()
// CServer_DataDlg ��Ϣ�������

//------------------------------------------------------------------------------------------------------			
//	�������ܣ������Ի��������С����ť������Ҫ����Ĵ��������Ƹ�ͼ�ꡣ
//			  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó����⽫�ɿ���Զ���ɡ�
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ����û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//------------------------------------------------------------------------------------------------------
HCURSOR CServer_DataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ��Ի�����ʼ��
//------------------------------------------------------------------------------------------------------
BOOL CServer_DataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	CString v_sTemp,sValue,sKey;
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);												//���ô�ͼ��
	SetIcon(m_hIcon, FALSE);											//����Сͼ��
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//�б��ʼ�����ͻ�����Ϣ
	m_cList_Client.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_cList_Client.InsertColumn(0,"���",LVCFMT_LEFT,40);				//
	m_cList_Client.InsertColumn(1,"ʱ��",LVCFMT_CENTER,120);			//
	m_cList_Client.InsertColumn(2,"IP",LVCFMT_LEFT,120);				//
	m_cList_Client.InsertColumn(3,"SOCKET",LVCFMT_LEFT,100);			//
	m_cList_Client.InsertColumn(4,"����",LVCFMT_LEFT,100);				//

	//�б��ʼ�����쳣��Ϣ
	m_cList_Bug.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_cList_Bug.InsertColumn(0,"���",LVCFMT_LEFT,40);					//
	m_cList_Bug.InsertColumn(1,"ʱ��",LVCFMT_CENTER,120);				//
	m_cList_Bug.InsertColumn(2,"�쳣����",LVCFMT_LEFT,120);				//
	m_cList_Bug.InsertColumn(3,"�쳣��Ϣ",LVCFMT_LEFT,300);				//

	//�б��ʼ�����ļ���Ϣ
	m_cList_Command.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_cList_Command.InsertColumn(0,"���",LVCFMT_LEFT,40);				//
	m_cList_Command.InsertColumn(1,"ʱ��",LVCFMT_CENTER,125);			//
	m_cList_Command.InsertColumn(2,"�ļ���Ϣ",LVCFMT_LEFT,360);			//
	m_cList_Command.InsertColumn(3,"�ļ�������Ϣ",LVCFMT_LEFT,600);		//
	
	//�б��ʼ������־�ļ�������Ϣ
	m_cList_Log.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_cList_Log.InsertColumn(0,"���",LVCFMT_LEFT,40);
	m_cList_Log.InsertColumn(1,"ʱ��",LVCFMT_CENTER,140);
	m_cList_Log.InsertColumn(2,"��ʱ",LVCFMT_CENTER,80);
	m_cList_Log.InsertColumn(3,"�ļ���Ϣ",LVCFMT_LEFT,360);
	m_cList_Log.InsertColumn(4,"�ļ�������Ϣ",LVCFMT_LEFT,600);

	//�ִ�
	CRect	v_cRect;													//����
	CWnd*	pWnd = GetDlgItem(IDC_STATIC_SLIT_PANE);					//��ã�ָ��
	pWnd->GetWindowRect(&v_cRect);										//��ã��ߴ�
	ScreenToClient(&v_cRect);											//����任
	pWnd->DestroyWindow();												//���٣�ָ��
	//	this creates pane containing list control on the top and edit on the bottom
	m_SplitterPane_Left.Create(										//��ֱ�ߣ���
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN|SS_HORIZ, 		 
		this,					// the parent of the splitter pane
		&m_cList_Client,		// Top pane
		&m_cList_Log,			// Bottom pane
		IDC_Report_VERT1_Time,	// this ID is used for saving/restoring splitter position and therefore it must be unique !
		v_cRect					// dimensions of the splitter pane
		);
	m_SplitterPane_Left.m_bVertSplitter_Parent=true;					//��ֱ�ָ���Ϊ����
	m_SplitterPane_Right.Create(										//ˮƽ��
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN|SS_HORIZ,		 
		this,					// the parent of the splitter pane
		&m_cList_Command,		// Left pane
		&m_cList_Bug,			// Right pane
		IDC_Report_VERT2_Time,	// this ID is used for saving/restoring splitter position and it must be unique !
		v_cRect					// dimensions of the splitter pane
		);
	m_SplitterPane_Right.m_bVertSplitter_Parent=true;					//��ֱ�ָ���Ϊ����
	m_SplitterPane_Main.Create(											//ˮƽ��
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN|SS_VERT,		 
		this,					// the parent of the splitter pane
		&m_SplitterPane_Left,	// Left pane
		&m_SplitterPane_Right,	// Right pane
		IDC_MO_HORIZ_Time,		// this ID is used for saving/restoring splitter position and it must be unique !
		v_cRect					// dimensions of the splitter pane
		);

	//��ʱ�� 
	SetTimer(1,1000,NULL);												//��ʱ��1s
	SetTimer(2,60*1000,NULL);											//��ʱ��1min

	//������ʼ��
	v_iThread_Exit=0;													//�߳�״̬��δ����
	v_bDlg_Max=true;													//�����Ч
	m_cStart.EnableWindow(false);										//��Ч
	m_cStop.EnableWindow(false);										//��Ч
	m_cServer.EnableWindow(false);										//��Ч
	m_cinformation_monitor.SetCheck(0);

	//�������ļ�Ŀ¼
	CFileFind	finder;
	CString v_sPath_Data=theApp.m_sPath+"\\DATA";						//�ļ�Ŀ¼
	if (!finder.FindFile(v_sPath_Data))									//���ڣ�
	{
		if (!::CreateDirectory(v_sPath_Data, NULL))						//�����ڣ�����Ŀ¼
		{
			MessageBox("Ŀ¼����ʧ�ܣ�"+v_sPath_Data);					//ʧ��
			return false;
		}
	}

	//�����ź���[���ֻ������һ��]
	::SetProp(m_hWnd,theApp.m_sAppName,(HANDLE)1);						//�����ź��� [��ָ�����ڵ����Ա�������һ������]

	//��С������
	ShowWindow( SW_SHOWMINIMIZED );										//��С������

	//��ʼ��
	v_bCommamd=false;													//��λ��ָ�ʱ��־
	v_bClient=false;													//��λ���ͻ��˴����־
	v_bTj=false;														//��λ��ͳ�Ʊ�־
	m_cStart.EnableWindow(true);										//��Ч
	v_sTemp.Format("%d",theApp.a_iAlarmNotifyPort);                     //��ʾ�˿�
	m_cPort.SetWindowText(v_sTemp);										//��ʾ�������˿�
	global_control_ = false;
	return TRUE;														// ���ǽ��������õ��ؼ������򷵻� TRUE
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���������ߴ�
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: �ڴ˴������Ϣ����������
	if ( m_SplitterPane_Main.GetSafeHwnd() ){							//�ִ��ߴ����
		CRect v_cRect;
		GetClientRect( &v_cRect );
		v_cRect.DeflateRect(10,10);
		v_cRect.top+=45;
		m_SplitterPane_Main.MoveWindow(&v_cRect);
	}
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ��رճ���
//------------------------------------------------------------------------------------------------------			
void CServer_DataDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//�߳����У�����
	if (v_iThread_Exit==1)
	{
		//���ã�ǿ���˳�
		if(MessageBox("�Ƿ��˳��澯��ȡ  ��","�˳�ȷ��",MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
			v_iThread_Exit=2;											//�߳�״̬��ǿ���˳��߳�
	}
	else
	{
		//ϵͳ�˳�ȷ��
		CExit_Confirm	v_cDlg;											//ʵ����
		CDialog::OnOK();											//�˳�
		//�Ƴ��ź���[���ֻ������һ��]
		::RemoveProp(m_hWnd,theApp.m_sAppName);						//�Ƴ��ź���
	}
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���ʱ��
//	�������̣���1s��������󻯡���1min�����߿ͻ���ɾ����
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	COleDateTime v_tDate_Timer=COleDateTime::GetCurrentTime();			//��ȡ����ǰʱ��

	switch(nIDEvent)													//��ʱ�¼�
	{
	case 1:																//1s��ʱ�� [��󻯴��塢����ָ��ִ�г�ʱ����]
		//��һ������󻯴���
		if (v_bDlg_Max)
		{
			v_bDlg_Max=false;											//�����Ч
			ShowWindow( SW_SHOWMAXIMIZED );								//��󻯴���

 			if(theApp.a_auto_start_.Compare("YES") == 0){
			OnBnClickedOk();
			}
		}

		//�ڶ�����ָ��ִ�г�ʱ����
		if (v_bCommand_Flag)											//ָ��ִ�У�����
		{
			v_iCommand_Sum++;											//ָ��ִ�м�����++
			if (v_iCommand_Sum>10)										//��ʱ��������ָ���޷��ء�
				v_bCommamd=true;										//��λ��ָ�ʱ��־
		}
		break;
	case 2:																//1m��ʱ�����������
		if (v_iThread_Exit==1)											//��ȡ��ʼ������
			v_bClient=true;												//��λ���ͻ��˴����־����ʱ�˳���

		if (v_tDate_Timer.GetMinute()%15==0)							//15min/��
			v_bTj=true;													//��λ��ͳ�Ʊ�־
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ��ͻ�����
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedFile3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMyExcel MyExcel;
	MyExcel.Show(&m_cList_Client);										//����Excel
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�ָ���
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedOutput2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMyExcel MyExcel;
	MyExcel.Show(&m_cList_Command);										//����Excel
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ��쳣����
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedFile2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMyExcel MyExcel;
	MyExcel.Show(&m_cList_Bug);											//����Excel
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�����...
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CCOM_About dlg;														//������
	dlg.DoModal();														//�򿪣�������
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���������
//------------------------------------------------------------------------------------------------------			
void CServer_DataDlg::OnBnClickedOk2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	v_iThread_Exit=2;													//�߳�״̬��ǿ���˳��߳�
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���ʼ����
//------------------------------------------------------------------------------------------------------			
void CServer_DataDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CTime v_tCurren=CTime::GetCurrentTime();							//��ã���ǰʱ��
	if (v_iThread_Exit==0)
	{
		//���ڴ���
		if(v_tCurren>theApp.a_tDeadLine)								//�Ƚϣ�����Ч����
		{
			MessageBox("ʹ�����ѹ�,���ȡ�µ�Key");						//���ڣ��˳�
			return;														//����
		}
		v_iThread_Exit=1;												//�߳�״̬������
		v_pThread_Query=AfxBeginThread(My_Thread_Read,this);			//�����߳�
	}
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ��������̣߳���ʼ������
//------------------------------------------------------------------------------------------------------			
UINT CServer_DataDlg::My_Thread_Read(LPVOID lparam)
{    
	CServer_DataDlg *  lp_this = (CServer_DataDlg *)lparam;		         //����ָ��
	lp_this->m_cStart.EnableWindow(false);								//��Ч
	lp_this->m_cStop.EnableWindow(true);								//��Ч
	//�������
	lp_this->v_lConnect.clear();										//��գ���Ϣ����
 	lp_this->v_lCommand.clear();										//��գ�ָ������
	//����telnet����
	lp_this->TelnetManager.Start();		

	//��Ϣ�����߳�	
	lp_this->v_pThread_Connect=AfxBeginThread(lp_this->My_Connect,lparam);		
	lp_this->v_cMutex[0]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//ָ����߳�
	lp_this->v_pThread_Command=AfxBeginThread(lp_this->My_Command,lparam);		
	lp_this->v_cMutex[1]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//�ļ������߳�
	lp_this->v_pThread_ParseDataFile=AfxBeginThread(lp_this->ParseDataFileThread,lparam);		
	lp_this->v_cMutex[2]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//���ݿ⴦���߳�
	lp_this->v_pThread_MaintainDataBase=AfxBeginThread(lp_this->MaintainDataBaseThread,lparam);  
	lp_this->v_cMutex[3]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�  

	//SQLBulkInsertά���߳�
	lp_this->v_pThread_ExecuteBulkInsert=AfxBeginThread(lp_this->ExecuteBulkInsertThread,lparam);  
	lp_this->v_cMutex[4]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼� 

	//ȫ����������߳�
	lp_this->Thread_SumWholeNetworkPm_=AfxBeginThread(lp_this->SumWholeNetworkPmThread,lparam);		
	lp_this->v_cMutex[5]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//�����������������߳�
	lp_this->Thread_SumRegionandOtherPm_=AfxBeginThread(lp_this->SumRegionPmThread,lparam);  
	lp_this->v_cMutex[6]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�  

	//����С�������߳�1
	lp_this->Thread1_BaseCellPm_=AfxBeginThread(lp_this->BaseCellPmThread1,lparam);		
	lp_this->v_cMutex[7]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//С����ʽ�����߳�1
	lp_this->Thread1_CalculateCellPm_=AfxBeginThread(lp_this->CalculateCellPmThread1,lparam);		
	lp_this->v_cMutex[8]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//����С�������̡߳�Сʱ���ȡ�
	lp_this->Thread_HourBaseCellPm_=AfxBeginThread(lp_this->HourBaseCellPmThread,lparam);		
	lp_this->v_cMutex[9]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//С����ʽ�����̡߳�Сʱ���ȡ�
	lp_this->Thread_HourCalculateCellPm_=AfxBeginThread(lp_this->HourCalculateCellPmThread,lparam);		
	lp_this->v_cMutex[10]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//��Ϊ��������߳�
	lp_this->Thread_SumHWPm_=AfxBeginThread(lp_this->SumHWPmThread,lparam);		
	lp_this->v_cMutex[11]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//������������߳�
	lp_this->Thread_SumZTEPm_=AfxBeginThread(lp_this->SumZTEPmThread,lparam);		
	lp_this->v_cMutex[12]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//����С�������߳�2
	lp_this->Thread2_BaseCellPm_=AfxBeginThread(lp_this->BaseCellPmThread2,lparam);		
	lp_this->v_cMutex[13]=CreateEvent(NULL, FALSE, FALSE, NULL);			//�����¼�

	//�ȴ��߳��˳�
    WaitForMultipleObjects(14,lp_this->v_cMutex,TRUE,INFINITE);	   
	//m_bThreadAlive��ΪFALSE,ʹ�߳�ȫ���Ƴ�
	lp_this->TelnetManager.ShutDowm();									//ǿ���˳�Telnet
	lp_this->m_cStart.EnableWindow(true);								//��Ч
	lp_this->m_cStop.EnableWindow(false);								//��Ч
	lp_this->v_iThread_Exit=0;											//�߳�״̬���ɹ��˳�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ�����С�������߳�1
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::BaseCellPmThread1(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                       //�ӿ�
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->basePm_lock_.Lock();
		if(pDlg->basePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->basePm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->basePm_queue_.front();
		pDlg->ResetObjType(pDlg->basePm_queue_.front());	
		pDlg->basePm_queue_.pop_front();
		pDlg->basePm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		file_name = pObjType->first;
		file_name = file_name.Mid(0,file_name.GetLength()-2);
		pDlg->GetFileTime(file_name,file_time);
		pDlg->base_Pm_[0].OpenPmFile(theApp.a_save_directory_path_,file_name);
		//�����������
		pDlg->base_Pm_[0].ProcessData(ObjType,file_time,pDlg->base_Pm_[0].Pm_classification); 
		pDlg->base_Pm_[0].ClosePmFile(); 
		temp_time = file_time;
		temp_time.Replace("-","");
		temp_time = "_" + temp_time.Left(8);
		pDlg->sql_lock_.Lock();
		pDlg->base_Pm_[0].PushSqlIntoStack(theApp.a_save_directory_path_,temp_time,1500,file_name,pDlg->sql_stack_);
		pDlg->sql_lock_.Unlock();
		//�ļ��д���С����
		{
			CStdioFile file;
			CString content,message;
			int line = 0;
			if(file.Open(theApp.a_save_directory_path_  + "\\Pm_LTE_PDCP_"+ file_name + ".txt", CFile::modeRead | CFile::typeText)){
				while(file.ReadString(content)){
					line++;
				}

			}
			message.Format("%d",line);
			message = "��ɽ����ļ����������" + message + "��";
			pDlg->ShowListCommand(file_name,message);   
		}      

		//reset
		pDlg->ResetObjType(ObjType);

	}
	SetEvent(pDlg->v_cMutex[7]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ�����С�������߳�2
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::BaseCellPmThread2(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                       //�ӿ�
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->basePm_lock_.Lock();
		if(pDlg->basePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->basePm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->basePm_queue_.front();
		pDlg->ResetObjType(pDlg->basePm_queue_.front());	
		pDlg->basePm_queue_.pop_front();
		pDlg->basePm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		file_name = pObjType->first;
		file_name = file_name.Mid(0,file_name.GetLength()-2);
		pDlg->GetFileTime(file_name,file_time);
		pDlg->base_Pm_[1].OpenPmFile(theApp.a_save_directory_path_,file_name);
		//�����������
		pDlg->base_Pm_[1].ProcessData(ObjType,file_time,pDlg->base_Pm_[1].Pm_classification); 
		pDlg->base_Pm_[1].ClosePmFile(); 
		temp_time = file_time;
		temp_time.Replace("-","");
		temp_time = "_" + temp_time.Left(8);
		pDlg->sql_lock_.Lock();
		pDlg->base_Pm_[1].PushSqlIntoStack(theApp.a_save_directory_path_,temp_time,1500,file_name,pDlg->sql_stack_);
		pDlg->sql_lock_.Unlock();
		//�ļ��д���С����
		{
			CStdioFile file;
			CString content,message;
			int line = 0;
			if(file.Open(theApp.a_save_directory_path_  + "\\Pm_LTE_PDCP_"+ file_name + ".txt", CFile::modeRead | CFile::typeText)){
				while(file.ReadString(content)){
					line++;
				}

			}
			message.Format("%d",line);
			message = "��ɽ����ļ����������" + message + "��";
			pDlg->ShowListCommand(file_name,message);   
		}      

		//reset
		pDlg->ResetObjType(ObjType);

	}
	SetEvent(pDlg->v_cMutex[7]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ�����С�������̡߳�Сʱ���ȡ�
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::HourBaseCellPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                       //�ӿ�
	CString file_name,file_time,temp_time,granularity,before_granularity,file_hour;
	//bool begin_flag = false;
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->hour_basePm_lock_.Lock();
		if(pDlg->hour_basePm_queue_.size()<=0)									
		{
			pDlg->hour_basePm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->hour_basePm_queue_.front();
		pDlg->ResetObjType(pDlg->hour_basePm_queue_.front());		
		pDlg->hour_basePm_queue_.pop_front();
		pDlg->hour_basePm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		//��ʱ����
		if (granularity == "45:00" && pObjType->first == "����")
		{
			CString updata_time;
			COleDateTime Ole_temp_time,granularity_time;
			granularity_time = COleDateTime::GetCurrentTime();
			Ole_temp_time.ParseDateTime(file_time);
			Ole_temp_time -= COleDateTimeSpan(0,0,45,0);
			updata_time = Ole_temp_time.Format("%Y-%m-%d %H:%M:%S");
			int control_count(0),day;
			pDlg->hour_base_Pm_.OpenPmFile(theApp.a_save_directory_path_,file_name);
			//�����������
			pDlg->hour_base_Pm_.meanHourPmCount(pDlg->hour_base_Pm_.hour_Pm_object_);
			pDlg->hour_base_Pm_.ProcessHourData(pDlg->hour_base_Pm_.hour_Pm_object_,updata_time,pDlg->hour_base_Pm_.Pm_classification); 
			pDlg->hour_base_Pm_.ClosePmFile(); 
			pDlg->sql_lock_.Lock();
			Ole_temp_time.ParseDateTime(updata_time);
			day = Ole_temp_time.GetDay();
			day = day - 1;
			control_count = day / 5;
			temp_time = updata_time;
			temp_time.Replace("-","");
			temp_time = "_" + temp_time.Left(6);
			switch (control_count)
			{
			case 0:
				pDlg->hour_base_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_1",1500,file_name,pDlg->sql_stack_);
				break;
			case 1:
				pDlg->hour_base_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_2",1500,file_name,pDlg->sql_stack_);
				break;
			case 2:
				pDlg->hour_base_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_3",1500,file_name,pDlg->sql_stack_);
				break;
			case 3:
				pDlg->hour_base_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_4",1500,file_name,pDlg->sql_stack_);
				break;
			case 4:
				pDlg->hour_base_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_5",1500,file_name,pDlg->sql_stack_);
				break;
			default:
				pDlg->hour_base_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_6",1500,file_name,pDlg->sql_stack_);
				break;
			}
			pDlg->sql_lock_.Unlock();
			pDlg->hour_base_Pm_.Init();
			//ǰһ���������ʱ��
			pDlg->KeepTime("�������_" + updata_time,"���Сʱ���Ȼ������ݴ�����ʱ",granularity_time);

			//ÿ�춨ʱ����
			if (file_hour == "09")
			{
				pDlg->global_control_lock_.Lock();
				pDlg->global_control_ = true;
				pDlg->Updata();
				pDlg->KeepTime("����","����",granularity_time);
				theApp.m_MyLog.WriteText("����\r\n");	
				pDlg->global_control_ = false;
				pDlg->global_control_lock_.Unlock();
			}
		} 
		else
		{
			if (pObjType->first == "����")
			{
				continue;
			}
			file_name = pObjType->first;
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//�����ļ�����
			file_name = "hour_" + file_name;
			//����ʱ��
			granularity = file_time.Right(5);
			file_hour = file_time.Right(8);
			file_hour =file_hour.Left(2);
			pDlg->hour_base_Pm_.SaveHourPmData(ObjType);
		}
		//reset
		pDlg->ResetObjType(ObjType);

	}
	SetEvent(pDlg->v_cMutex[9]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ�С����ʽ�߳�1
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::CalculateCellPmThread1(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                        //�ӿ�
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->calculatePm_lock_.Lock();
		if(pDlg->calculatePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->calculatePm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->calculatePm_queue_.front();
		pDlg->ResetObjType(pDlg->calculatePm_queue_.front());
		pDlg->calculatePm_queue_.pop_front();
		pDlg->calculatePm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		file_name = pObjType->first;
		file_name = file_name.Mid(0,file_name.GetLength()-2);
		pDlg->GetFileTime(file_name,file_time);
	    //����С����ʽ����
		pDlg->calculation_Pm_[0].CalculatePerformance(ObjType,file_time);
		//��ʽ�������ܱ��浽txt�ļ�
		pDlg->calculation_Pm_[0].SaveFormulaPmToTXT(theApp.a_save_directory_path_,file_name,file_time);
		temp_time = file_time;
		temp_time.Replace("-","");
		temp_time = "_" + temp_time.Left(8);
		pDlg->sql_lock_.Lock();
		pDlg->calculation_Pm_[0].PushSqlIntoStack(theApp.a_save_directory_path_,temp_time,1500,file_name,pDlg->sql_stack_);
		pDlg->sql_lock_.Unlock();
		//reset
		pDlg->ResetObjType(ObjType);
	}
	SetEvent(pDlg->v_cMutex[8]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ�С����ʽ�̡߳�Сʱ���ȡ�
//----------------------------------------------------------------------------------------
UINT CServer_DataDlg::HourCalculateCellPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                        //�ӿ�
	CString file_name,file_time,temp_time,granularity_time,before_granularity;
	bool begin_flag = false;
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->hour_calculatePm_lock_.Lock();
		if(pDlg->hour_calculatePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->hour_calculatePm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->hour_calculatePm_queue_.front();
		pDlg->ResetObjType(pDlg->hour_calculatePm_queue_.front());
		pDlg->hour_calculatePm_queue_.pop_front();
		pDlg->hour_calculatePm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		file_name = pObjType->first;
		file_name = file_name.Mid(0,file_name.GetLength()-2);
		pDlg->GetFileTime(file_name,file_time);
		//�����ļ�����
		file_name = "hour_" + file_name;
		//����ʱ��
		granularity_time = file_time.Right(5);                       //��÷���like 00:00 15:00
		if (granularity_time == "00:00" && before_granularity != granularity_time && begin_flag )
		{
			CString updata_time;
			COleDateTime Ole_temp_time;
			int control_count(0),day;
			Ole_temp_time.ParseDateTime(file_time);
			Ole_temp_time -= COleDateTimeSpan(0,1,0,0);
			updata_time = Ole_temp_time.Format("%Y-%m-%d %H:%M:%S");
			//����С����ʽ����
			pDlg->hour_calculation_Pm_.meanHourPmCount(pDlg->hour_calculation_Pm_.hour_Pm_object_);
			pDlg->hour_calculation_Pm_.CalculateHourPerformance(updata_time);
			//��ʽ�������ܱ��浽txt�ļ�
			pDlg->hour_calculation_Pm_.SaveFormulaPmToTXT(theApp.a_save_directory_path_,file_name,updata_time);
			pDlg->sql_lock_.Lock();
			Ole_temp_time.ParseDateTime(updata_time);
			day = Ole_temp_time.GetDay();
			day = day - 1;
			control_count = day / 5;
			temp_time = updata_time;
			temp_time.Replace("-","");
			temp_time = "_" + temp_time.Left(6);
			switch (control_count)
			{
			case 0:
				pDlg->hour_calculation_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_1",1500,file_name,pDlg->sql_stack_);
				break;
			case 1:
				pDlg->hour_calculation_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_2",1500,file_name,pDlg->sql_stack_);
				break;
			case 2:
				pDlg->hour_calculation_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_3",1500,file_name,pDlg->sql_stack_);
				break;
			case 3:
				pDlg->hour_calculation_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_4",1500,file_name,pDlg->sql_stack_);
				break;
			case 4:
				pDlg->hour_calculation_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_5",1500,file_name,pDlg->sql_stack_);
				break;
			default:
				pDlg->hour_calculation_Pm_.PushSqlIntoStack(theApp.a_save_directory_path_,temp_time + "_6",1500,file_name,pDlg->sql_stack_);
				break;
			}
			pDlg->sql_lock_.Unlock();
			pDlg->hour_calculation_Pm_.Init();
			pDlg->hour_calculation_Pm_.SaveHourPmData(ObjType);
			before_granularity = granularity_time;
		} 
		else
		{
			pDlg->hour_calculation_Pm_.SaveHourPmData(ObjType);
			before_granularity = granularity_time;
		}

		//reset
		begin_flag = true;
		pDlg->ResetObjType(ObjType);
	}
	SetEvent(pDlg->v_cMutex[10]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ�ȫ����������߳�
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::SumWholeNetworkPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                        //�ӿ�
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->networkPm_lock_.Lock();
		if(pDlg->networkPm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->networkPm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->networkPm_queue_.front();
		pDlg->ResetObjType(pDlg->networkPm_queue_.front());
		pDlg->networkPm_queue_.pop_front();
		pDlg->networkPm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		file_name = pObjType->first;
		if (file_name == "����")
		{
			//ȫ����������
			pDlg->network_Pm_.CalculatePmForWholeNetwork();
			//ȫ�������ϴ�
			pDlg->network_Pm_.UploadingPm(file_time,pDlg->network_Pm_.network_Pm_,"ȫ��");
		} 
		else
		{
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//ȫ�������������
			pDlg->network_Pm_.SumBasePmForWholeNetwork(ObjType,pDlg->network_filter_);
			//ȫ�����������������
			pDlg->network_Pm_.SumBasePmInGranularityForWholeNetwork();
		}
		//reset
		pDlg->ResetObjType(ObjType);
	}
	SetEvent(pDlg->v_cMutex[5]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ���Ϊ��������߳�
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::SumHWPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                        //�ӿ�
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1 )										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->hwPm_lock_.Lock();
		if(pDlg->hwPm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->hwPm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->hwPm_queue_.front();
		pDlg->ResetObjType(pDlg->hwPm_queue_.front());
		pDlg->hwPm_queue_.pop_front();
		pDlg->hwPm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		file_name = pObjType->first;
		if (file_name == "����")
		{
			//��Ϊ��������
			pDlg->hw_Pm_.CalculatePmForHW();
			//��Ϊ�����ϴ�
			pDlg->hw_Pm_.UploadingPm(file_time,pDlg->hw_Pm_.hw_Pm_,"��Ϊ");
		} 
		else
		{
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//��Ϊ�����������
			pDlg->hw_Pm_.SumBasePmForHW(ObjType,pDlg->hw_filter_);
			//��Ϊ���������������
			pDlg->hw_Pm_.SumBasePmInGranularityForHW();
		}
		//reset
		pDlg->ResetObjType(ObjType);
	}

	SetEvent(pDlg->v_cMutex[11]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ�������������߳�
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::SumZTEPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                        //�ӿ�
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->ztePm_lock_.Lock();
		if(pDlg->ztePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->ztePm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->ztePm_queue_.front();
		pDlg->ResetObjType(pDlg->ztePm_queue_.front());
		pDlg->ztePm_queue_.pop_front();
		pDlg->ztePm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		file_name = pObjType->first;
		if (file_name == "����")
		{
			//���˼�������
			pDlg->zte_Pm_.CalculatePmForZTE();
			//���������ϴ�
			pDlg->zte_Pm_.UploadingPm(file_time,pDlg->zte_Pm_.zte_Pm_,"����");
		} 
		else
		{
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//���˻����������
			pDlg->zte_Pm_.SumBasePmForZTE(ObjType,pDlg->zte_filter_);
			//���˻��������������
			pDlg->zte_Pm_.SumBasePmInGranularityForZTE();
		}

		//reset
		pDlg->ResetObjType(ObjType);
	}

	SetEvent(pDlg->v_cMutex[12]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ������������������߳�
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::SumRegionPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��
	map_ObjType::iterator	pObjType;							//�ӿ�ָ��
	map_ObjType ObjType;                                        //�ӿ�
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		pDlg->regionPm_lock_.Lock();
		if(pDlg->regionPm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->regionPm_lock_.Unlock();
			Sleep(500);												//��ʱ
			continue;													//�����ȴ�
		}
		ObjType = pDlg->regionPm_queue_.front();
		pDlg->ResetObjType(pDlg->regionPm_queue_.front());
		pDlg->regionPm_queue_.pop_front();
		pDlg->regionPm_lock_.Unlock();
		pObjType = ObjType.begin();
		if ( pObjType == ObjType.end())
		{
			continue;
		}
		file_name = pObjType->first;
		if (file_name == "����")
		{
			//������������
			pDlg->region_Pm_.CalculatePmForOther();
			//���������ϴ�
			pDlg->region_Pm_.UploadingPm(file_time,pDlg->region_Pm_.other_Pm_,"����");
			//���������ϴ�
			pDlg->region_Pm_.CalculateandUploadingPmForRegion(file_time);
		} 
		else
		{
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//��������������������
			pDlg->region_Pm_.SumBasePmForRegionandOther(ObjType,pDlg->region_filter_);
			//�������������������
			pDlg->region_Pm_.SumBasePmInGranularityForOther();
			//������������������
			pDlg->region_Pm_.SumBasePmInGranularityForRegion();
		}
		//reset
		pDlg->ResetObjType(ObjType);
	}
	SetEvent(pDlg->v_cMutex[6]);							    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ�������Ϣ�����߳� ��������������¼��Ϣ��ָ��ִ�н������
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::My_Connect(LPVOID lparam)
{   
	CString		v_sTemp;												//��ʱ����
	int			v_iCount;												//��ʱ����
	v_sCommand	v_cCommand;
	list<v_sCommand>::iterator v_pIter;									//������
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//�Ի���ָ��

	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		pDlg->v_cLock_Connect.Lock();									//����
		v_iCount=pDlg->v_lConnect.size();								//��������
		pDlg->v_cLock_Connect.Unlock();									//����
		if(v_iCount<=0)													//û�еȴ�������񣿣���
		{
			Sleep(50);													//��ʱ
			continue;													//�����ȴ�
		}		

		pDlg->m_cAAA.SetWindowText("1");
		//��ȡ������Ϣ
		pDlg->v_cLock_Connect.Lock();									//����
		v_pIter=pDlg->v_lConnect.begin();								//��ȡ������Ϣ
		if (v_pIter!=pDlg->v_lConnect.end())
		{
			v_cCommand.v_iCode=v_pIter->v_iCode;						//
			v_cCommand.v_iSocket=v_pIter->v_iSocket;					//
			v_cCommand.v_sText=v_pIter->v_sText;						//
			pDlg->v_lConnect.pop_front();								//�����Ƴ��������ѻ�ȡ������
		}
		else
		{
			pDlg->v_lConnect.clear();									//�����Ƴ��������ѻ�ȡ������
		}
		pDlg->v_cLock_Connect.Unlock();									//����

		pDlg->m_cAAA.SetWindowText("2");
		//������Ϣ����
		if (v_cCommand.v_iCode==0x01)									//������Ϣ
		{	
			//�ͻ���������++
			map<SOCKET,v_sClient_Connect>::iterator v_pIter;			//������
			v_pIter=pDlg->TelnetManager.v_mClient_Connect.find(v_cCommand.v_iSocket);	//���ң�����
			if (v_pIter!=pDlg->TelnetManager.v_mClient_Connect.end())	//�ҵ�������
				v_pIter->second.v_iSum++;								//����++
			//�ظ����ͻ�������
			pDlg->Send_Command(v_cCommand.v_iSocket,0x81,"");			//�ظ����ͻ�������
			if(pDlg->m_cinformation_monitor.GetCheck() == 1)
				pDlg->My_ShowList_Bug("��������","");					//��ʾ������
			//����ǲɼ���SOCKET����λ�ɼ��˼�����
			if (pDlg->v_sServer_Socket!=NULL && pDlg->v_sServer_Socket==v_cCommand.v_iSocket)	//�ɼ���SOCKET==����SOCKET������
				pDlg->v_iServer_Sum=0;									//��λ���ɼ��˼�����
		}
		else if(v_cCommand.v_iCode==0x02)								//��¼��Ϣ����ʾ�ͻ����ơ�
		{
			pDlg->TelnetManager.v_cTelnet_Lock.Lock();					//����
			//�޸Ŀͻ���Ϣ
			map<SOCKET,v_sClient_Connect>::iterator v_pIter;			//������
			v_pIter=pDlg->TelnetManager.v_mClient_Connect.find(v_cCommand.v_iSocket);	//���ң�����
			if (v_pIter!=pDlg->TelnetManager.v_mClient_Connect.end())	//�ҵ�������
				v_pIter->second.v_sName=v_cCommand.v_sText;				//ɾ���ͻ�����Ϣ
			//��ʾ�ͻ���Ϣ
			for (int vi=0;vi<pDlg->m_cList_Client.GetItemCount();vi++)	//�������ͻ��б�
			{
				if (v_cCommand.v_iSocket==atoi(pDlg->m_cList_Client.GetItemText(vi,3)))		//�ҵ������������ݣ��ͻ�SOCKET��
				{
					pDlg->m_cList_Client.SetItemText(vi,4,v_cCommand.v_sText);				//���
					break;												//�˳�
				}
			}
			//����ͻ�����=="�ɼ�������"�����ǲɼ��ˣ�
			if (v_cCommand.v_sText=="�ɼ�������")						//�ǲɼ�������������
			{
				pDlg->v_sServer_Socket=v_cCommand.v_iSocket;			//��¼���ɼ���SOCKET
				pDlg->m_cServer.EnableWindow(true);						//��Ч���ɼ���
			}
			pDlg->TelnetManager.v_cTelnet_Lock.Unlock();				//����
		}
		else if(v_cCommand.v_iCode==0x86)								//BSC�ɼ��쳣��֪ͨ���пͻ��ˡ�
		{
			pDlg->My_ShowList_Bug("BSC�ɼ��쳣","�쳣��"+v_cCommand.v_sText);		//��ʾ���쳣
			theApp.m_MyLog.WriteText("�쳣��"+v_cCommand.v_sText+"\r\n");			//log
			pDlg->My_Send_Client_Alarm(v_cCommand.v_iCode,v_cCommand.v_sText);		//�����пͻ��˷�����Ϣ
		}
		else if(v_cCommand.v_iCode==0x83)								//ָ��ִ�н���������ͻ��´�ָ��Ŀͻ��ˡ�
		{
			pDlg->m_cAAA.SetWindowText("3");
			pDlg->TelnetManager.v_cTelnet_Lock.Lock();					//����
			pDlg->Send_Command(pDlg->v_iClient_Socket,v_cCommand.v_iCode,v_cCommand.v_sText);	//����ָ��ִ�н�����ظ��ͻ��ˡ�
			pDlg->v_sCommand_Rece+=v_cCommand.v_sText;					//ָ��ִ�н��
			pDlg->m_cAAA.SetWindowText("4");
			pDlg->My_ShowList_Command_Text(v_cCommand.v_sText);			//ָ��ִ�н����ʾ
			pDlg->m_cAAA.SetWindowText("5");
			if (pDlg->v_sCommand_Rece.Find("\r\nEND")!=-1 || pDlg->v_sCommand_Rece.Find("\r\nNOT ACCEPTED")!=-1)		//ָ��ִ�н���������
				pDlg->v_bCommand_Flag=0;								//��λ��ָ��ͱ��

			pDlg->v_sCommand_Rece=v_cCommand.v_sText;					//ָ��ִ�н��
			pDlg->TelnetManager.v_cTelnet_Lock.Unlock();				//����
		}
		pDlg->m_cAAA.SetWindowText("9");
	}
	SetEvent(pDlg->v_cMutex[0]);										//���ͣ��߳̽����¼�
	return 0;
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�ָ����̡߳�ָ��ʹ���
//------------------------------------------------------------------------------------------------------
UINT CServer_DataDlg::My_Command(LPVOID lparam)
{
 	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//����ָ��
    int			v_iCount;												//��ʱ����
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//��һ����ָ�ʱ
		if(pDlg->v_bCommamd)											//ָ�ʱ��־��Ч������
		{
			pDlg->v_bCommamd=false;										//��λ��ָ�ʱ��־
			pDlg->TelnetManager.v_cTelnet_Lock.Lock();					//����
			pDlg->Send_Command(pDlg->v_iClient_Socket,0x83,"ָ��ִ�г�ʱ��\r\nEND");	//����ָ�ʱ���
			pDlg->My_ShowList_Command_Text("ָ��ִ�г�ʱ��");			//ָ��ִ�н����ʾ
			pDlg->v_bCommand_Flag=0;									//��λ��ָ��ͱ��
			pDlg->TelnetManager.v_cTelnet_Lock.Unlock();				//����
		}

		//�ڶ������ͻ��˴���
		if(pDlg->v_bClient)												//�ͻ��˴����־��Ч������
		{
			pDlg->v_bClient=false;										//��λ���ͻ��˴����־
			pDlg->My_Client_Proc();										//�ͻ�����Ϣ������ʱ�˳���
		}

		//��������ָ���
		pDlg->v_cLock_Command.Lock();									//����
		v_iCount=pDlg->v_lCommand.size();								//���������
		pDlg->v_cLock_Command.Unlock();									//����
		if(pDlg->v_bCommand_Flag==1 || v_iCount<=0)						//û��ִ��ָ����� || û�еȴ�������񣿣���
		{
			Sleep(50);													//��ʱ
			continue;													//�����ȴ�
		}		

		pDlg->m_cTest.SetWindowText("1");

		//��ȡ������Ϣ
		pDlg->v_cLock_Command.Lock();									//����
		list<v_sCommand>::iterator v_pIter=pDlg->v_lCommand.begin();	//
		v_sCommand v_cCommand;
		v_cCommand.v_iCode=v_pIter->v_iCode;							//��ȡָ����Ϣ
		v_cCommand.v_iSocket=v_pIter->v_iSocket;						//��ȡָ����Ϣ
		v_cCommand.v_sText=v_pIter->v_sText;							//��ȡָ����Ϣ
		pDlg->v_lCommand.pop_front();									//�����Ƴ��������ѻ�ȡ������
		pDlg->v_cLock_Command.Unlock();									//����

		pDlg->m_cTest.SetWindowText("2");

		//����ָ��03����ȡ�����ļ����ƣ��ļ�����������map����
		if (v_cCommand.v_iCode==0x03)									//ָ����Ϣ
		{	
			CString sTemp;
			sTemp=v_cCommand.v_sText;
			sTemp.Delete(0,14);
			if (sTemp.Left(1)=="E")                                     //HW PM
			{
				sTemp=sTemp.Mid(28,16);
				sTemp.Replace("-"," ");
				sTemp.Insert(4,"-");
				sTemp.Insert(7,"-");
				sTemp.Insert(13,":");
				sTemp.Insert(16,":00");
				sTemp+="_HW";	
			} 
			else
			{
				sTemp=sTemp.Mid(33,16);                                 //ZTE PM
				sTemp.Replace("-"," ");
				sTemp.Replace("-"," ");
				sTemp.Insert(4,"-");
				sTemp.Insert(7,"-");
				sTemp.Insert(13,":");
				sTemp.Insert(16,":00");
				sTemp+="_ZTE";
			}

			if (pDlg->m_cinformation_monitor.GetCheck() == 1)
			{
				COleDateTime SpanTime=COleDateTime::GetCurrentTime();		                      //������ʱ
				CString sTime=SpanTime.Format("%H:%M:%S");
				CString sMsg  =  "0x03";
				pDlg->ShowListCommand(sTemp,sMsg);
				theApp.m_MyLog.WriteText("ͨ�ţ�"+sMsg+":"+sTemp+"\r\n");	  //log
			}

			//���ҿͻ�����Ϣ
			pDlg->v_cLock_FileName.Lock();							     //����
			pDlg->file_queue_.push(v_cCommand.v_sText);
			pDlg->v_cLock_FileName.Unlock();		
		}
		//���Ƚ���ָ��05
		else if (v_cCommand.v_iCode==0x05)									//ָ�����
		{
			//���ҿͻ�����Ϣ
			CString sTemp;
			sTemp=v_cCommand.v_sText;
			pDlg->v_cLock_FileName.Lock();							     //����
			pDlg->file_queue_.push(v_cCommand.v_sText);
			pDlg->v_cLock_FileName.Unlock();	
			if (pDlg->m_cinformation_monitor.GetCheck() == 1)
			{
				COleDateTime SpanTime=COleDateTime::GetCurrentTime();		                      //������ʱ
				CString sTime=SpanTime.Format("%H:%M:%S");
				CString sMsg  =  "0x05";
				pDlg->ShowListCommand(sTemp,sMsg);
				theApp.m_MyLog.WriteText("ͨ�ţ�"+sMsg+":"+sTemp+"\r\n");	  //log
			}
		}
	}
	SetEvent(pDlg->v_cMutex[1]);										//���ͣ��߳̽����¼�
	return 0;
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ����������ݿ����ع���
//------------------------------------------------------------------------------------------------------
UINT CServer_DataDlg::MaintainDataBaseThread(LPVOID lparam)
{
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				        //����ָ��
	COleDateTime     current_time,temp_time;
	COleDateTime FolderTime,CurTime,compareTime,tempTime;
	CString          time_parameter,folder_name,delete_time;
	bool             month_table_flag(true),day_table_flag(true);
	int nValue,day;
	//logĿ¼�µ��ļ�������
	list<CString>       folder_queue;
	pDlg->DB_maintenance_lock_.Lock();
	pDlg->DB_maintenance_.Init();
	pDlg->DB_maintenance_lock_.Unlock();
	//��ȡ��ǰʱ����������졿
	current_time = COleDateTime::GetCurrentTime();
	time_parameter = current_time.Format("%Y-%m-%d");
	current_time.ParseDateTime(time_parameter);
	current_time -= COleDateTimeSpan(1,0,0,0);

	//��ȫδ����15�����������ݿ��
	compareTime.ParseDateTime(pDlg->DB_maintenance_.granularity_time_);
	while(compareTime < current_time){
		compareTime += COleDateTimeSpan(1,0,0,0);
		time_parameter = compareTime.Format("%Y-%m-%d");
		pDlg->DB_maintenance_.CreateQuarterGranularityTable(time_parameter);
    }

	//��ȫδ���ɵ���Сʱ�������ݿ��
	pDlg->DB_maintenance_lock_.Lock();
	//�ж��Ƿ����CON_Pm_LTE_Cal_Next_Month��
	month_table_flag = pDlg->DB_maintenance_.JudgeCalculateTableUpdate();
	if (month_table_flag == true)
	{
		//�����¸��±�׼��ʽ���ܱ�,�������ɾ�����½�
		{
			pDlg->DB_maintenance_.UpdateCONPmLTECalNextMonthTable();
		}
	}
	//�ж��±��Ƿ��Ѿ�������true:������һ���±�
	month_table_flag = pDlg->DB_maintenance_.JudgeThisMonthHourGranularityTableExist(time_parameter);
	if (month_table_flag == true)
	{
		//������һ����Сʱ�������ݿ��
		{
			pDlg->DB_maintenance_.CreateThisMonthHourGranularityTable(time_parameter);
		}
	}
	pDlg->DB_maintenance_lock_.Unlock();

	while(pDlg->v_iThread_Exit==1)
	{

		//û�еȴ�������񣿣���
		if(pDlg->global_control_)									
		{
			Sleep(1000);												//��ʱ
			continue;													//�����ȴ�
		}

		//��ȡ��ǰʱ�����
		current_time = COleDateTime::GetCurrentTime();
		time_parameter = current_time.Format("%Y-%m-%d");

		//ɾ������GZ�ļ�
		CurTime = COleDateTime::GetCurrentTime();
		pDlg->DB_maintenance_lock_.Lock();
		nValue = atoi(pDlg->DB_maintenance_.log_save_day_);
		pDlg->DB_maintenance_lock_.Unlock();
		pDlg->file_collection_.SearchDirectory(theApp.a_sPm_Path_,folder_queue);
		while(folder_queue.size()>0)
		{
			folder_name = *(folder_queue.begin());
			compareTime.ParseDateTime(folder_name);
			compareTime += COleDateTimeSpan(nValue,0,0,0);
			if(compareTime<=CurTime)
			{
				pDlg->DB_maintenance_lock_.Lock();
				pDlg->DB_maintenance_.DeleteFolderandFile(folder_name,theApp.a_sPm_Path_);
				pDlg->DB_maintenance_lock_.Unlock();
			}
			folder_queue.pop_front();
		}

		//ɾ������15�������ȱ�
		day = atoi(pDlg->DB_maintenance_.pm_save_day_);
		temp_time.ParseDateTime(pDlg->DB_maintenance_.granularity_time_);
		temp_time -= COleDateTimeSpan(day,0,0,0);
		delete_time = temp_time.Format("%Y-%m-%d %H:%M");
		pDlg->DB_maintenance_lock_.Lock();
		pDlg->DB_maintenance_.DeleteStaleData(delete_time);
		pDlg->DB_maintenance_lock_.Unlock();

		//ɾ������bulkinsert�ļ�
		pDlg->DB_maintenance_lock_.Lock();
		CString delete_granularity_time;
		pDlg->DB_maintenance_.Init();
		delete_granularity_time = pDlg->DB_maintenance_.granularity_time_;
		pDlg->DB_maintenance_.DeleteBulkInsertFile(delete_granularity_time,theApp.a_save_directory_path_);
        pDlg->DB_maintenance_lock_.Unlock();

		//�����±�
		pDlg->DB_maintenance_lock_.Lock();
		if (current_time.GetDay() == 28)
		{
			//�ж��Ƿ����CON_Pm_LTE_Cal_Next_Month��
			month_table_flag = pDlg->DB_maintenance_.JudgeCalculateTableUpdate();
			if (month_table_flag == true)
			{
				//�����¸��±�׼��ʽ���ܱ�,�������ɾ�����½�
				{
					pDlg->DB_maintenance_.UpdateCONPmLTECalNextMonthTable();
				}
			}
			//�ж��±��Ƿ��Ѿ�������true:������һ���±�
			month_table_flag = pDlg->DB_maintenance_.JudgeHourGranularityTableExist(time_parameter);
			if (month_table_flag == true)
			{
				//������һ����Сʱ�������ݿ��
				{
					pDlg->DB_maintenance_.CreateHourGranularityTable(time_parameter);
				}
			}
		}
		pDlg->DB_maintenance_lock_.Unlock();

		//����Pm_LTE_Cal�����ܼ���15���ȱ�
		pDlg->DB_maintenance_lock_.Lock();
		if (current_time.GetDay() == 1)
		{
			//������������
			pDlg->global_control_lock_.Lock();

			//�ж��Ƿ����CON_Pm_LTE_Cal_This_Month��
			month_table_flag = pDlg->DB_maintenance_.JudgeThisCalculateTableUpdate();
			if (month_table_flag == true)
			{
				//�������±�׼��ʽ���ܱ�,�������ɾ�����½�
				{
					pDlg->DB_maintenance_.UpdateCONPmLTECalThisMonthTable();
				}
				//��������
				pDlg->global_control_ = true;
				pDlg->Updata();
				theApp.m_MyLog.WriteText("�²���\r\n");	
				pDlg->global_control_ = false;
				pDlg->global_control_lock_.Unlock();
			}
			else
			{
				pDlg->global_control_lock_.Unlock();
			}
		}
		pDlg->DB_maintenance_lock_.Unlock();

		//����ÿ���
		{
			pDlg->DB_maintenance_lock_.Lock();
			day_table_flag = pDlg->DB_maintenance_.JudgeQuarterGranularityTableExist(time_parameter);
			if (day_table_flag == true)
			{
				//��������һ��15�����������ݿ��
				{
					pDlg->DB_maintenance_.CreateQuarterGranularityTable(time_parameter);
				}
			}
			pDlg->DB_maintenance_lock_.Unlock();
		}

		//����
		Sleep(300*1000);

	}

	SetEvent(pDlg->v_cMutex[3]);
	return 0;
}
	
//----------------------------------------------------------------------------------------
//	�������ܣ�ִ��Bulk insert���
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::ExecuteBulkInsertThread(LPVOID lparam)
{
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;						//�Ի���ָ��
	while(pDlg->v_iThread_Exit==1 )										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		if(pDlg->sql_stack_.size()<=0 || pDlg->global_control_)									
		{
			Sleep(1000);												//��ʱ
			continue;													//�����ȴ�
		}
		pDlg->ExecuteBulkInsertStack();
	}
	SetEvent(pDlg->v_cMutex[4]);										    //���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ���XML�ļ���ѹ,��������,������С�
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::ParseDataFileThread(LPVOID lparam)
{
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;						  //�Ի���ָ��
	map_ObjType::iterator	 iterator_ObjType;							  //ObjTypeָʾ��
	map_ObjType_Child::iterator iterator_ObjType_Child;                   //ObjType_Child������
	map_ObjType              ObjType;                                     //�ӿ�
	CString temp,socket_file_name,zip_file_path,zip_file_name,xml_file_name,xml_file_path,file_name,file_time,file_type,current_grantularity,before_grantularity,grantularity_time;
	CString delete_granularity_time;
	COleDateTime before_time,Ole_temp_time;
	CString message;
	int temp_count(0);
	//���ݳ�ʼ��
	pDlg->global_control_lock_.Lock();
	pDlg->Init();
	pDlg->global_control_lock_.Unlock();
	//�ļ���¼
	pDlg->v_cLock_FileName.Lock();
	pDlg->file_collection_.GetUnprocessedLog(theApp.a_sPm_Path_,pDlg->DB_maintenance_.granularity_time_,pDlg->file_queue_);
	pDlg->v_cLock_FileName.Unlock();
	//ɾ���ظ�����
	delete_granularity_time = pDlg->DB_maintenance_.granularity_time_;
	//ǰһ���������ʱ��
	grantularity_time = COleDateTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
	pDlg->DeleteNextGranularityData(delete_granularity_time);
	while(pDlg->v_iThread_Exit==1)										//ǿ���˳������� ��v_iThread_Exit==2��
	{
		//û�еȴ�������񣿣���
		if(pDlg->file_queue_.size()<=0 || pDlg->global_control_)									
		{
			Sleep(1000);												//��ʱ
			continue;													//�����ȴ�
		}

		before_time = COleDateTime::GetCurrentTime();				  //��õ�ǰʱ��

		pDlg->v_cLock_FileName.Lock();									//����	
		socket_file_name = pDlg->file_queue_.front();                   //��·���ĺ��ļ�����			
		pDlg->file_queue_.pop();
		//��ʾ���ļ���
		temp.Format("%d",pDlg->file_queue_.size());
		pDlg->m_cFileCount.SetWindowTextA(temp);						
		pDlg->v_cLock_FileName.Unlock();

		//���Ƚ���
		if (socket_file_name == "����")
		{
			pDlg->ResetObjType(ObjType);
			list_Object temp_list_Object;
			ObjType["����"] = temp_list_Object;

			//��ȡ����ŵ�ȫ�����ܶ��С�3��
			pDlg->networkPm_lock_.Lock();										//����
			pDlg->networkPm_queue_.push_back(ObjType);				//���������������ܶ���
			pDlg->networkPm_lock_.Unlock();

			//��ȡ����ŵ��������ܶ��С�4��
			pDlg->regionPm_lock_.Lock();										//����
			pDlg->regionPm_queue_.push_back(ObjType);				//���������������ܶ���
			pDlg->regionPm_lock_.Unlock();

			//��ȡ����ŵ�Сʱ���Ȼ������ܶ��С�5��
			pDlg->hour_basePm_lock_.Lock();												//����
			pDlg->hour_basePm_queue_.push_back(ObjType);						 //���������������ܶ���
			pDlg->hour_basePm_lock_.Unlock();

			//��ȡ����ŵ���Ϊ���ܶ��С�7��
			pDlg->hwPm_lock_.Lock();											//����
			pDlg->hwPm_queue_.push_back(ObjType);				    //���������������ܶ���
			pDlg->hwPm_lock_.Unlock();

			//��ȡ����ŵ��������ܶ��С�8��
			pDlg->ztePm_lock_.Lock();											//����
			pDlg->ztePm_queue_.push_back(ObjType);				    //���������������ܶ���
			pDlg->ztePm_lock_.Unlock();

			pDlg->ResetObjType(ObjType);

			//�������ݿ�������ȣ���
			if ( current_grantularity == "45:00")
			{
				//�ϴ��������ʱ��
				temp = "update CON_System set zPm_LTE_Time_Data='"+ file_time +"'";
				pDlg->ExecuteSQL(temp);
			}

			//ɾ���ظ�����
			delete_granularity_time = file_time;
			pDlg->DeleteNextGranularityData(delete_granularity_time);

			//ǰһ���������ʱ��
			Ole_temp_time.ParseDateTime(grantularity_time);
			pDlg->KeepTime("�������_" + file_time,"���15�����������ݴ�������ʱ",Ole_temp_time);

			//�������ʱ�����
			Sleep(20*1000);
			temp = "update CON_System set zPm_LTE_Time_Real='"+ file_time +"'";
			pDlg->ExecuteSQL(temp);

			//�жϴ�������������
			if (GetDriveType("F:") == DRIVE_NO_ROOT_DIR)
			{
				system(_T("net use F: \\\\192.168.1.101\\f  \"Aa123456\" /user:\"DLPM\""));
			}

			continue;
		}

		//����ļ���Ϣ
		pDlg->GetFileInformation(socket_file_name,zip_file_name,zip_file_path,xml_file_name,xml_file_path,file_name,file_time,file_type);

		//��ʾ���ڴ�������ʱ��
		Ole_temp_time.ParseDateTime(file_time);
		Ole_temp_time += COleDateTimeSpan(0,0,15,0);
		temp = Ole_temp_time.Format("%Y-%m-%d %H:%M");
		pDlg->m_cGTime.SetWindowTextA(temp);

		//��ǰ����ʱ��
		current_grantularity = file_time.Right(5);

		//�������ȼ�ʱʱ�䣿
		if (current_grantularity != before_grantularity)
		{
			grantularity_time = COleDateTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
		}

		//log�ļ���ѹ
		if (!pDlg->PMArchive(zip_file_path,xml_file_path))
		{
			message  = zip_file_name+"��ѹ�ļ�ʧ��";		
			pDlg->My_ShowList_Bug("��ѹ�ļ��쳣","�����쳣��"+message);		//��ʾ���쳣
			continue;
		}

		//��ȡ���ݱ��浽����
		pDlg->v_cFile_XML.File_Reset();//�������
		if (!pDlg->v_cFile_XML.Read_XML_PM_Dir(xml_file_path))             //��ȡ��XML�ļ���Ϣ
		{      			
			message  = zip_file_name+"����ļ�ʧ��";		
			pDlg->My_ShowList_Bug("��ȡ�ļ��쳣","�����쳣��"+message);		//��ʾ���쳣
			continue;
		}

		////�ӿڻ���ļ�ʱ��
		//temp_count = 0;
		//iterator_ObjType = pDlg->v_cFile_XML.v_mObjType.begin();
		//while(iterator_ObjType != pDlg->v_cFile_XML.v_mObjType.end())
		//{
		//	temp_count ++;
		//	temp.Format("%d",temp_count);
		//	temp = file_name + "_"  + temp;
		//	ObjType[temp] = iterator_ObjType->second;
		//	iterator_ObjType ++;
		//}

		//�ӿڻ���ļ�ʱ��
		temp_count = 0;
		iterator_ObjType_Child = pDlg->v_cFile_XML.v_mObjType_Child.begin();
		while(iterator_ObjType_Child != pDlg->v_cFile_XML.v_mObjType_Child.end())
		{
			temp_count ++;
			temp.Format("%d",temp_count);
			temp = file_name + "_"  + temp;
			ObjType[temp] = iterator_ObjType_Child->second.v_lObject;
			iterator_ObjType_Child ++;
		}

		//С������ӳ��Ϊ����
		pDlg->MapCellNameToID(ObjType,pDlg->cellname_map_ID_);

		//�Ǵ���С�����ݹ���
		pDlg->cell_filter_.FilterNonDalianCell(ObjType);

		//��ȡ����ŵ��������ܶ��С�1��
		pDlg->basePm_lock_.Lock();												//����
		pDlg->basePm_queue_.push_back(ObjType);								 //���������������ܶ���
		pDlg->basePm_lock_.Unlock();

		//��ȡ����ŵ��������ܶ��С�2��
		pDlg->calculatePm_lock_.Lock();											//����
		pDlg->calculatePm_queue_.push_back(ObjType);						 //���������������ܶ���
		pDlg->calculatePm_lock_.Unlock();
		
		//��ȡ����ŵ�ȫ�����ܶ��С�3��
		pDlg->networkPm_lock_.Lock();											//����
		pDlg->networkPm_queue_.push_back(ObjType);							//���������������ܶ���
		pDlg->networkPm_lock_.Unlock();

		//��ȡ����ŵ��������ܶ��С�4��
		pDlg->regionPm_lock_.Lock();											//����
		pDlg->regionPm_queue_.push_back(ObjType);							//���������������ܶ���
		pDlg->regionPm_lock_.Unlock();

		//��ȡ����ŵ�Сʱ���Ȼ������ܶ��С�5��
		pDlg->hour_basePm_lock_.Lock();												//����
		pDlg->hour_basePm_queue_.push_back(ObjType);						 //���������������ܶ���
		pDlg->hour_basePm_lock_.Unlock();

		//��ȡ����ŵ�Сʱ���ȼ������ܶ��С�6��
		pDlg->hour_calculatePm_lock_.Lock();												//����
		pDlg->hour_calculatePm_queue_.push_back(ObjType);					 //���������������ܶ���
		pDlg->hour_calculatePm_lock_.Unlock();
       
		if(file_type == "��Ϊ")
		{
			//��ȡ����ŵ���Ϊ���ܶ��С�7��
			pDlg->hwPm_lock_.Lock();											//����
			pDlg->hwPm_queue_.push_back(ObjType);				//���������������ܶ���
			pDlg->hwPm_lock_.Unlock();
		}
		else
		{
			//��ȡ����ŵ��������ܶ��С�8��
			pDlg->ztePm_lock_.Lock();											//����
			pDlg->ztePm_queue_.push_back(ObjType);				//���������������ܶ���
			pDlg->ztePm_lock_.Unlock();
			pDlg->ResetObjType(ObjType);
		}
		//��սӿ�
		pDlg->ResetObjType(ObjType);
		before_grantularity = current_grantularity;

		//ʱ���ʱ
		pDlg->KeepTime(file_name,"����ļ��������������",before_time);

	}
	SetEvent(pDlg->v_cMutex[2]);												//���ͣ��߳̽����¼�
	return 0;
}

//----------------------------------------------------------------------------------------
//	�������ܣ��ͻ�����Ϣ������ʱ�˳���
//------------------------------------------------------------------------------*---------
void CServer_DataDlg::My_Client_Proc()
{
	bool	v_bOK=false;												//ɾ����־
	//��һ������ʾ[IOCP�߳���-ָ��-����]
	CString v_sCount;													//��ʱ��������ʾ������չ��IOCP�߳���-��Ϣ������-ָ���������
	v_sCount.Format("%d-%d-%d",TelnetManager.v_cIocp.GetWorkThread(),v_lCommand.size(),v_lConnect.size());		//IOCP�߳���-ָ��-����
	m_cState.SetWindowText(v_sCount);									//��ʾ��IOCP�߳���-ָ��-����

	//�ڶ��������߿ͻ���ɾ��			
	TelnetManager.v_cTelnet_Lock.Lock();								//����
	map<SOCKET,v_sClient_Connect>::iterator v_pIter;					//������
	v_pIter=TelnetManager.v_mClient_Connect.begin();					//����������λ��
	while(v_pIter!=TelnetManager.v_mClient_Connect.end())				//����������
	{
		v_pIter->second.v_iTimer++;										//��ʱ��������++
		if (v_pIter->second.v_iTimer>=3)								//>=3��
		{
			if(v_pIter->second.v_iSum==0)								//û������������
			{
				if (v_sServer_Socket==v_pIter->second.v_iSocket)		//�ǲɼ�������������
				{
					v_sServer_Socket=NULL;								//��λ���ɼ���SOCKET
					m_cServer.EnableWindow(false);						//��Ч���ɼ���
				}
				if (v_pIter->second.v_iSocket!=NULL)					//�ͻ���SOCKET��Ч������
					closesocket(v_pIter->second.v_iSocket);				//�رտͻ�������
				v_pIter=TelnetManager.v_mClient_Connect.erase(v_pIter);	//ɾ���ͻ�����Ϣ
				v_bOK=true;												//ɾ����־
				continue;												//��������
			}
			v_pIter->second.v_iTimer=0;									//��λ����ʱ��������
			v_pIter->second.v_iSum=0;									//��λ������������			
		}
		v_pIter++;														//������++
	}
	if (v_bOK)															//��ɾ����־��������������ʾ���ݡ�
	{
		//������ʾ���ͻ�����Ϣ
		m_cList_Client.DeleteAllItems();								//����б���Ϣ
		v_pIter=TelnetManager.v_mClient_Connect.begin();				//����������λ��
		while(v_pIter!=TelnetManager.v_mClient_Connect.end())			//����������
		{
			My_ShowList_Client(v_pIter->second.v_sIP,v_pIter->second.v_iSocket,v_pIter->second.v_sName);	//�б���ʾ
			v_pIter++;													//������++
		}
	}
	TelnetManager.v_cTelnet_Lock.Unlock();								//����
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ������пͻ��˷�����Ϣ
//	��    ����v_iFunctionCode�������룻v_sCommand����Ϣָ�
//	��    �ӣ���������(0x81,"")�����ɼ��������쳣��(0x85,"...")����BSC�ɼ��쳣��(0x86,"...")��
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_Send_Client_Alarm(unsigned int v_iCode ,CString v_sError)
{
	TelnetManager.v_cTelnet_Lock.Lock();						//����
	map<SOCKET,v_sClient_Connect>::iterator v_pIter;			//������
	v_pIter=TelnetManager.v_mClient_Connect.begin();			//����������λ��
	while(v_pIter!=TelnetManager.v_mClient_Connect.end())		//����������
	{
		Send_Command(v_pIter->second.v_iSocket,v_iCode,v_sError);	//����ָ��
		v_pIter++;												//������++
	}
	TelnetManager.v_cTelnet_Lock.Unlock();						//����
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�����ָ��
//	��    ����v_iFunctionCode�������룻v_sCommand����Ϣָ�
//	��    �ӣ���ָ�(0x03,"...")����������(0x81,"")�����ɼ��������쳣��(0x85,"...")����BSC�ɼ��쳣��(0x86,"...")��
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::Send_Command(SOCKET v_sClient_Socket,unsigned int v_iCode ,CString v_sCommand)
{
	u_char v_cSendData[1024];											//���ͻ�����
	v_cSendData[0] = 0x43;												//����ͷ
	v_cSendData[1] = 0x4D;												//����ͷ
	int v_iLen = 6 + v_sCommand.GetLength();							//���ݳ���
	v_cSendData[2] = v_iLen/256;										//���ݳ��ȣ���λ
	v_cSendData[3] = v_iLen%256;										//���ݳ��ȣ���λ
	v_cSendData[4] = v_iCode;											//������ ��01��������02����¼��03��ָ���
	u_char* v_pSend=&v_cSendData[5];									//ָ��λ��
	memcpy(v_pSend,v_sCommand,v_sCommand.GetLength());					//���ݴ���
	AddCheckBits(v_cSendData);											//����У��
	v_cLock_Send.Lock();												//����
	Send_Socket(v_sClient_Socket,(char*)v_cSendData,v_iLen);			//��������
	v_cLock_Send.Unlock();												//����	
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���������
//------------------------------------------------------------------------------------------------------
int CServer_DataDlg::Send_Socket(SOCKET v_sClient_Socket,char* v_sSend,int v_iLen)
{
	if (v_sClient_Socket!=NULL)											//SOCKET��Ч������
		send(v_sClient_Socket,v_sSend,v_iLen,0);						//���ʹ��󣿣���
	return true;
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�����У�麯��
//------------------------------------------------------------------------------------------------------
BOOL  CServer_DataDlg::AddCheckBits(u_char* sendData)
{
	int v_ilen=(u_char)sendData[2]*256+(u_char)sendData[3];				//���ݳ���
	if(v_ilen<6)														//��Ч���ݣ�����
		return FALSE;													//��Ч����
	u_char v_chSun=0x00;												//Ĭ��У���
	for (int i=0;i<v_ilen-1;i++)										//���ݱ���
		v_chSun+=sendData[i];											//���㣺У���
	sendData[v_ilen-1]=0xFF-v_chSun;									//���ã�У���
	return TRUE;														//��Ч����
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�Client�б�������һ��ͨ����Ϣ
//	��    ����v_sIP��Client IP��v_sName��
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Client(CString v_sIP,SOCKET v_iSocket,CString v_sName)
{
	int		vi,v_iIndex;												//��ʱ����
	CString v_sTemp;													//��ʱ����
	CString v_sCurTime;													//��ʱ����
	CTime	v_CurTime=CTime::GetCurrentTime();

	v_sCurTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	v_iIndex = m_cList_Client.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_cList_Client.SetItemText(v_iIndex,0," "+v_sTemp);					//���
	m_cList_Client.SetItemText(v_iIndex,1,v_sCurTime);					//����ʱ��
	m_cList_Client.SetItemText(v_iIndex,2,v_sIP);						//Client IP
	v_sTemp.Format("%d",v_iSocket);										//Client SOCKET
	m_cList_Client.SetItemText(v_iIndex,3,v_sTemp);						//Client SOCKET
	m_cList_Client.SetItemText(v_iIndex,4,v_sName);						//Client ����
	theApp.m_MyLog.WriteText("�ͻ����ӣ�"+v_sIP+"�� Socket"+v_sTemp+"\r\n");		//�ͻ�������Ϣ

	//�б��������ƣ�����1000����ɾ��300����
	m_cList_Client.SetRedraw(FALSE);									//�����½���
	if (m_cList_Client.GetItemCount()>=1000)							//�����¼������1000
	{
		for(vi=0;vi<300;vi++)											//ɾ��ǰ300��
			m_cList_Client.DeleteItem(0);								//ɾ����һ�����ݼ�¼
		for(vi=0;vi<m_cList_Client.GetItemCount();vi++)					//���·������
		{
			v_sTemp.Format("%04d",vi+1);
			m_cList_Client.SetItemText(vi,0," "+v_sTemp);
		}
	}
	m_cList_Client.SetRedraw(true);										//���½���
	m_cList_Client.EnsureVisible(m_cList_Client.GetItemCount()-1,false);//��ʾ����һ����¼
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���־�б�������һ��ͨ����Ϣ
//	��    ����v_sType�����ͣ�	v_sAlarm���澯��Ϣ��
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Log(CString v_sType,CString v_sAlarm)
{
	int		vi,v_iIndex;												//��ʱ����
	CString v_sTemp;													//��ʱ����
	CString v_sCurTime;													//��ʱ����
	CTime	v_CurTime=CTime::GetCurrentTime();

	v_sCurTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	v_iIndex = m_cList_Log.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_cList_Log.SetItemText(v_iIndex,0," "+v_sTemp);					//���
	m_cList_Log.SetItemText(v_iIndex,1,v_sCurTime);						//����ʱ��
	m_cList_Log.SetItemText(v_iIndex,2,v_sType);						//����
	m_cList_Log.SetItemText(v_iIndex,3,v_sAlarm);						//�澯��Ϣ

	//�б��������ƣ�����1000����ɾ��300����
	m_cList_Log.SetRedraw(FALSE);										//�����½���
	if (m_cList_Log.GetItemCount()>=1000)								//�����¼������1000
	{
		for(vi=0;vi<300;vi++)											//ɾ��ǰ300��
			m_cList_Log.DeleteItem(0);									//ɾ����һ�����ݼ�¼
		for(vi=0;vi<m_cList_Log.GetItemCount();vi++)					//���·������
		{
			v_sTemp.Format("%04d",vi+1);
			m_cList_Log.SetItemText(vi,0," "+v_sTemp);
		}
	}
	m_cList_Log.SetRedraw(true);										//���½���
	m_cList_Log.EnsureVisible(m_cList_Log.GetItemCount()-1,false);		//��ʾ����һ����¼
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�ָ���б�������һ��ͨ����Ϣ
//	��    ����v_sIP���ͻ���IP��	v_sName���ͻ������ƣ�	v_sBSC��BSC��	v_sCommand��ָ�
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Command(CString v_sIP,CString v_sName,CString v_sBSC,CString v_sCommand)
{
	int		vi,v_iIndex;												//��ʱ����
	CString v_sTemp;													//��ʱ����
	CString v_sCurTime;													//��ʱ����
	CTime	v_CurTime=CTime::GetCurrentTime();

	v_sCurTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	v_iIndex = m_cList_Command.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_cList_Command.SetItemText(v_iIndex,0," "+v_sTemp);				//���
	m_cList_Command.SetItemText(v_iIndex,1,v_sCurTime);					//ʱ��
	m_cList_Command.SetItemText(v_iIndex,2,v_sIP);						//IP
	m_cList_Command.SetItemText(v_iIndex,3,v_sName);					//����
	m_cList_Command.SetItemText(v_iIndex,4,v_sBSC);						//BSC
	m_cList_Command.SetItemText(v_iIndex,5,v_sCommand);					//ָ��

	//�б��������ƣ�����1000����ɾ��300����
	m_cList_Command.SetRedraw(FALSE);									//�����½���
	if (m_cList_Command.GetItemCount()>=1000)							//�����¼������1000
	{
		for(vi=0;vi<300;vi++)											//ɾ��ǰ300��
			m_cList_Command.DeleteItem(0);								//ɾ����һ�����ݼ�¼
		for(vi=0;vi<m_cList_Command.GetItemCount();vi++)				//���·������
		{
			v_sTemp.Format("%04d",vi+1);
			m_cList_Command.SetItemText(vi,0," "+v_sTemp);
		}
	}
	m_cList_Command.SetRedraw(true);										//���½���
	m_cList_Command.EnsureVisible(m_cList_Command.GetItemCount()-1,false);	//��ʾ����һ����¼
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�ָ��ִ�н����ʾ
//	��    ����v_tpTimespan����ʱ��	v_sType�����ͣ�	v_sLog��Log��Ϣ��
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Command_Text(CString v_sText)
{
	CString		v_sTemp;												//��ʱ����
	COleDateTime v_CurTime_Text=COleDateTime::GetCurrentTime();			//��ǰʱ��

	COleDateTimeSpan v_tSpan_Text=v_CurTime_Text-v_tDate;				//��ʱ
	int		v_iSeconds=v_tSpan_Text.GetTotalSeconds();					//��
	v_sTemp.Format("%d",v_iSeconds);									//��ʱ
	if (m_cList_Command.GetItemCount()>0)
	{
		m_cList_Command.SetItemText(m_cList_Command.GetItemCount()-1,6,v_sTemp);	//��ʱ��ָ��ִ��
		m_cList_Command.SetItemText(m_cList_Command.GetItemCount()-1,7,v_sText);	//ָ��ִ�н��
	}
}
//------------------------------------------------------------------------------------------------------			
//	�������ܣ�Bug�б�������һ��ͨ����Ϣ
//	��    ����v_sBug_Type���쳣���ͣ�	v_sBug���쳣��Ϣ
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Bug(CString v_sBug_Type,CString v_sBug)
{
	int		vi,v_iIndex;												//��ʱ����
	CString v_sTemp;													//��ʱ����
	CString v_sCurTime;													//��ʱ����
	CTime	v_CurTime=CTime::GetCurrentTime();

	v_sCurTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	v_iIndex = m_cList_Bug.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_cList_Bug.SetItemText(v_iIndex,0," "+v_sTemp);					//���
	m_cList_Bug.SetItemText(v_iIndex,1,v_sCurTime);						//����ʱ��
	m_cList_Bug.SetItemText(v_iIndex,2,v_sBug_Type);					//�쳣����
	m_cList_Bug.SetItemText(v_iIndex,3,v_sBug);							//�쳣��Ϣ

	//�б��������ƣ�����1000����ɾ��300����
	m_cList_Bug.SetRedraw(FALSE);										//�����½���
	if (m_cList_Bug.GetItemCount()>=1000)								//�����¼������1000
	{
		for(vi=0;vi<300;vi++)											//ɾ��ǰ300��
			m_cList_Bug.DeleteItem(0);									//ɾ����һ�����ݼ�¼
		for(vi=0;vi<m_cList_Bug.GetItemCount();vi++)					//���·������
		{
			v_sTemp.Format("%04d",vi+1);
			m_cList_Bug.SetItemText(vi,0," "+v_sTemp);
		}
	}
	m_cList_Bug.SetRedraw(true);										//���½���
	m_cList_Bug.EnsureVisible(m_cList_Bug.GetItemCount()-1,false);		//��ʾ����һ����¼
}

//------------------------------------------------------------------------------------------------------
//	�������ܣ�SQLִ�к���
//	��    ����v_sSql��SQL���
//------------------------------------------------------------------------------------------------------
bool CServer_DataDlg::ExecuteSQL(CString v_sSql)
{
	// TODO: Add your control notification handler code here
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	try
	{
		// �������ݿ�
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(120000);
		v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//ִ�����ݿ����
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		return true;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�ر����ݿ�
		CString v_sError = (char *)e.Description();						//��ã�������Ϣ
		My_ShowList_Bug("�������ݱ�","�쳣��"+v_sError);				//��ʾ���쳣
		theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+v_sSql+"\r\n");	//log
		return false;
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���ʾ�ļ�������Ϣ
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::ShowListCommand(CString sFileName,CString sMsg)
{
	int iIndex,vi;
	CString sTemp,sTime;
	CTime	v_CurTime=CTime::GetCurrentTime();
	iIndex = m_cList_Command.InsertItem(0xffff,_T("0"));
	sTemp.Format("%04d",iIndex+1);
	sTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	m_cList_Command.SetItemText(iIndex,0," "+sTemp);					//���
	m_cList_Command.SetItemText(iIndex,1,sTime);					//����ʱ��
	m_cList_Command.SetItemText(iIndex,2,sFileName);				    //�ļ�����
	m_cList_Command.SetItemText(iIndex,3,sMsg);				    //�ļ��������

	//�б��������ƣ�����1000����ɾ��300����
	m_cList_Command.SetRedraw(FALSE);									//�����½���
	if (m_cList_Command.GetItemCount()>=1000)							//�����¼������1000
	{
		for(vi=0;vi<300;vi++)											//ɾ��ǰ300��
			m_cList_Command.DeleteItem(0);								//ɾ����һ�����ݼ�¼
		for(vi=0;vi<m_cList_Command.GetItemCount();vi++)				//���·������
		{
			sTemp.Format("%04d",vi+1);
			m_cList_Command.SetItemText(vi,0," "+sTemp);
		}
	}
	m_cList_Command.SetRedraw(true);										//���½���
	m_cList_Command.EnsureVisible(m_cList_Command.GetItemCount()-1,false);	//��ʾ����һ����¼
	
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���ʾ��ϸ�ļ�������Ϣ
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::ShowListLog(CString sPanTime,CString sFileName,CString sMsg)
{
	int iIndex,vi;
	CString sTemp,sTime;
	CTime	v_CurTime=CTime::GetCurrentTime();
	v_CurTime=CTime::GetCurrentTime();                                  //��õ�ǰʱ��
	sTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");

	iIndex = m_cList_Log.InsertItem(0xffff,_T("0"));
	sTemp.Format("%04d",iIndex+1);
	m_cList_Log.SetItemText(iIndex,0," "+sTemp);					//���	
	m_cList_Log.SetItemText(iIndex,1,sTime);					    //����ʱ��
	m_cList_Log.SetItemText(iIndex,2,sPanTime);					    //������ʱ��
	m_cList_Log.SetItemText(iIndex,3,sFileName);				    //�ļ�����
	m_cList_Log.SetItemText(iIndex,4,sMsg);				        //�ļ��������

	//�б��������ƣ�����1000����ɾ��300����
	m_cList_Log.SetRedraw(FALSE);										//�����½���
	if (m_cList_Log.GetItemCount()>=1000)								//�����¼������1000
	{
		for(vi=0;vi<300;vi++)											//ɾ��ǰ300��
			m_cList_Log.DeleteItem(0);									//ɾ����һ�����ݼ�¼
		for(vi=0;vi<m_cList_Log.GetItemCount();vi++)					//���·������
		{
			sTemp.Format("%04d",vi+1);
			m_cList_Log.SetItemText(vi,0," "+sTemp);
		}
	}
	m_cList_Log.SetRedraw(true);										//���½���
	m_cList_Log.EnsureVisible(m_cList_Log.GetItemCount()-1,false);		//��ʾ����һ����¼
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ���ѹGZ�ļ�
//��    ����szFileName��ѹ���ļ�·����sFileName����ѹ�ļ�·��
//------------------------------------------------------------------------------------------------------
BOOL CServer_DataDlg::PMArchive(CString szFileName,CString sFileName)
{
	CDecompressClass	    m_Decompress;									//ʵ����
	CString m_szFileName;
	BOOL m_fIsFile;
	char* pBuffer;														    //������ָ��
	int nLen;
	//�ļ���ѹ
	m_szFileName="";
	m_fIsFile=false;
	m_Decompress.CloseArchive();										    //�رս�ѹ����
	if (!m_Decompress.OpenArchive(szFileName))	                            //�򿪡���ѹ�ļ�������
	{																	    //��ѹʧ��
		return FALSE;													    //����
	}
	//��ȡ���ļ���Ϣ															//���ݳ���
	if (!m_Decompress.GetArchiveFile(0,									    //�ļ�ͷ
		&pBuffer,														    //��ȡ������ָ��
		nLen,															    //��ȡ�����ݳ���
		m_fIsFile,														    //
		m_szFileName))														//�ļ���Ϣ��ȡ�ɹ�������
	{
		My_ShowList_Bug("��ѹ�ļ�","�ļ���Ϣ�쳣��"+sFileName);				//��ʾ���쳣
		return FALSE;													    //����
	}
	//�����ѹ��XML�ļ�
	if(!v_fWriter.Open(sFileName, CFile::modeCreate | CFile::modeWrite))	//���ļ����粻���������ɣ������������ļ���
	{
		My_ShowList_Bug("��ѹѹ���ļ�","�ļ������쳣��"+sFileName);			//��ʾ���쳣
		//ʧ������
		return FALSE;														//����
	}					
	v_fWriter.Write(pBuffer,nLen);											//д���ļ������ݣ�����ָ�롢���ݳ��ȡ�
	v_fWriter.Close();														//�ļ��ر�
	m_Decompress.CloseArchive();											//�ļ��ر�[ѹ���ļ�]
	return TRUE;
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�������Ʋ���
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedSystemParm()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	list<map_ObjType>::iterator iterator_ObjType;
	queue<CString>  temp_file_queue;
	stack<CString>  temp_sql_stack;
	CString temp;

	//ϵͳ����
	DB_maintenance_lock_.Lock();
	DB_maintenance_.Init();
	DB_maintenance_lock_.Unlock();
	PARMDialog.PmLTEMonth = DB_maintenance_.pm_save_month_;
	PARMDialog.PmLTEDayFTP = DB_maintenance_.log_save_day_;
	PARMDialog.PmLTEDay = DB_maintenance_.pm_save_day_;
	PARMDialog.PmLTETimeData = DB_maintenance_.granularity_time_;

	//��ȡ�������ܶ��С�1��
	basePm_lock_.Lock();	
	PARMDialog.basePm_queue_.clear();
	iterator_ObjType = basePm_queue_.begin();
	while(iterator_ObjType != basePm_queue_.end())
	{
		temp = iterator_ObjType->begin()->first;
		PARMDialog.basePm_queue_.push_back(temp);
		iterator_ObjType++;
	}
	basePm_lock_.Unlock();

	//��ȡ�������ܶ��С�2��
	calculatePm_lock_.Lock();
	PARMDialog.calculatePm_queue_.clear();
	iterator_ObjType = calculatePm_queue_.begin();
	while(iterator_ObjType != calculatePm_queue_.end())
	{
		temp = iterator_ObjType->begin()->first;
		PARMDialog.calculatePm_queue_.push_back(temp);
		iterator_ObjType++;
	}
	calculatePm_lock_.Unlock();

	//��ȡȫ�����ܶ��С�3��
	networkPm_lock_.Lock();			
	PARMDialog.networkPm_queue_.clear();
	iterator_ObjType = networkPm_queue_.begin();
	while(iterator_ObjType != networkPm_queue_.end())
	{
		temp = iterator_ObjType->begin()->first;
		PARMDialog.networkPm_queue_.push_back(temp);
		iterator_ObjType++;
	}	
	networkPm_lock_.Unlock();

	//��ȡ�������ܶ��С�4��
	regionPm_lock_.Lock();
	PARMDialog.regionPm_queue_.clear();
	iterator_ObjType = regionPm_queue_.begin();
	while(iterator_ObjType != regionPm_queue_.end())
	{
		temp = iterator_ObjType->begin()->first;
		PARMDialog.regionPm_queue_.push_back(temp);
		iterator_ObjType++;
	}	
	regionPm_lock_.Unlock();

	//��ȡ�ļ����С�5��
	v_cLock_FileName.Lock();	
	PARMDialog.file_queue_.clear();
	temp_file_queue = file_queue_;
	while(temp_file_queue.size())
	{
		temp = temp_file_queue.front();
		PARMDialog.file_queue_.push_back(temp);
		temp_file_queue.pop();
	}	
	v_cLock_FileName.Unlock();

	//��ȡsql���С�6��
	sql_lock_.Lock();
	PARMDialog.sql_queue_.clear();
	temp_sql_stack = sql_stack_;
	while(temp_sql_stack.size())
	{
		temp = temp_sql_stack.top();
		PARMDialog.sql_queue_.push_back(temp);
		temp_sql_stack.pop();
	}	
	sql_lock_.Unlock();

	//��ȡСʱ�������ܶ��С�7��
	hour_basePm_lock_.Lock();	
	PARMDialog.hour_basePm_queue_.clear();
	iterator_ObjType = hour_basePm_queue_.begin();
	while(iterator_ObjType != hour_basePm_queue_.end())
	{
		temp = iterator_ObjType->begin()->first;
		PARMDialog.hour_basePm_queue_.push_back(temp);
		iterator_ObjType++;
	}
	hour_basePm_lock_.Unlock();

	//��ȡСʱ�������ܶ��С�8��
	hour_calculatePm_lock_.Lock();	
	PARMDialog.hour_calculatePm_queue_.clear();
	iterator_ObjType = hour_calculatePm_queue_.begin();
	while(iterator_ObjType != hour_calculatePm_queue_.end())
	{
		temp = iterator_ObjType->begin()->first;
		PARMDialog.hour_calculatePm_queue_.push_back(temp);
		iterator_ObjType++;
	}
	hour_calculatePm_lock_.Unlock();

	//��ȡ��Ϊ���ܶ��С�9��
	hwPm_lock_.Lock();			
	PARMDialog.hwPm_queue_.clear();
	iterator_ObjType = hwPm_queue_.begin();
	while(iterator_ObjType != hwPm_queue_.end())
	{
		temp = iterator_ObjType->begin()->first;
		PARMDialog.hwPm_queue_.push_back(temp);
		iterator_ObjType++;
	}	
	hwPm_lock_.Unlock();

	//��ȡ�������ܶ��С�10��
	ztePm_lock_.Lock();			
	PARMDialog.ztePm_queue_.clear();
	iterator_ObjType = ztePm_queue_.begin();
	while(iterator_ObjType != ztePm_queue_.end())
	{
		temp = iterator_ObjType->begin()->first;
		PARMDialog.ztePm_queue_.push_back(temp);
		iterator_ObjType++;
	}	
	ztePm_lock_.Unlock();

	PARMDialog.DoModal();
}

//------------------------------------------------------------------------------------------------------			
//��������:���ݱ�����ʼ��
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::Init()
{
	//�жϴ�������������
	if (GetDriveType("F:") == DRIVE_NO_ROOT_DIR)
	{
		system(_T("net use F: \\\\192.168.1.101\\f  \"Aa123456\" /user:\"DLPM\""));
	}

	DB_maintenance_lock_.Lock();
	DB_maintenance_.Init();
	DB_maintenance_lock_.Unlock();
	cell_filter_.Init();
	network_filter_.Init();
	region_filter_.Init();
	hw_filter_.Init();
	zte_filter_.Init();
	base_Pm_[0].Init();
	base_Pm_[1].Init();
	hour_base_Pm_.Init();
	calculation_Pm_[0].Init();
	hour_calculation_Pm_.Init();
	network_Pm_.Init();
	region_Pm_.Init();
	hw_Pm_.Init();
	zte_Pm_.Init();
	//���С�����Ӧ��Ψһ����
	GetCellMap(cellname_map_ID_);
}

//------------------------------------------------------------------------------------------------------			
//��������:���ݱ�������
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::Updata()
{
	cell_filter_.Init();
	network_filter_.Init();
	region_filter_.Init();
	hw_filter_.Init();
	zte_filter_.Init();
	network_Pm_.Updata();
	region_Pm_.Updata();
	hw_Pm_.Updata();
	zte_Pm_.Updata();
	base_Pm_[0].Updata();
	base_Pm_[1].Updata();
	calculation_Pm_[0].Updata();
	hour_calculation_Pm_.Updata();
	//���С�����Ӧ��Ψһ����
	GetCellMap(cellname_map_ID_);
}

//--------------------------------------------------------------------------------------------------------------------------			
//��������:ִ��bulk insertջ
//---------------------------------------------------------------------------------------------------------------------------
void CServer_DataDlg::ExecuteBulkInsertStack()
{
	CString sTemp;
	sql_lock_.Lock();
	sTemp = sql_stack_.top();
	sql_stack_.pop();
	sql_lock_.Unlock();	
    ExecuteSQL(sTemp);
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�����ļ�������ʱ�䡾������szFileNameѹ���ļ����ƣ�sFileTime�ļ�����ȡ��ʱ����Ϣ��
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::GetFileTime(const CString& szFileName,CString &sFileTime)
{
	if (szFileName.Left(1)=="E")                     //HW PM
	{
		sFileTime=szFileName.Mid(28,13);
		sFileTime.Replace("-"," ");
		sFileTime.Insert(4,"-");
		sFileTime.Insert(7,"-");
		sFileTime.Insert(13,":");
		sFileTime.Insert(16,":00");

	} 
	else
	{
		sFileTime=szFileName.Mid(33,13);            //ZTE PM
		sFileTime.Replace("-"," ");
		sFileTime.Replace("-"," ");
		sFileTime.Insert(4,"-");
		sFileTime.Insert(7,"-");
		sFileTime.Insert(13,":");
		sFileTime.Insert(16,":00");
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ�С������ӳ��Ϊ����
//��   ���� v_mObjType :�ӿ�С�����ݣ�cellname_map_ID��С������ӳ��
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::MapCellNameToID(map_ObjType &v_mObjType,map_Counter &cellname_map_ID)
{
	map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
	list_Object::iterator	iterator_Object;							//countС��������
	map<CString,CString>::iterator iterator_cell;                       //С����ѯ������
	iterator_objtype = v_mObjType.begin();
	while(iterator_objtype != v_mObjType.end())
	{
		iterator_Object = iterator_objtype->second.begin();
		while(iterator_Object != iterator_objtype->second.end())
		{
			iterator_Object->v_sObject_ID.Replace("��","(");
			iterator_Object->v_sObject_ID.Replace("��",")");
			iterator_cell = cellname_map_ID.find(iterator_Object->v_sObject_ID);
			if (iterator_cell != cellname_map_ID.end())
			{
				iterator_Object->v_sObject_ID = iterator_cell->second;
				iterator_Object++;
			}
			else
			{
				iterator_objtype->second.erase(iterator_Object ++);

			}			
		}
		iterator_objtype ++;
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  ��������:���С�����Ӧ��Ψһ����
//  ��    ��:cellname_map_ID : С���������
//-----------------------------------------------------------------------------------------------------------------
void CServer_DataDlg:: GetCellMap(map<CString,CString> &cellname_map_ID)
{
	_RecordsetPtr v_pRecordset;											  //��¼��
	CMyADOConn v_cAdoConn;												  //���ݿ�ʵ��
	CString		   sql,sTemp,Key,value;							      //��ʱ����
	int            iTemp;
	try
	{
		sql="SELECT zCell,zCell_ID FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		          //ִ��SELETE���	
		while(!v_pRecordset->adoEOF)
		{
			sTemp = v_cAdoConn.GetField_String("zCell");
			Key = sTemp;
			iTemp = v_cAdoConn.GetField_Int("zCell_ID");
			value.Format("%d",iTemp);
			cellname_map_ID.insert(pair<CString,CString>(Key,value));
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										  //�ر����ݿ�

		CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sError+"\r\n");	              //log
	}
}

//------------------------------------------------------------------------------------------------------
//	�������ܣ��ӿڱ�������
//  ��    ����ObjType���ӿ�
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::ResetObjType(map_ObjType &ObjType)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjTypeָʾ��
	list_Object::iterator	v_pIterator_Object;							//list_Object������
	v_pIterator_ObjType = ObjType.begin();
	while(v_pIterator_ObjType != ObjType.end())
	{
		v_pIterator_Object=v_pIterator_ObjType->second.begin();
		while(v_pIterator_Object!=v_pIterator_ObjType->second.end())
		{
			v_pIterator_Object->v_mCounter.clear();						//Counter���������
			v_pIterator_Object++;
		}
		v_pIterator_ObjType->second.clear();							//Object�������
		v_pIterator_ObjType++;
	}
	ObjType.clear();													//ObjType���������
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	�������ܣ�����ļ���Ϣ
//  ��   ����socket_file_name:socket��������ļ����ƣ�zip_file_name:ѹ���ļ����ƣ�zip_file_path:ѹ���ļ�·����xml_file_name��xml�ļ����ƣ�xml_file_path����ѹ�ļ�·����file_name���ļ����ƣ�file_time���ļ�ʱ��;file_type:�ļ����͡����˻�Ϊ��
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CServer_DataDlg::GetFileInformation(CString socket_file_name,CString &zip_file_name,CString &zip_file_path,CString &xml_file_name,CString &xml_file_path,CString &file_name,CString &file_time,CString &file_type)
{
	CString temp;
	//ѹ���ļ�·��
	temp = socket_file_name;
	temp.Delete(0,3);
	zip_file_path = theApp.a_sPm_Path_ + "\\" + temp;  

	//���ѹ���ļ�����
	temp = zip_file_path;
	temp.Delete(0,24);
	zip_file_name = temp;

	//��ѹXML�ļ����Ƽ�·��
	xml_file_name = "Pmfile.xml";
	xml_file_path = ".\\DATA\\" + xml_file_name;
	file_name = zip_file_name;
	file_name.Delete(file_name.GetLength()-7,7);

	//����ļ�ʱ��
	GetFileTime(zip_file_name,file_time);

	//�ļ������ж�(���˻��ǻ�Ϊ)
	if (zip_file_name.Left(1) == "C")
		file_type = "����";
	else
		file_type = "��Ϊ";
}

//-------------------------------------------------------------------------------------------------------------------		
//��������:�����ʱ
//��   ����key:���ƣ�message;��ʾ��Ϣ;before_time:��ʼʱ��
//-------------------------------------------------------------------------------------------------------------------
void CServer_DataDlg::KeepTime(CString key,CString message,COleDateTime &before_time)
{
	CString span_time;
	COleDateTimeSpan Ole_span_time;
	Ole_span_time = COleDateTime::GetCurrentTime() - before_time;//������ʱ
	span_time = Ole_span_time.Format("%H:%M:%S");
	ShowListLog(span_time,key,message);
	if (message == "���15�����������ݴ�������ʱ")
	{
		theApp.m_MyLog.WriteText(key + "�� " + span_time + "\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------
//	�������ܣ�ɾ���ظ���������
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::DeleteNextGranularityData(CString granlarity)
{
	_RecordsetPtr   v_pRecordset;									                         //��¼��
	CMyADOConn      v_cAdoConn;												                 //���ݿ�ʵ��
	CString			sql,sql_key="",sql_value="",time;							             //��ʱ����
	COleDateTime    delete_next_granularity;		                                         //������ʱ
	CString         sdelete_next_granularity;
	delete_next_granularity.ParseDateTime(granlarity);
	delete_next_granularity += COleDateTimeSpan(0,0,15,0);
	sdelete_next_granularity = delete_next_granularity.Format("%Y-%m-%d %H:%M");
	map_Counter::iterator	iterator_whole_network_performance_updata;				         //count���ܵ�����
	try
	{   
		time = sdelete_next_granularity.Mid(0,4)+sdelete_next_granularity.Mid(4,4);
		time.Replace("-","");
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);			
		v_cAdoConn.SetCommandTimeout(60000);
		sql = "delete from PM_Alarm where ztime = '" + sdelete_next_granularity + "'";	
		v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
		sql = "delete from Pm_LTE_Region where ztime = '" + sdelete_next_granularity + "'";			
		v_cAdoConn.ExecuteSQL((_bstr_t)sql);	
		//ɾ���ظ�Сʱ�������ݵ�����
		{
			int control_count(0);
			COleDateTime Ole_temp_time;
			Ole_temp_time.ParseDateTime(sdelete_next_granularity);
			int day = Ole_temp_time.GetDay();
			control_count = (day - 1) / 5;
			switch (control_count)
			{
			case 0:
				sql = "delete from Pm_LTE_Cal_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_MAC_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PDCP_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PHY_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_RRC_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_THO_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_TXT_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				break;
			case 1:
				sql = "delete from Pm_LTE_Cal_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_MAC_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PDCP_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PHY_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_RRC_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_THO_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_TXT_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				break;
			case 2:
				sql = "delete from Pm_LTE_Cal_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_MAC_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PDCP_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PHY_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_RRC_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_THO_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_TXT_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				break;
			case 3:
				sql = "delete from Pm_LTE_Cal_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_MAC_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PDCP_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PHY_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_RRC_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_THO_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_TXT_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				break;
			case 4:
				sql = "delete from Pm_LTE_Cal_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_MAC_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PDCP_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PHY_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_RRC_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_THO_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_TXT_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				break;
			default:
				sql = "delete from Pm_LTE_Cal_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_MAC_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PDCP_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_PHY_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_RRC_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_THO_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				sql = "delete from Pm_LTE_TXT_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
				break;
			}

		}
		//ɾ���ظ�15�����������ݵ�����
		{
			CString temp_time = sdelete_next_granularity;
			temp_time.Replace("-","");
			temp_time = temp_time.Left(8);
			sql = "delete from Pm_LTE_Cal_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
			sql = "delete from Pm_LTE_MAC_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
			sql = "delete from Pm_LTE_PDCP_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
			sql = "delete from Pm_LTE_PHY_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
			sql = "delete from Pm_LTE_RRC_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
			sql = "delete from Pm_LTE_THO_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
			sql = "delete from Pm_LTE_TXT_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
		}
		v_cAdoConn.ExitConnect();										//�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�ر����ݿ�
		CString v_sError = (char *)e.Description();						//��ã�������Ϣ
		My_ShowList_Bug("�������ݱ�","�쳣��"+v_sError);				    //��ʾ���쳣
		theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	    //log
	}
}

//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------
