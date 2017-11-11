
// Server_DataDlg.cpp : 实现文件
#include "stdafx.h"
#include "Server_Data.h"
#include "Server_DataDlg.h"
#include "Exit_Confirm.h"												//退出确认
#include "COM_About.h"
#include "MyTran.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CServer_DataDlg 对话框
//------------------------------------------------------------------------------------------------------			
//	函数功能：构造函数
//------------------------------------------------------------------------------------------------------
CServer_DataDlg::CServer_DataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServer_DataDlg::IDD, pParent),TelnetManager((LPVOID)this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	v_sServer_Socket=NULL;												//采集端SOCKET
	v_iServer_Sum=0;													//采集端计数器
	v_bCommand_Flag=0;													//复位：指令发送标记
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：析构函数
//------------------------------------------------------------------------------------------------------
CServer_DataDlg::~CServer_DataDlg()
{
	v_lConnect.clear();													//链表清除
	v_lCommand.clear();													//链表清除
	if(v_sServer_Socket!=NULL)											//采集端SOCKET有效？？？
		closesocket(v_sServer_Socket);									//关闭采集端SOCKET
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：屏幕变量
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
//	函数功能：消息映射
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
// CServer_DataDlg 消息处理程序

//------------------------------------------------------------------------------------------------------			
//	函数功能：如果向对话框添加最小化按钮，则需要下面的代码来绘制该图标。
//			  对于使用文档/视图模型的 MFC 应用程序，这将由框架自动完成。
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：当用户拖动最小化窗口时系统调用此函数取得光标显示。
//------------------------------------------------------------------------------------------------------
HCURSOR CServer_DataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：对话窗初始化
//------------------------------------------------------------------------------------------------------
BOOL CServer_DataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	CString v_sTemp,sValue,sKey;
	//  执行此操作
	SetIcon(m_hIcon, TRUE);												//设置大图标
	SetIcon(m_hIcon, FALSE);											//设置小图标
	// TODO: 在此添加额外的初始化代码

	//列表初始化：客户端信息
	m_cList_Client.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_cList_Client.InsertColumn(0,"序号",LVCFMT_LEFT,40);				//
	m_cList_Client.InsertColumn(1,"时间",LVCFMT_CENTER,120);			//
	m_cList_Client.InsertColumn(2,"IP",LVCFMT_LEFT,120);				//
	m_cList_Client.InsertColumn(3,"SOCKET",LVCFMT_LEFT,100);			//
	m_cList_Client.InsertColumn(4,"姓名",LVCFMT_LEFT,100);				//

	//列表初始化：异常信息
	m_cList_Bug.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_cList_Bug.InsertColumn(0,"序号",LVCFMT_LEFT,40);					//
	m_cList_Bug.InsertColumn(1,"时间",LVCFMT_CENTER,120);				//
	m_cList_Bug.InsertColumn(2,"异常类型",LVCFMT_LEFT,120);				//
	m_cList_Bug.InsertColumn(3,"异常信息",LVCFMT_LEFT,300);				//

	//列表初始化：文件信息
	m_cList_Command.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_cList_Command.InsertColumn(0,"序号",LVCFMT_LEFT,40);				//
	m_cList_Command.InsertColumn(1,"时间",LVCFMT_CENTER,125);			//
	m_cList_Command.InsertColumn(2,"文件信息",LVCFMT_LEFT,360);			//
	m_cList_Command.InsertColumn(3,"文件处理信息",LVCFMT_LEFT,600);		//
	
	//列表初始化：日志文件处理信息
	m_cList_Log.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES);
	m_cList_Log.InsertColumn(0,"序号",LVCFMT_LEFT,40);
	m_cList_Log.InsertColumn(1,"时间",LVCFMT_CENTER,140);
	m_cList_Log.InsertColumn(2,"历时",LVCFMT_CENTER,80);
	m_cList_Log.InsertColumn(3,"文件信息",LVCFMT_LEFT,360);
	m_cList_Log.InsertColumn(4,"文件处理信息",LVCFMT_LEFT,600);

	//分窗
	CRect	v_cRect;													//矩形
	CWnd*	pWnd = GetDlgItem(IDC_STATIC_SLIT_PANE);					//获得：指针
	pWnd->GetWindowRect(&v_cRect);										//获得：尺寸
	ScreenToClient(&v_cRect);											//坐标变换
	pWnd->DestroyWindow();												//销毁：指针
	//	this creates pane containing list control on the top and edit on the bottom
	m_SplitterPane_Left.Create(										//垂直线：上
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN|SS_HORIZ, 		 
		this,					// the parent of the splitter pane
		&m_cList_Client,		// Top pane
		&m_cList_Log,			// Bottom pane
		IDC_Report_VERT1_Time,	// this ID is used for saving/restoring splitter position and therefore it must be unique !
		v_cRect					// dimensions of the splitter pane
		);
	m_SplitterPane_Left.m_bVertSplitter_Parent=true;					//垂直分隔符为父窗
	m_SplitterPane_Right.Create(										//水平线
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN|SS_HORIZ,		 
		this,					// the parent of the splitter pane
		&m_cList_Command,		// Left pane
		&m_cList_Bug,			// Right pane
		IDC_Report_VERT2_Time,	// this ID is used for saving/restoring splitter position and it must be unique !
		v_cRect					// dimensions of the splitter pane
		);
	m_SplitterPane_Right.m_bVertSplitter_Parent=true;					//垂直分隔符为父窗
	m_SplitterPane_Main.Create(											//水平线
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN|SS_VERT,		 
		this,					// the parent of the splitter pane
		&m_SplitterPane_Left,	// Left pane
		&m_SplitterPane_Right,	// Right pane
		IDC_MO_HORIZ_Time,		// this ID is used for saving/restoring splitter position and it must be unique !
		v_cRect					// dimensions of the splitter pane
		);

	//定时器 
	SetTimer(1,1000,NULL);												//定时：1s
	SetTimer(2,60*1000,NULL);											//定时：1min

	//变量初始化
	v_iThread_Exit=0;													//线程状态：未启动
	v_bDlg_Max=true;													//最大化有效
	m_cStart.EnableWindow(false);										//无效
	m_cStop.EnableWindow(false);										//无效
	m_cServer.EnableWindow(false);										//无效
	m_cinformation_monitor.SetCheck(0);

	//建立：文件目录
	CFileFind	finder;
	CString v_sPath_Data=theApp.m_sPath+"\\DATA";						//文件目录
	if (!finder.FindFile(v_sPath_Data))									//存在？
	{
		if (!::CreateDirectory(v_sPath_Data, NULL))						//不存在：创建目录
		{
			MessageBox("目录创建失败："+v_sPath_Data);					//失败
			return false;
		}
	}

	//设置信号量[软件只能运行一次]
	::SetProp(m_hWnd,theApp.m_sAppName,(HANDLE)1);						//设置信号量 [在指定窗口的属性表中增加一个新项]

	//最小化窗体
	ShowWindow( SW_SHOWMINIMIZED );										//最小化窗体

	//初始化
	v_bCommamd=false;													//复位：指令超时标志
	v_bClient=false;													//复位：客户端处理标志
	v_bTj=false;														//复位：统计标志
	m_cStart.EnableWindow(true);										//无效
	v_sTemp.Format("%d",theApp.a_iAlarmNotifyPort);                     //显示端口
	m_cPort.SetWindowText(v_sTemp);										//显示：监听端口
	global_control_ = false;
	return TRUE;														// 除非将焦点设置到控件，否则返回 TRUE
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：调整窗体尺寸
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	if ( m_SplitterPane_Main.GetSafeHwnd() ){							//分窗尺寸调整
		CRect v_cRect;
		GetClientRect( &v_cRect );
		v_cRect.DeflateRect(10,10);
		v_cRect.top+=45;
		m_SplitterPane_Main.MoveWindow(&v_cRect);
	}
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：关闭程序
//------------------------------------------------------------------------------------------------------			
void CServer_DataDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//线程运行？？？
	if (v_iThread_Exit==1)
	{
		//设置：强制退出
		if(MessageBox("是否退出告警获取  ？","退出确认",MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
			v_iThread_Exit=2;											//线程状态：强制退出线程
	}
	else
	{
		//系统退出确认
		CExit_Confirm	v_cDlg;											//实例化
		CDialog::OnOK();											//退出
		//移除信号量[软件只能运行一次]
		::RemoveProp(m_hWnd,theApp.m_sAppName);						//移除信号量
	}
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：定时器
//	工作过程：【1s：窗体最大化】【1min：掉线客户端删除】
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	COleDateTime v_tDate_Timer=COleDateTime::GetCurrentTime();			//获取：当前时间

	switch(nIDEvent)													//定时事件
	{
	case 1:																//1s定时器 [最大化窗体、发送指令执行超时处理]
		//第一步：最大化窗体
		if (v_bDlg_Max)
		{
			v_bDlg_Max=false;											//最大化无效
			ShowWindow( SW_SHOWMAXIMIZED );								//最大化窗体

 			if(theApp.a_auto_start_.Compare("YES") == 0){
			OnBnClickedOk();
			}
		}

		//第二步：指令执行超时处理
		if (v_bCommand_Flag)											//指令执行？？？
		{
			v_iCommand_Sum++;											//指令执行计数器++
			if (v_iCommand_Sum>10)										//超时？？？【指令无返回】
				v_bCommamd=true;										//置位：指令超时标志
		}
		break;
	case 2:																//1m定时器：任务分配
		if (v_iThread_Exit==1)											//获取开始？？？
			v_bClient=true;												//置位：客户端处理标志【超时退出】

		if (v_tDate_Timer.GetMinute()%15==0)							//15min/次
			v_bTj=true;													//置位：统计标志
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：客户导出
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedFile3()
{
	// TODO: 在此添加控件通知处理程序代码
	CMyExcel MyExcel;
	MyExcel.Show(&m_cList_Client);										//导出Excel
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：指令导出
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedOutput2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMyExcel MyExcel;
	MyExcel.Show(&m_cList_Command);										//导出Excel
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：异常导出
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedFile2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMyExcel MyExcel;
	MyExcel.Show(&m_cList_Bug);											//导出Excel
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：关于...
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CCOM_About dlg;														//主窗体
	dlg.DoModal();														//打开：主窗体
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：结束工作
//------------------------------------------------------------------------------------------------------			
void CServer_DataDlg::OnBnClickedOk2()
{
	// TODO: 在此添加控件通知处理程序代码
	v_iThread_Exit=2;													//线程状态：强制退出线程
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：开始工作
//------------------------------------------------------------------------------------------------------			
void CServer_DataDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CTime v_tCurren=CTime::GetCurrentTime();							//获得：当前时间
	if (v_iThread_Exit==0)
	{
		//到期处理
		if(v_tCurren>theApp.a_tDeadLine)								//比较：与有效日期
		{
			MessageBox("使用期已过,请获取新的Key");						//到期：退出
			return;														//返回
		}
		v_iThread_Exit=1;												//线程状态：运行
		v_pThread_Query=AfxBeginThread(My_Thread_Read,this);			//开启线程
	}
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：【启动线程：开始工作】
//------------------------------------------------------------------------------------------------------			
UINT CServer_DataDlg::My_Thread_Read(LPVOID lparam)
{    
	CServer_DataDlg *  lp_this = (CServer_DataDlg *)lparam;		         //窗体指针
	lp_this->m_cStart.EnableWindow(false);								//无效
	lp_this->m_cStop.EnableWindow(true);								//有效
	//队列清除
	lp_this->v_lConnect.clear();										//清空：信息链表
 	lp_this->v_lCommand.clear();										//清空：指令链表
	//开启telnet管理
	lp_this->TelnetManager.Start();		

	//信息处理线程	
	lp_this->v_pThread_Connect=AfxBeginThread(lp_this->My_Connect,lparam);		
	lp_this->v_cMutex[0]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//指令处理线程
	lp_this->v_pThread_Command=AfxBeginThread(lp_this->My_Command,lparam);		
	lp_this->v_cMutex[1]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//文件处理线程
	lp_this->v_pThread_ParseDataFile=AfxBeginThread(lp_this->ParseDataFileThread,lparam);		
	lp_this->v_cMutex[2]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//数据库处理线程
	lp_this->v_pThread_MaintainDataBase=AfxBeginThread(lp_this->MaintainDataBaseThread,lparam);  
	lp_this->v_cMutex[3]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件  

	//SQLBulkInsert维护线程
	lp_this->v_pThread_ExecuteBulkInsert=AfxBeginThread(lp_this->ExecuteBulkInsertThread,lparam);  
	lp_this->v_cMutex[4]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件 

	//全网性能求和线程
	lp_this->Thread_SumWholeNetworkPm_=AfxBeginThread(lp_this->SumWholeNetworkPmThread,lparam);		
	lp_this->v_cMutex[5]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//区域和其他性能求和线程
	lp_this->Thread_SumRegionandOtherPm_=AfxBeginThread(lp_this->SumRegionPmThread,lparam);  
	lp_this->v_cMutex[6]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件  

	//基本小区性能线程1
	lp_this->Thread1_BaseCellPm_=AfxBeginThread(lp_this->BaseCellPmThread1,lparam);		
	lp_this->v_cMutex[7]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//小区公式性能线程1
	lp_this->Thread1_CalculateCellPm_=AfxBeginThread(lp_this->CalculateCellPmThread1,lparam);		
	lp_this->v_cMutex[8]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//基本小区性能线程【小时粒度】
	lp_this->Thread_HourBaseCellPm_=AfxBeginThread(lp_this->HourBaseCellPmThread,lparam);		
	lp_this->v_cMutex[9]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//小区公式性能线程【小时粒度】
	lp_this->Thread_HourCalculateCellPm_=AfxBeginThread(lp_this->HourCalculateCellPmThread,lparam);		
	lp_this->v_cMutex[10]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//华为性能求和线程
	lp_this->Thread_SumHWPm_=AfxBeginThread(lp_this->SumHWPmThread,lparam);		
	lp_this->v_cMutex[11]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//中兴性能求和线程
	lp_this->Thread_SumZTEPm_=AfxBeginThread(lp_this->SumZTEPmThread,lparam);		
	lp_this->v_cMutex[12]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//基本小区性能线程2
	lp_this->Thread2_BaseCellPm_=AfxBeginThread(lp_this->BaseCellPmThread2,lparam);		
	lp_this->v_cMutex[13]=CreateEvent(NULL, FALSE, FALSE, NULL);			//产生事件

	//等待线程退出
    WaitForMultipleObjects(14,lp_this->v_cMutex,TRUE,INFINITE);	   
	//m_bThreadAlive置为FALSE,使线程全部推出
	lp_this->TelnetManager.ShutDowm();									//强制退出Telnet
	lp_this->m_cStart.EnableWindow(true);								//有效
	lp_this->m_cStop.EnableWindow(false);								//无效
	lp_this->v_iThread_Exit=0;											//线程状态：成功退出
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：基本小区性能线程1
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::BaseCellPmThread1(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                       //接口
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->basePm_lock_.Lock();
		if(pDlg->basePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->basePm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
		//处理基本性能
		pDlg->base_Pm_[0].ProcessData(ObjType,file_time,pDlg->base_Pm_[0].Pm_classification); 
		pDlg->base_Pm_[0].ClosePmFile(); 
		temp_time = file_time;
		temp_time.Replace("-","");
		temp_time = "_" + temp_time.Left(8);
		pDlg->sql_lock_.Lock();
		pDlg->base_Pm_[0].PushSqlIntoStack(theApp.a_save_directory_path_,temp_time,1500,file_name,pDlg->sql_stack_);
		pDlg->sql_lock_.Unlock();
		//文件中大连小区数
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
			message = "完成解析文件获得新数据" + message + "条";
			pDlg->ShowListCommand(file_name,message);   
		}      

		//reset
		pDlg->ResetObjType(ObjType);

	}
	SetEvent(pDlg->v_cMutex[7]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：基本小区性能线程2
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::BaseCellPmThread2(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                       //接口
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->basePm_lock_.Lock();
		if(pDlg->basePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->basePm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
		//处理基本性能
		pDlg->base_Pm_[1].ProcessData(ObjType,file_time,pDlg->base_Pm_[1].Pm_classification); 
		pDlg->base_Pm_[1].ClosePmFile(); 
		temp_time = file_time;
		temp_time.Replace("-","");
		temp_time = "_" + temp_time.Left(8);
		pDlg->sql_lock_.Lock();
		pDlg->base_Pm_[1].PushSqlIntoStack(theApp.a_save_directory_path_,temp_time,1500,file_name,pDlg->sql_stack_);
		pDlg->sql_lock_.Unlock();
		//文件中大连小区数
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
			message = "完成解析文件获得新数据" + message + "条";
			pDlg->ShowListCommand(file_name,message);   
		}      

		//reset
		pDlg->ResetObjType(ObjType);

	}
	SetEvent(pDlg->v_cMutex[7]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：基本小区性能线程【小时粒度】
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::HourBaseCellPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                       //接口
	CString file_name,file_time,temp_time,granularity,before_granularity,file_hour;
	//bool begin_flag = false;
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->hour_basePm_lock_.Lock();
		if(pDlg->hour_basePm_queue_.size()<=0)									
		{
			pDlg->hour_basePm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
		//整时？？
		if (granularity == "45:00" && pObjType->first == "结束")
		{
			CString updata_time;
			COleDateTime Ole_temp_time,granularity_time;
			granularity_time = COleDateTime::GetCurrentTime();
			Ole_temp_time.ParseDateTime(file_time);
			Ole_temp_time -= COleDateTimeSpan(0,0,45,0);
			updata_time = Ole_temp_time.Format("%Y-%m-%d %H:%M:%S");
			int control_count(0),day;
			pDlg->hour_base_Pm_.OpenPmFile(theApp.a_save_directory_path_,file_name);
			//处理基本性能
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
			//前一个完成粒度时间
			pDlg->KeepTime("完成粒度_" + updata_time,"完成小时粒度基本数据处理历时",granularity_time);

			//每天定时更新
			if (file_hour == "09")
			{
				pDlg->global_control_lock_.Lock();
				pDlg->global_control_ = true;
				pDlg->Updata();
				pDlg->KeepTime("测试","测试",granularity_time);
				theApp.m_MyLog.WriteText("测试\r\n");	
				pDlg->global_control_ = false;
				pDlg->global_control_lock_.Unlock();
			}
		} 
		else
		{
			if (pObjType->first == "结束")
			{
				continue;
			}
			file_name = pObjType->first;
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//更改文件名称
			file_name = "hour_" + file_name;
			//粒度时间
			granularity = file_time.Right(5);
			file_hour = file_time.Right(8);
			file_hour =file_hour.Left(2);
			pDlg->hour_base_Pm_.SaveHourPmData(ObjType);
		}
		//reset
		pDlg->ResetObjType(ObjType);

	}
	SetEvent(pDlg->v_cMutex[9]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：小区公式线程1
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::CalculateCellPmThread1(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                        //接口
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->calculatePm_lock_.Lock();
		if(pDlg->calculatePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->calculatePm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
	    //计算小区公式性能
		pDlg->calculation_Pm_[0].CalculatePerformance(ObjType,file_time);
		//公式计算性能保存到txt文件
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
	SetEvent(pDlg->v_cMutex[8]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：小区公式线程【小时粒度】
//----------------------------------------------------------------------------------------
UINT CServer_DataDlg::HourCalculateCellPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                        //接口
	CString file_name,file_time,temp_time,granularity_time,before_granularity;
	bool begin_flag = false;
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->hour_calculatePm_lock_.Lock();
		if(pDlg->hour_calculatePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->hour_calculatePm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
		//更改文件名称
		file_name = "hour_" + file_name;
		//粒度时间
		granularity_time = file_time.Right(5);                       //获得分钟like 00:00 15:00
		if (granularity_time == "00:00" && before_granularity != granularity_time && begin_flag )
		{
			CString updata_time;
			COleDateTime Ole_temp_time;
			int control_count(0),day;
			Ole_temp_time.ParseDateTime(file_time);
			Ole_temp_time -= COleDateTimeSpan(0,1,0,0);
			updata_time = Ole_temp_time.Format("%Y-%m-%d %H:%M:%S");
			//计算小区公式性能
			pDlg->hour_calculation_Pm_.meanHourPmCount(pDlg->hour_calculation_Pm_.hour_Pm_object_);
			pDlg->hour_calculation_Pm_.CalculateHourPerformance(updata_time);
			//公式计算性能保存到txt文件
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
	SetEvent(pDlg->v_cMutex[10]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：全网性能求和线程
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::SumWholeNetworkPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                        //接口
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->networkPm_lock_.Lock();
		if(pDlg->networkPm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->networkPm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
		if (file_name == "结束")
		{
			//全网计算性能
			pDlg->network_Pm_.CalculatePmForWholeNetwork();
			//全网性能上传
			pDlg->network_Pm_.UploadingPm(file_time,pDlg->network_Pm_.network_Pm_,"全网");
		} 
		else
		{
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//全网基本性能求和
			pDlg->network_Pm_.SumBasePmForWholeNetwork(ObjType,pDlg->network_filter_);
			//全网基本性能粒度求和
			pDlg->network_Pm_.SumBasePmInGranularityForWholeNetwork();
		}
		//reset
		pDlg->ResetObjType(ObjType);
	}
	SetEvent(pDlg->v_cMutex[5]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：华为性能求和线程
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::SumHWPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                        //接口
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1 )										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->hwPm_lock_.Lock();
		if(pDlg->hwPm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->hwPm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
		if (file_name == "结束")
		{
			//华为计算性能
			pDlg->hw_Pm_.CalculatePmForHW();
			//华为性能上传
			pDlg->hw_Pm_.UploadingPm(file_time,pDlg->hw_Pm_.hw_Pm_,"华为");
		} 
		else
		{
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//华为基本性能求和
			pDlg->hw_Pm_.SumBasePmForHW(ObjType,pDlg->hw_filter_);
			//华为基本性能粒度求和
			pDlg->hw_Pm_.SumBasePmInGranularityForHW();
		}
		//reset
		pDlg->ResetObjType(ObjType);
	}

	SetEvent(pDlg->v_cMutex[11]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：中兴性能求和线程
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::SumZTEPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                        //接口
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->ztePm_lock_.Lock();
		if(pDlg->ztePm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->ztePm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
		if (file_name == "结束")
		{
			//中兴计算性能
			pDlg->zte_Pm_.CalculatePmForZTE();
			//中兴性能上传
			pDlg->zte_Pm_.UploadingPm(file_time,pDlg->zte_Pm_.zte_Pm_,"中兴");
		} 
		else
		{
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//中兴基本性能求和
			pDlg->zte_Pm_.SumBasePmForZTE(ObjType,pDlg->zte_filter_);
			//中兴基本性能粒度求和
			pDlg->zte_Pm_.SumBasePmInGranularityForZTE();
		}

		//reset
		pDlg->ResetObjType(ObjType);
	}

	SetEvent(pDlg->v_cMutex[12]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：区域和其他性能求和线程
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::SumRegionPmThread(LPVOID lparam)
{   
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针
	map_ObjType::iterator	pObjType;							//接口指针
	map_ObjType ObjType;                                        //接口
	CString file_name,file_time,temp_time;
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		pDlg->regionPm_lock_.Lock();
		if(pDlg->regionPm_queue_.size()<=0 || pDlg->global_control_)									
		{
			pDlg->regionPm_lock_.Unlock();
			Sleep(500);												//延时
			continue;													//继续等待
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
		if (file_name == "结束")
		{
			//其他计算性能
			pDlg->region_Pm_.CalculatePmForOther();
			//其他性能上传
			pDlg->region_Pm_.UploadingPm(file_time,pDlg->region_Pm_.other_Pm_,"其他");
			//区域性能上传
			pDlg->region_Pm_.CalculateandUploadingPmForRegion(file_time);
		} 
		else
		{
			file_name = file_name.Mid(0,file_name.GetLength()-2);
			pDlg->GetFileTime(file_name,file_time);
			//区域和其他基本性能求和
			pDlg->region_Pm_.SumBasePmForRegionandOther(ObjType,pDlg->region_filter_);
			//其他基本性能粒度求和
			pDlg->region_Pm_.SumBasePmInGranularityForOther();
			//区域基本性能粒度求和
			pDlg->region_Pm_.SumBasePmInGranularityForRegion();
		}
		//reset
		pDlg->ResetObjType(ObjType);
	}
	SetEvent(pDlg->v_cMutex[6]);							    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：连接信息处理线程 【处理心跳、登录信息、指令执行结果处理】
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::My_Connect(LPVOID lparam)
{   
	CString		v_sTemp;												//临时变量
	int			v_iCount;												//临时变量
	v_sCommand	v_cCommand;
	list<v_sCommand>::iterator v_pIter;									//迭代器
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//对话窗指针

	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		pDlg->v_cLock_Connect.Lock();									//加锁
		v_iCount=pDlg->v_lConnect.size();								//命令数量
		pDlg->v_cLock_Connect.Unlock();									//解锁
		if(v_iCount<=0)													//没有等处理的任务？？？
		{
			Sleep(50);													//延时
			continue;													//继续等待
		}		

		pDlg->m_cAAA.SetWindowText("1");
		//获取连接信息
		pDlg->v_cLock_Connect.Lock();									//加锁
		v_pIter=pDlg->v_lConnect.begin();								//获取连接信息
		if (v_pIter!=pDlg->v_lConnect.end())
		{
			v_cCommand.v_iCode=v_pIter->v_iCode;						//
			v_cCommand.v_iSocket=v_pIter->v_iSocket;					//
			v_cCommand.v_sText=v_pIter->v_sText;						//
			pDlg->v_lConnect.pop_front();								//队列移除【消除已获取的任务】
		}
		else
		{
			pDlg->v_lConnect.clear();									//队列移除【消除已获取的任务】
		}
		pDlg->v_cLock_Connect.Unlock();									//解锁

		pDlg->m_cAAA.SetWindowText("2");
		//连接信息处理
		if (v_cCommand.v_iCode==0x01)									//心跳信息
		{	
			//客户心跳数量++
			map<SOCKET,v_sClient_Connect>::iterator v_pIter;			//迭代器
			v_pIter=pDlg->TelnetManager.v_mClient_Connect.find(v_cCommand.v_iSocket);	//查找？？？
			if (v_pIter!=pDlg->TelnetManager.v_mClient_Connect.end())	//找到？？？
				v_pIter->second.v_iSum++;								//心跳++
			//回复：客户端心跳
			pDlg->Send_Command(v_cCommand.v_iSocket,0x81,"");			//回复：客户端心跳
			if(pDlg->m_cinformation_monitor.GetCheck() == 1)
				pDlg->My_ShowList_Bug("心跳发送","");					//显示：心跳
			//如果是采集端SOCKET，则复位采集端计算器
			if (pDlg->v_sServer_Socket!=NULL && pDlg->v_sServer_Socket==v_cCommand.v_iSocket)	//采集端SOCKET==心跳SOCKET？？？
				pDlg->v_iServer_Sum=0;									//复位：采集端计算器
		}
		else if(v_cCommand.v_iCode==0x02)								//登录信息【显示客户名称】
		{
			pDlg->TelnetManager.v_cTelnet_Lock.Lock();					//加锁
			//修改客户信息
			map<SOCKET,v_sClient_Connect>::iterator v_pIter;			//迭代器
			v_pIter=pDlg->TelnetManager.v_mClient_Connect.find(v_cCommand.v_iSocket);	//查找？？？
			if (v_pIter!=pDlg->TelnetManager.v_mClient_Connect.end())	//找到？？？
				v_pIter->second.v_sName=v_cCommand.v_sText;				//删除客户端信息
			//显示客户信息
			for (int vi=0;vi<pDlg->m_cList_Client.GetItemCount();vi++)	//遍历：客户列表
			{
				if (v_cCommand.v_iSocket==atoi(pDlg->m_cList_Client.GetItemText(vi,3)))		//找到？？？【依据：客户SOCKET】
				{
					pDlg->m_cList_Client.SetItemText(vi,4,v_cCommand.v_sText);				//序号
					break;												//退出
				}
			}
			//如果客户名称=="采集服务器"，则是采集端；
			if (v_cCommand.v_sText=="采集服务器")						//是采集服务器？？？
			{
				pDlg->v_sServer_Socket=v_cCommand.v_iSocket;			//记录：采集端SOCKET
				pDlg->m_cServer.EnableWindow(true);						//有效：采集端
			}
			pDlg->TelnetManager.v_cTelnet_Lock.Unlock();				//解锁
		}
		else if(v_cCommand.v_iCode==0x86)								//BSC采集异常【通知所有客户端】
		{
			pDlg->My_ShowList_Bug("BSC采集异常","异常："+v_cCommand.v_sText);		//显示：异常
			theApp.m_MyLog.WriteText("异常："+v_cCommand.v_sText+"\r\n");			//log
			pDlg->My_Send_Client_Alarm(v_cCommand.v_iCode,v_cCommand.v_sText);		//向所有客户端发送消息
		}
		else if(v_cCommand.v_iCode==0x83)								//指令执行结果处理【发送回下达指令的客户端】
		{
			pDlg->m_cAAA.SetWindowText("3");
			pDlg->TelnetManager.v_cTelnet_Lock.Lock();					//加锁
			pDlg->Send_Command(pDlg->v_iClient_Socket,v_cCommand.v_iCode,v_cCommand.v_sText);	//发送指令执行结果【回复客户端】
			pDlg->v_sCommand_Rece+=v_cCommand.v_sText;					//指令执行结果
			pDlg->m_cAAA.SetWindowText("4");
			pDlg->My_ShowList_Command_Text(v_cCommand.v_sText);			//指令执行结果显示
			pDlg->m_cAAA.SetWindowText("5");
			if (pDlg->v_sCommand_Rece.Find("\r\nEND")!=-1 || pDlg->v_sCommand_Rece.Find("\r\nNOT ACCEPTED")!=-1)		//指令执行结束？？？
				pDlg->v_bCommand_Flag=0;								//复位：指令发送标记

			pDlg->v_sCommand_Rece=v_cCommand.v_sText;					//指令执行结果
			pDlg->TelnetManager.v_cTelnet_Lock.Unlock();				//解锁
		}
		pDlg->m_cAAA.SetWindowText("9");
	}
	SetEvent(pDlg->v_cMutex[0]);										//发送：线程结束事件
	return 0;
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：指令处理线程【指令发送处理】
//------------------------------------------------------------------------------------------------------
UINT CServer_DataDlg::My_Command(LPVOID lparam)
{
 	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				//窗体指针
    int			v_iCount;												//临时变量
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//第一步：指令超时
		if(pDlg->v_bCommamd)											//指令超时标志无效？？？
		{
			pDlg->v_bCommamd=false;										//复位：指令超时标志
			pDlg->TelnetManager.v_cTelnet_Lock.Lock();					//加锁
			pDlg->Send_Command(pDlg->v_iClient_Socket,0x83,"指令执行超时！\r\nEND");	//发送指令超时结果
			pDlg->My_ShowList_Command_Text("指令执行超时！");			//指令执行结果显示
			pDlg->v_bCommand_Flag=0;									//复位：指令发送标记
			pDlg->TelnetManager.v_cTelnet_Lock.Unlock();				//解锁
		}

		//第二步：客户端处理
		if(pDlg->v_bClient)												//客户端处理标志无效？？？
		{
			pDlg->v_bClient=false;										//复位：客户端处理标志
			pDlg->My_Client_Proc();										//客户端信息处理【超时退出】
		}

		//第三步：指令处理
		pDlg->v_cLock_Command.Lock();									//加锁
		v_iCount=pDlg->v_lCommand.size();								//命令处理数量
		pDlg->v_cLock_Command.Unlock();									//解锁
		if(pDlg->v_bCommand_Flag==1 || v_iCount<=0)						//没有执行指令？？？ || 没有等处理的任务？？？
		{
			Sleep(50);													//延时
			continue;													//继续等待
		}		

		pDlg->m_cTest.SetWindowText("1");

		//获取连接信息
		pDlg->v_cLock_Command.Lock();									//加锁
		list<v_sCommand>::iterator v_pIter=pDlg->v_lCommand.begin();	//
		v_sCommand v_cCommand;
		v_cCommand.v_iCode=v_pIter->v_iCode;							//获取指令信息
		v_cCommand.v_iSocket=v_pIter->v_iSocket;						//获取指令信息
		v_cCommand.v_sText=v_pIter->v_sText;							//获取指令信息
		pDlg->v_lCommand.pop_front();									//队列移除【消除已获取的任务】
		pDlg->v_cLock_Command.Unlock();									//解锁

		pDlg->m_cTest.SetWindowText("2");

		//连接指令03【获取解析文件名称，文件重命名用于map排序】
		if (v_cCommand.v_iCode==0x03)									//指令信息
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
				COleDateTime SpanTime=COleDateTime::GetCurrentTime();		                      //更新历时
				CString sTime=SpanTime.Format("%H:%M:%S");
				CString sMsg  =  "0x03";
				pDlg->ShowListCommand(sTemp,sMsg);
				theApp.m_MyLog.WriteText("通信："+sMsg+":"+sTemp+"\r\n");	  //log
			}

			//查找客户端信息
			pDlg->v_cLock_FileName.Lock();							     //加锁
			pDlg->file_queue_.push(v_cCommand.v_sText);
			pDlg->v_cLock_FileName.Unlock();		
		}
		//粒度结束指令05
		else if (v_cCommand.v_iCode==0x05)									//指令结束
		{
			//查找客户端信息
			CString sTemp;
			sTemp=v_cCommand.v_sText;
			pDlg->v_cLock_FileName.Lock();							     //加锁
			pDlg->file_queue_.push(v_cCommand.v_sText);
			pDlg->v_cLock_FileName.Unlock();	
			if (pDlg->m_cinformation_monitor.GetCheck() == 1)
			{
				COleDateTime SpanTime=COleDateTime::GetCurrentTime();		                      //更新历时
				CString sTime=SpanTime.Format("%H:%M:%S");
				CString sMsg  =  "0x05";
				pDlg->ShowListCommand(sTemp,sMsg);
				theApp.m_MyLog.WriteText("通信："+sMsg+":"+sTemp+"\r\n");	  //log
			}
		}
	}
	SetEvent(pDlg->v_cMutex[1]);										//发送：线程结束事件
	return 0;
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：处理与数据库表相关功能
//------------------------------------------------------------------------------------------------------
UINT CServer_DataDlg::MaintainDataBaseThread(LPVOID lparam)
{
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;				        //窗体指针
	COleDateTime     current_time,temp_time;
	COleDateTime FolderTime,CurTime,compareTime,tempTime;
	CString          time_parameter,folder_name,delete_time;
	bool             month_table_flag(true),day_table_flag(true);
	int nValue,day;
	//log目录下的文件夹名称
	list<CString>       folder_queue;
	pDlg->DB_maintenance_lock_.Lock();
	pDlg->DB_maintenance_.Init();
	pDlg->DB_maintenance_lock_.Unlock();
	//获取当前时间参数【整天】
	current_time = COleDateTime::GetCurrentTime();
	time_parameter = current_time.Format("%Y-%m-%d");
	current_time.ParseDateTime(time_parameter);
	current_time -= COleDateTimeSpan(1,0,0,0);

	//补全未生成15分钟粒度数据库表
	compareTime.ParseDateTime(pDlg->DB_maintenance_.granularity_time_);
	while(compareTime < current_time){
		compareTime += COleDateTimeSpan(1,0,0,0);
		time_parameter = compareTime.Format("%Y-%m-%d");
		pDlg->DB_maintenance_.CreateQuarterGranularityTable(time_parameter);
    }

	//补全未生成当月小时粒度数据库表
	pDlg->DB_maintenance_lock_.Lock();
	//判断是否更新CON_Pm_LTE_Cal_Next_Month表？
	month_table_flag = pDlg->DB_maintenance_.JudgeCalculateTableUpdate();
	if (month_table_flag == true)
	{
		//创建下个月标准公式性能表,如果存在删除后新建
		{
			pDlg->DB_maintenance_.UpdateCONPmLTECalNextMonthTable();
		}
	}
	//判断月表是否已经创建【true:创建下一个月表】
	month_table_flag = pDlg->DB_maintenance_.JudgeThisMonthHourGranularityTableExist(time_parameter);
	if (month_table_flag == true)
	{
		//创建下一个月小时粒度数据库表
		{
			pDlg->DB_maintenance_.CreateThisMonthHourGranularityTable(time_parameter);
		}
	}
	pDlg->DB_maintenance_lock_.Unlock();

	while(pDlg->v_iThread_Exit==1)
	{

		//没有等处理的任务？？？
		if(pDlg->global_control_)									
		{
			Sleep(1000);												//延时
			continue;													//继续等待
		}

		//获取当前时间参数
		current_time = COleDateTime::GetCurrentTime();
		time_parameter = current_time.Format("%Y-%m-%d");

		//删除过期GZ文件
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

		//删除过期15分钟粒度表
		day = atoi(pDlg->DB_maintenance_.pm_save_day_);
		temp_time.ParseDateTime(pDlg->DB_maintenance_.granularity_time_);
		temp_time -= COleDateTimeSpan(day,0,0,0);
		delete_time = temp_time.Format("%Y-%m-%d %H:%M");
		pDlg->DB_maintenance_lock_.Lock();
		pDlg->DB_maintenance_.DeleteStaleData(delete_time);
		pDlg->DB_maintenance_lock_.Unlock();

		//删除过期bulkinsert文件
		pDlg->DB_maintenance_lock_.Lock();
		CString delete_granularity_time;
		pDlg->DB_maintenance_.Init();
		delete_granularity_time = pDlg->DB_maintenance_.granularity_time_;
		pDlg->DB_maintenance_.DeleteBulkInsertFile(delete_granularity_time,theApp.a_save_directory_path_);
        pDlg->DB_maintenance_lock_.Unlock();

		//创建月表
		pDlg->DB_maintenance_lock_.Lock();
		if (current_time.GetDay() == 28)
		{
			//判断是否更新CON_Pm_LTE_Cal_Next_Month表？
			month_table_flag = pDlg->DB_maintenance_.JudgeCalculateTableUpdate();
			if (month_table_flag == true)
			{
				//创建下个月标准公式性能表,如果存在删除后新建
				{
					pDlg->DB_maintenance_.UpdateCONPmLTECalNextMonthTable();
				}
			}
			//判断月表是否已经创建【true:创建下一个月表】
			month_table_flag = pDlg->DB_maintenance_.JudgeHourGranularityTableExist(time_parameter);
			if (month_table_flag == true)
			{
				//创建下一个月小时粒度数据库表
				{
					pDlg->DB_maintenance_.CreateHourGranularityTable(time_parameter);
				}
			}
		}
		pDlg->DB_maintenance_lock_.Unlock();

		//更新Pm_LTE_Cal即性能计算15粒度表？
		pDlg->DB_maintenance_lock_.Lock();
		if (current_time.GetDay() == 1)
		{
			//更新配置数据
			pDlg->global_control_lock_.Lock();

			//判断是否更新CON_Pm_LTE_Cal_This_Month表？
			month_table_flag = pDlg->DB_maintenance_.JudgeThisCalculateTableUpdate();
			if (month_table_flag == true)
			{
				//创建本月标准公式性能表,如果存在删除后新建
				{
					pDlg->DB_maintenance_.UpdateCONPmLTECalThisMonthTable();
				}
				//更新数据
				pDlg->global_control_ = true;
				pDlg->Updata();
				theApp.m_MyLog.WriteText("月测试\r\n");	
				pDlg->global_control_ = false;
				pDlg->global_control_lock_.Unlock();
			}
			else
			{
				pDlg->global_control_lock_.Unlock();
			}
		}
		pDlg->DB_maintenance_lock_.Unlock();

		//创建每天表
		{
			pDlg->DB_maintenance_lock_.Lock();
			day_table_flag = pDlg->DB_maintenance_.JudgeQuarterGranularityTableExist(time_parameter);
			if (day_table_flag == true)
			{
				//创建当天一天15分钟粒度数据库表
				{
					pDlg->DB_maintenance_.CreateQuarterGranularityTable(time_parameter);
				}
			}
			pDlg->DB_maintenance_lock_.Unlock();
		}

		//休眠
		Sleep(300*1000);

	}

	SetEvent(pDlg->v_cMutex[3]);
	return 0;
}
	
//----------------------------------------------------------------------------------------
//	函数功能：执行Bulk insert语句
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::ExecuteBulkInsertThread(LPVOID lparam)
{
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;						//对话窗指针
	while(pDlg->v_iThread_Exit==1 )										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		if(pDlg->sql_stack_.size()<=0 || pDlg->global_control_)									
		{
			Sleep(1000);												//延时
			continue;													//继续等待
		}
		pDlg->ExecuteBulkInsertStack();
	}
	SetEvent(pDlg->v_cMutex[4]);										    //发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：【XML文件解压,过滤数据,保存队列】
//------------------------------------------------------------------------------*---------
UINT CServer_DataDlg::ParseDataFileThread(LPVOID lparam)
{
	CServer_DataDlg* pDlg=(CServer_DataDlg*)lparam;						  //对话窗指针
	map_ObjType::iterator	 iterator_ObjType;							  //ObjType指示器
	map_ObjType_Child::iterator iterator_ObjType_Child;                   //ObjType_Child迭代器
	map_ObjType              ObjType;                                     //接口
	CString temp,socket_file_name,zip_file_path,zip_file_name,xml_file_name,xml_file_path,file_name,file_time,file_type,current_grantularity,before_grantularity,grantularity_time;
	CString delete_granularity_time;
	COleDateTime before_time,Ole_temp_time;
	CString message;
	int temp_count(0);
	//数据初始化
	pDlg->global_control_lock_.Lock();
	pDlg->Init();
	pDlg->global_control_lock_.Unlock();
	//文件补录
	pDlg->v_cLock_FileName.Lock();
	pDlg->file_collection_.GetUnprocessedLog(theApp.a_sPm_Path_,pDlg->DB_maintenance_.granularity_time_,pDlg->file_queue_);
	pDlg->v_cLock_FileName.Unlock();
	//删除重复数据
	delete_granularity_time = pDlg->DB_maintenance_.granularity_time_;
	//前一个完成粒度时间
	grantularity_time = COleDateTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
	pDlg->DeleteNextGranularityData(delete_granularity_time);
	while(pDlg->v_iThread_Exit==1)										//强制退出？？？ 【v_iThread_Exit==2】
	{
		//没有等处理的任务？？？
		if(pDlg->file_queue_.size()<=0 || pDlg->global_control_)									
		{
			Sleep(1000);												//延时
			continue;													//继续等待
		}

		before_time = COleDateTime::GetCurrentTime();				  //获得当前时间

		pDlg->v_cLock_FileName.Lock();									//加锁	
		socket_file_name = pDlg->file_queue_.front();                   //带路径的和文件名称			
		pDlg->file_queue_.pop();
		//显示待文件数
		temp.Format("%d",pDlg->file_queue_.size());
		pDlg->m_cFileCount.SetWindowTextA(temp);						
		pDlg->v_cLock_FileName.Unlock();

		//粒度结束
		if (socket_file_name == "结束")
		{
			pDlg->ResetObjType(ObjType);
			list_Object temp_list_Object;
			ObjType["结束"] = temp_list_Object;

			//读取结果放到全网性能队列【3】
			pDlg->networkPm_lock_.Lock();										//加锁
			pDlg->networkPm_queue_.push_back(ObjType);				//性能容器放入性能队列
			pDlg->networkPm_lock_.Unlock();

			//读取结果放到区域性能队列【4】
			pDlg->regionPm_lock_.Lock();										//加锁
			pDlg->regionPm_queue_.push_back(ObjType);				//性能容器放入性能队列
			pDlg->regionPm_lock_.Unlock();

			//读取结果放到小时粒度基本性能队列【5】
			pDlg->hour_basePm_lock_.Lock();												//加锁
			pDlg->hour_basePm_queue_.push_back(ObjType);						 //性能容器放入性能队列
			pDlg->hour_basePm_lock_.Unlock();

			//读取结果放到华为性能队列【7】
			pDlg->hwPm_lock_.Lock();											//加锁
			pDlg->hwPm_queue_.push_back(ObjType);				    //性能容器放入性能队列
			pDlg->hwPm_lock_.Unlock();

			//读取结果放到中兴性能队列【8】
			pDlg->ztePm_lock_.Lock();											//加锁
			pDlg->ztePm_queue_.push_back(ObjType);				    //性能容器放入性能队列
			pDlg->ztePm_lock_.Unlock();

			pDlg->ResetObjType(ObjType);

			//更新数据库完成粒度？？
			if ( current_grantularity == "45:00")
			{
				//上传完成粒度时间
				temp = "update CON_System set zPm_LTE_Time_Data='"+ file_time +"'";
				pDlg->ExecuteSQL(temp);
			}

			//删除重复数据
			delete_granularity_time = file_time;
			pDlg->DeleteNextGranularityData(delete_granularity_time);

			//前一个完成粒度时间
			Ole_temp_time.ParseDateTime(grantularity_time);
			pDlg->KeepTime("完成粒度_" + file_time,"完成15分钟粒度数据处理总历时",Ole_temp_time);

			//完成粒度时间更新
			Sleep(20*1000);
			temp = "update CON_System set zPm_LTE_Time_Real='"+ file_time +"'";
			pDlg->ExecuteSQL(temp);

			//判断创建网络驱动器
			if (GetDriveType("F:") == DRIVE_NO_ROOT_DIR)
			{
				system(_T("net use F: \\\\192.168.1.101\\f  \"Aa123456\" /user:\"DLPM\""));
			}

			continue;
		}

		//获得文件信息
		pDlg->GetFileInformation(socket_file_name,zip_file_name,zip_file_path,xml_file_name,xml_file_path,file_name,file_time,file_type);

		//显示正在处理粒度时间
		Ole_temp_time.ParseDateTime(file_time);
		Ole_temp_time += COleDateTimeSpan(0,0,15,0);
		temp = Ole_temp_time.Format("%Y-%m-%d %H:%M");
		pDlg->m_cGTime.SetWindowTextA(temp);

		//当前粒度时间
		current_grantularity = file_time.Right(5);

		//更新粒度计时时间？
		if (current_grantularity != before_grantularity)
		{
			grantularity_time = COleDateTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
		}

		//log文件解压
		if (!pDlg->PMArchive(zip_file_path,xml_file_path))
		{
			message  = zip_file_name+"解压文件失败";		
			pDlg->My_ShowList_Bug("解压文件异常","解析异常："+message);		//显示：异常
			continue;
		}

		//获取数据保存到容器
		pDlg->v_cFile_XML.File_Reset();//清空数据
		if (!pDlg->v_cFile_XML.Read_XML_PM_Dir(xml_file_path))             //获取：XML文件信息
		{      			
			message  = zip_file_name+"解读文件失败";		
			pDlg->My_ShowList_Bug("读取文件异常","解析异常："+message);		//显示：异常
			continue;
		}

		////接口获得文件时间
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

		//接口获得文件时间
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

		//小区名称映射为整数
		pDlg->MapCellNameToID(ObjType,pDlg->cellname_map_ID_);

		//非大连小区数据过滤
		pDlg->cell_filter_.FilterNonDalianCell(ObjType);

		//读取结果放到基本性能队列【1】
		pDlg->basePm_lock_.Lock();												//加锁
		pDlg->basePm_queue_.push_back(ObjType);								 //性能容器放入性能队列
		pDlg->basePm_lock_.Unlock();

		//读取结果放到计算性能队列【2】
		pDlg->calculatePm_lock_.Lock();											//加锁
		pDlg->calculatePm_queue_.push_back(ObjType);						 //性能容器放入性能队列
		pDlg->calculatePm_lock_.Unlock();
		
		//读取结果放到全网性能队列【3】
		pDlg->networkPm_lock_.Lock();											//加锁
		pDlg->networkPm_queue_.push_back(ObjType);							//性能容器放入性能队列
		pDlg->networkPm_lock_.Unlock();

		//读取结果放到区域性能队列【4】
		pDlg->regionPm_lock_.Lock();											//加锁
		pDlg->regionPm_queue_.push_back(ObjType);							//性能容器放入性能队列
		pDlg->regionPm_lock_.Unlock();

		//读取结果放到小时粒度基本性能队列【5】
		pDlg->hour_basePm_lock_.Lock();												//加锁
		pDlg->hour_basePm_queue_.push_back(ObjType);						 //性能容器放入性能队列
		pDlg->hour_basePm_lock_.Unlock();

		//读取结果放到小时粒度计算性能队列【6】
		pDlg->hour_calculatePm_lock_.Lock();												//加锁
		pDlg->hour_calculatePm_queue_.push_back(ObjType);					 //性能容器放入性能队列
		pDlg->hour_calculatePm_lock_.Unlock();
       
		if(file_type == "华为")
		{
			//读取结果放到华为性能队列【7】
			pDlg->hwPm_lock_.Lock();											//加锁
			pDlg->hwPm_queue_.push_back(ObjType);				//性能容器放入性能队列
			pDlg->hwPm_lock_.Unlock();
		}
		else
		{
			//读取结果放到中兴性能队列【8】
			pDlg->ztePm_lock_.Lock();											//加锁
			pDlg->ztePm_queue_.push_back(ObjType);				//性能容器放入性能队列
			pDlg->ztePm_lock_.Unlock();
			pDlg->ResetObjType(ObjType);
		}
		//清空接口
		pDlg->ResetObjType(ObjType);
		before_grantularity = current_grantularity;

		//时间计时
		pDlg->KeepTime(file_name,"完成文件解析及性能入队",before_time);

	}
	SetEvent(pDlg->v_cMutex[2]);												//发送：线程结束事件
	return 0;
}

//----------------------------------------------------------------------------------------
//	函数功能：客户端信息处理【超时退出】
//------------------------------------------------------------------------------*---------
void CServer_DataDlg::My_Client_Proc()
{
	bool	v_bOK=false;												//删除标志
	//第一步：显示[IOCP线程数-指令-连接]
	CString v_sCount;													//临时变量【显示工作进展：IOCP线程数-信息队列数-指令队列数】
	v_sCount.Format("%d-%d-%d",TelnetManager.v_cIocp.GetWorkThread(),v_lCommand.size(),v_lConnect.size());		//IOCP线程数-指令-连接
	m_cState.SetWindowText(v_sCount);									//显示：IOCP线程数-指令-连接

	//第二步：掉线客户端删除			
	TelnetManager.v_cTelnet_Lock.Lock();								//加锁
	map<SOCKET,v_sClient_Connect>::iterator v_pIter;					//迭代器
	v_pIter=TelnetManager.v_mClient_Connect.begin();					//迭代器：首位置
	while(v_pIter!=TelnetManager.v_mClient_Connect.end())				//结束？？？
	{
		v_pIter->second.v_iTimer++;										//定时器计数器++
		if (v_pIter->second.v_iTimer>=3)								//>=3次
		{
			if(v_pIter->second.v_iSum==0)								//没有心跳？？？
			{
				if (v_sServer_Socket==v_pIter->second.v_iSocket)		//是采集服务器？？？
				{
					v_sServer_Socket=NULL;								//复位：采集端SOCKET
					m_cServer.EnableWindow(false);						//无效：采集端
				}
				if (v_pIter->second.v_iSocket!=NULL)					//客户端SOCKET有效？？？
					closesocket(v_pIter->second.v_iSocket);				//关闭客户端连接
				v_pIter=TelnetManager.v_mClient_Connect.erase(v_pIter);	//删除客户端信息
				v_bOK=true;												//删除标志
				continue;												//继续处理
			}
			v_pIter->second.v_iTimer=0;									//复位：定时器计数器
			v_pIter->second.v_iSum=0;									//复位：心跳计数器			
		}
		v_pIter++;														//迭代器++
	}
	if (v_bOK)															//有删除标志？？？【更新显示内容】
	{
		//更新显示：客户端信息
		m_cList_Client.DeleteAllItems();								//清除列表信息
		v_pIter=TelnetManager.v_mClient_Connect.begin();				//迭代器：首位置
		while(v_pIter!=TelnetManager.v_mClient_Connect.end())			//结束？？？
		{
			My_ShowList_Client(v_pIter->second.v_sIP,v_pIter->second.v_iSocket,v_pIter->second.v_sName);	//列表显示
			v_pIter++;													//迭代器++
		}
	}
	TelnetManager.v_cTelnet_Lock.Unlock();								//解锁
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：向所有客户端发送消息
//	参    数：v_iFunctionCode：功能码；v_sCommand：信息指令；
//	例    子：【心跳：(0x81,"")】【采集服务器异常：(0x85,"...")】【BSC采集异常：(0x86,"...")】
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_Send_Client_Alarm(unsigned int v_iCode ,CString v_sError)
{
	TelnetManager.v_cTelnet_Lock.Lock();						//加锁
	map<SOCKET,v_sClient_Connect>::iterator v_pIter;			//迭代器
	v_pIter=TelnetManager.v_mClient_Connect.begin();			//迭代器：首位置
	while(v_pIter!=TelnetManager.v_mClient_Connect.end())		//结束？？？
	{
		Send_Command(v_pIter->second.v_iSocket,v_iCode,v_sError);	//发送指令
		v_pIter++;												//迭代器++
	}
	TelnetManager.v_cTelnet_Lock.Unlock();						//解锁
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：发送指令
//	参    数：v_iFunctionCode：功能码；v_sCommand：信息指令；
//	例    子：【指令：(0x03,"...")】【心跳：(0x81,"")】【采集服务器异常：(0x85,"...")】【BSC采集异常：(0x86,"...")】
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::Send_Command(SOCKET v_sClient_Socket,unsigned int v_iCode ,CString v_sCommand)
{
	u_char v_cSendData[1024];											//发送缓冲区
	v_cSendData[0] = 0x43;												//报文头
	v_cSendData[1] = 0x4D;												//报文头
	int v_iLen = 6 + v_sCommand.GetLength();							//数据长度
	v_cSendData[2] = v_iLen/256;										//数据长度：高位
	v_cSendData[3] = v_iLen%256;										//数据长度：低位
	v_cSendData[4] = v_iCode;											//功能码 【01：心跳；02：登录；03：指令；】
	u_char* v_pSend=&v_cSendData[5];									//指令位置
	memcpy(v_pSend,v_sCommand,v_sCommand.GetLength());					//数据处理
	AddCheckBits(v_cSendData);											//计算校验
	v_cLock_Send.Lock();												//解锁
	Send_Socket(v_sClient_Socket,(char*)v_cSendData,v_iLen);			//发送数据
	v_cLock_Send.Unlock();												//加锁	
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：发送数据
//------------------------------------------------------------------------------------------------------
int CServer_DataDlg::Send_Socket(SOCKET v_sClient_Socket,char* v_sSend,int v_iLen)
{
	if (v_sClient_Socket!=NULL)											//SOCKET有效？？？
		send(v_sClient_Socket,v_sSend,v_iLen,0);						//发送错误？？？
	return true;
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：增加校验函数
//------------------------------------------------------------------------------------------------------
BOOL  CServer_DataDlg::AddCheckBits(u_char* sendData)
{
	int v_ilen=(u_char)sendData[2]*256+(u_char)sendData[3];				//数据长度
	if(v_ilen<6)														//无效数据？？？
		return FALSE;													//无效返回
	u_char v_chSun=0x00;												//默认校验和
	for (int i=0;i<v_ilen-1;i++)										//数据遍历
		v_chSun+=sendData[i];											//计算：校验和
	sendData[v_ilen-1]=0xFF-v_chSun;									//设置：校验和
	return TRUE;														//有效返回
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：Client列表窗中增加一行通信信息
//	参    数：v_sIP：Client IP；v_sName；
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Client(CString v_sIP,SOCKET v_iSocket,CString v_sName)
{
	int		vi,v_iIndex;												//临时变量
	CString v_sTemp;													//临时变量
	CString v_sCurTime;													//临时变量
	CTime	v_CurTime=CTime::GetCurrentTime();

	v_sCurTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	v_iIndex = m_cList_Client.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_cList_Client.SetItemText(v_iIndex,0," "+v_sTemp);					//序号
	m_cList_Client.SetItemText(v_iIndex,1,v_sCurTime);					//发生时间
	m_cList_Client.SetItemText(v_iIndex,2,v_sIP);						//Client IP
	v_sTemp.Format("%d",v_iSocket);										//Client SOCKET
	m_cList_Client.SetItemText(v_iIndex,3,v_sTemp);						//Client SOCKET
	m_cList_Client.SetItemText(v_iIndex,4,v_sName);						//Client 名称
	theApp.m_MyLog.WriteText("客户连接："+v_sIP+"； Socket"+v_sTemp+"\r\n");		//客户连接信息

	//列表窗条数控制：超过1000条，删除300条；
	m_cList_Client.SetRedraw(FALSE);									//不更新界面
	if (m_cList_Client.GetItemCount()>=1000)							//如果纪录数大于1000
	{
		for(vi=0;vi<300;vi++)											//删除前300条
			m_cList_Client.DeleteItem(0);								//删除第一条数据记录
		for(vi=0;vi<m_cList_Client.GetItemCount();vi++)					//重新分配序号
		{
			v_sTemp.Format("%04d",vi+1);
			m_cList_Client.SetItemText(vi,0," "+v_sTemp);
		}
	}
	m_cList_Client.SetRedraw(true);										//更新界面
	m_cList_Client.EnsureVisible(m_cList_Client.GetItemCount()-1,false);//显示最新一条记录
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：日志列表窗中增加一行通信信息
//	参    数：v_sType：类型；	v_sAlarm：告警信息；
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Log(CString v_sType,CString v_sAlarm)
{
	int		vi,v_iIndex;												//临时变量
	CString v_sTemp;													//临时变量
	CString v_sCurTime;													//临时变量
	CTime	v_CurTime=CTime::GetCurrentTime();

	v_sCurTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	v_iIndex = m_cList_Log.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_cList_Log.SetItemText(v_iIndex,0," "+v_sTemp);					//序号
	m_cList_Log.SetItemText(v_iIndex,1,v_sCurTime);						//发生时间
	m_cList_Log.SetItemText(v_iIndex,2,v_sType);						//类型
	m_cList_Log.SetItemText(v_iIndex,3,v_sAlarm);						//告警信息

	//列表窗条数控制：超过1000条，删除300条；
	m_cList_Log.SetRedraw(FALSE);										//不更新界面
	if (m_cList_Log.GetItemCount()>=1000)								//如果纪录数大于1000
	{
		for(vi=0;vi<300;vi++)											//删除前300条
			m_cList_Log.DeleteItem(0);									//删除第一条数据记录
		for(vi=0;vi<m_cList_Log.GetItemCount();vi++)					//重新分配序号
		{
			v_sTemp.Format("%04d",vi+1);
			m_cList_Log.SetItemText(vi,0," "+v_sTemp);
		}
	}
	m_cList_Log.SetRedraw(true);										//更新界面
	m_cList_Log.EnsureVisible(m_cList_Log.GetItemCount()-1,false);		//显示最新一条记录
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：指令列表窗中增加一行通信信息
//	参    数：v_sIP：客户端IP；	v_sName：客户端名称；	v_sBSC：BSC；	v_sCommand：指令；
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Command(CString v_sIP,CString v_sName,CString v_sBSC,CString v_sCommand)
{
	int		vi,v_iIndex;												//临时变量
	CString v_sTemp;													//临时变量
	CString v_sCurTime;													//临时变量
	CTime	v_CurTime=CTime::GetCurrentTime();

	v_sCurTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	v_iIndex = m_cList_Command.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_cList_Command.SetItemText(v_iIndex,0," "+v_sTemp);				//序号
	m_cList_Command.SetItemText(v_iIndex,1,v_sCurTime);					//时间
	m_cList_Command.SetItemText(v_iIndex,2,v_sIP);						//IP
	m_cList_Command.SetItemText(v_iIndex,3,v_sName);					//姓名
	m_cList_Command.SetItemText(v_iIndex,4,v_sBSC);						//BSC
	m_cList_Command.SetItemText(v_iIndex,5,v_sCommand);					//指令

	//列表窗条数控制：超过1000条，删除300条；
	m_cList_Command.SetRedraw(FALSE);									//不更新界面
	if (m_cList_Command.GetItemCount()>=1000)							//如果纪录数大于1000
	{
		for(vi=0;vi<300;vi++)											//删除前300条
			m_cList_Command.DeleteItem(0);								//删除第一条数据记录
		for(vi=0;vi<m_cList_Command.GetItemCount();vi++)				//重新分配序号
		{
			v_sTemp.Format("%04d",vi+1);
			m_cList_Command.SetItemText(vi,0," "+v_sTemp);
		}
	}
	m_cList_Command.SetRedraw(true);										//更新界面
	m_cList_Command.EnsureVisible(m_cList_Command.GetItemCount()-1,false);	//显示最新一条记录
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：指令执行结果显示
//	参    数：v_tpTimespan：历时；	v_sType：类型；	v_sLog：Log信息；
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Command_Text(CString v_sText)
{
	CString		v_sTemp;												//临时变量
	COleDateTime v_CurTime_Text=COleDateTime::GetCurrentTime();			//当前时间

	COleDateTimeSpan v_tSpan_Text=v_CurTime_Text-v_tDate;				//历时
	int		v_iSeconds=v_tSpan_Text.GetTotalSeconds();					//秒
	v_sTemp.Format("%d",v_iSeconds);									//历时
	if (m_cList_Command.GetItemCount()>0)
	{
		m_cList_Command.SetItemText(m_cList_Command.GetItemCount()-1,6,v_sTemp);	//历时：指令执行
		m_cList_Command.SetItemText(m_cList_Command.GetItemCount()-1,7,v_sText);	//指令执行结果
	}
}
//------------------------------------------------------------------------------------------------------			
//	函数功能：Bug列表窗中增加一行通信信息
//	参    数：v_sBug_Type：异常类型；	v_sBug：异常信息
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::My_ShowList_Bug(CString v_sBug_Type,CString v_sBug)
{
	int		vi,v_iIndex;												//临时变量
	CString v_sTemp;													//临时变量
	CString v_sCurTime;													//临时变量
	CTime	v_CurTime=CTime::GetCurrentTime();

	v_sCurTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	v_iIndex = m_cList_Bug.InsertItem(0xffff,_T("0"));
	v_sTemp.Format("%04d",v_iIndex+1);
	m_cList_Bug.SetItemText(v_iIndex,0," "+v_sTemp);					//序号
	m_cList_Bug.SetItemText(v_iIndex,1,v_sCurTime);						//发生时间
	m_cList_Bug.SetItemText(v_iIndex,2,v_sBug_Type);					//异常类型
	m_cList_Bug.SetItemText(v_iIndex,3,v_sBug);							//异常信息

	//列表窗条数控制：超过1000条，删除300条；
	m_cList_Bug.SetRedraw(FALSE);										//不更新界面
	if (m_cList_Bug.GetItemCount()>=1000)								//如果纪录数大于1000
	{
		for(vi=0;vi<300;vi++)											//删除前300条
			m_cList_Bug.DeleteItem(0);									//删除第一条数据记录
		for(vi=0;vi<m_cList_Bug.GetItemCount();vi++)					//重新分配序号
		{
			v_sTemp.Format("%04d",vi+1);
			m_cList_Bug.SetItemText(vi,0," "+v_sTemp);
		}
	}
	m_cList_Bug.SetRedraw(true);										//更新界面
	m_cList_Bug.EnsureVisible(m_cList_Bug.GetItemCount()-1,false);		//显示最新一条记录
}

//------------------------------------------------------------------------------------------------------
//	函数功能：SQL执行函数
//	参    数：v_sSql：SQL命令；
//------------------------------------------------------------------------------------------------------
bool CServer_DataDlg::ExecuteSQL(CString v_sSql)
{
	// TODO: Add your control notification handler code here
	CMyADOConn v_cAdoConn;												//数据库实例
	try
	{
		// 连接数据库
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(120000);
		v_cAdoConn.ExecuteSQL((_bstr_t)v_sSql);							//执行数据库操作
		v_cAdoConn.ExitConnect();										//断开连接
		return true;
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//关闭数据库
		CString v_sError = (char *)e.Description();						//获得：出错信息
		My_ShowList_Bug("保存数据表","异常："+v_sError);				//显示：异常
		theApp.m_MyLog.WriteText("异常："+v_sError+"  "+v_sSql+"\r\n");	//log
		return false;
	}
}

//------------------------------------------------------------------------------------------------------			
//函数功能：显示文件处理信息
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::ShowListCommand(CString sFileName,CString sMsg)
{
	int iIndex,vi;
	CString sTemp,sTime;
	CTime	v_CurTime=CTime::GetCurrentTime();
	iIndex = m_cList_Command.InsertItem(0xffff,_T("0"));
	sTemp.Format("%04d",iIndex+1);
	sTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");
	m_cList_Command.SetItemText(iIndex,0," "+sTemp);					//序号
	m_cList_Command.SetItemText(iIndex,1,sTime);					//发生时间
	m_cList_Command.SetItemText(iIndex,2,sFileName);				    //文件名称
	m_cList_Command.SetItemText(iIndex,3,sMsg);				    //文件处理过程

	//列表窗条数控制：超过1000条，删除300条；
	m_cList_Command.SetRedraw(FALSE);									//不更新界面
	if (m_cList_Command.GetItemCount()>=1000)							//如果纪录数大于1000
	{
		for(vi=0;vi<300;vi++)											//删除前300条
			m_cList_Command.DeleteItem(0);								//删除第一条数据记录
		for(vi=0;vi<m_cList_Command.GetItemCount();vi++)				//重新分配序号
		{
			sTemp.Format("%04d",vi+1);
			m_cList_Command.SetItemText(vi,0," "+sTemp);
		}
	}
	m_cList_Command.SetRedraw(true);										//更新界面
	m_cList_Command.EnsureVisible(m_cList_Command.GetItemCount()-1,false);	//显示最新一条记录
	
}

//------------------------------------------------------------------------------------------------------			
//函数功能：显示详细文件处理信息
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::ShowListLog(CString sPanTime,CString sFileName,CString sMsg)
{
	int iIndex,vi;
	CString sTemp,sTime;
	CTime	v_CurTime=CTime::GetCurrentTime();
	v_CurTime=CTime::GetCurrentTime();                                  //获得当前时间
	sTime=v_CurTime.Format("%y.%m.%d_%H:%M:%S");

	iIndex = m_cList_Log.InsertItem(0xffff,_T("0"));
	sTemp.Format("%04d",iIndex+1);
	m_cList_Log.SetItemText(iIndex,0," "+sTemp);					//序号	
	m_cList_Log.SetItemText(iIndex,1,sTime);					    //发生时间
	m_cList_Log.SetItemText(iIndex,2,sPanTime);					    //处理花费时间
	m_cList_Log.SetItemText(iIndex,3,sFileName);				    //文件名称
	m_cList_Log.SetItemText(iIndex,4,sMsg);				        //文件处理过程

	//列表窗条数控制：超过1000条，删除300条；
	m_cList_Log.SetRedraw(FALSE);										//不更新界面
	if (m_cList_Log.GetItemCount()>=1000)								//如果纪录数大于1000
	{
		for(vi=0;vi<300;vi++)											//删除前300条
			m_cList_Log.DeleteItem(0);									//删除第一条数据记录
		for(vi=0;vi<m_cList_Log.GetItemCount();vi++)					//重新分配序号
		{
			sTemp.Format("%04d",vi+1);
			m_cList_Log.SetItemText(vi,0," "+sTemp);
		}
	}
	m_cList_Log.SetRedraw(true);										//更新界面
	m_cList_Log.EnsureVisible(m_cList_Log.GetItemCount()-1,false);		//显示最新一条记录
}
//------------------------------------------------------------------------------------------------------			
//函数功能：解压GZ文件
//参    数：szFileName：压缩文件路径；sFileName：解压文件路径
//------------------------------------------------------------------------------------------------------
BOOL CServer_DataDlg::PMArchive(CString szFileName,CString sFileName)
{
	CDecompressClass	    m_Decompress;									//实例化
	CString m_szFileName;
	BOOL m_fIsFile;
	char* pBuffer;														    //缓冲区指针
	int nLen;
	//文件解压
	m_szFileName="";
	m_fIsFile=false;
	m_Decompress.CloseArchive();										    //关闭解压对象
	if (!m_Decompress.OpenArchive(szFileName))	                            //打开、解压文件？？？
	{																	    //解压失败
		return FALSE;													    //返回
	}
	//获取：文件信息															//数据长度
	if (!m_Decompress.GetArchiveFile(0,									    //文件头
		&pBuffer,														    //获取：数据指针
		nLen,															    //获取：数据长度
		m_fIsFile,														    //
		m_szFileName))														//文件信息获取成功？？？
	{
		My_ShowList_Bug("解压文件","文件信息异常："+sFileName);				//显示：异常
		return FALSE;													    //返回
	}
	//保存解压的XML文件
	if(!v_fWriter.Open(sFileName, CFile::modeCreate | CFile::modeWrite))	//打开文件，如不存在则生成，如存在则清空文件；
	{
		My_ShowList_Bug("解压压缩文件","文件保存异常："+sFileName);			//显示：异常
		//失败数量
		return FALSE;														//返回
	}					
	v_fWriter.Write(pBuffer,nLen);											//写入文件【依据：数据指针、数据长度】
	v_fWriter.Close();														//文件关闭
	m_Decompress.CloseArchive();											//文件关闭[压缩文件]
	return TRUE;
}

//------------------------------------------------------------------------------------------------------			
//函数功能：程序控制参数
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::OnBnClickedSystemParm()
{
	// TODO: 在此添加控件通知处理程序代码
	list<map_ObjType>::iterator iterator_ObjType;
	queue<CString>  temp_file_queue;
	stack<CString>  temp_sql_stack;
	CString temp;

	//系统参数
	DB_maintenance_lock_.Lock();
	DB_maintenance_.Init();
	DB_maintenance_lock_.Unlock();
	PARMDialog.PmLTEMonth = DB_maintenance_.pm_save_month_;
	PARMDialog.PmLTEDayFTP = DB_maintenance_.log_save_day_;
	PARMDialog.PmLTEDay = DB_maintenance_.pm_save_day_;
	PARMDialog.PmLTETimeData = DB_maintenance_.granularity_time_;

	//读取基本性能队列【1】
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

	//读取计算性能队列【2】
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

	//读取全网性能队列【3】
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

	//读取区域性能队列【4】
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

	//读取文件队列【5】
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

	//读取sql队列【6】
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

	//读取小时基本性能队列【7】
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

	//读取小时计算性能队列【8】
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

	//读取华为性能队列【9】
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

	//读取中兴性能队列【10】
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
//函数功能:数据变量初始化
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::Init()
{
	//判断创建网络驱动器
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
	//获得小区其对应的唯一编码
	GetCellMap(cellname_map_ID_);
}

//------------------------------------------------------------------------------------------------------			
//函数功能:数据变量更新
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
	//获得小区其对应的唯一编码
	GetCellMap(cellname_map_ID_);
}

//--------------------------------------------------------------------------------------------------------------------------			
//函数功能:执行bulk insert栈
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
//函数功能：获得文件中粒度时间【参数：szFileName压缩文件名称；sFileTime文件中提取的时间信息】
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
//函数功能：小区名称映射为整数
//参   数： v_mObjType :接口小区数据；cellname_map_ID：小区名称映射
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::MapCellNameToID(map_ObjType &v_mObjType,map_Counter &cellname_map_ID)
{
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	list_Object::iterator	iterator_Object;							//count小区迭代器
	map<CString,CString>::iterator iterator_cell;                       //小区查询迭代器
	iterator_objtype = v_mObjType.begin();
	while(iterator_objtype != v_mObjType.end())
	{
		iterator_Object = iterator_objtype->second.begin();
		while(iterator_Object != iterator_objtype->second.end())
		{
			iterator_Object->v_sObject_ID.Replace("（","(");
			iterator_Object->v_sObject_ID.Replace("）",")");
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
//  函数功能:获得小区其对应的唯一编码
//  参    数:cellname_map_ID : 小区及其编码
//-----------------------------------------------------------------------------------------------------------------
void CServer_DataDlg:: GetCellMap(map<CString,CString> &cellname_map_ID)
{
	_RecordsetPtr v_pRecordset;											  //记录集
	CMyADOConn v_cAdoConn;												  //数据库实例
	CString		   sql,sTemp,Key,value;							      //临时变量
	int            iTemp;
	try
	{
		sql="SELECT zCell,zCell_ID FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		          //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			sTemp = v_cAdoConn.GetField_String("zCell");
			Key = sTemp;
			iTemp = v_cAdoConn.GetField_Int("zCell_ID");
			value.Format("%d",iTemp);
			cellname_map_ID.insert(pair<CString,CString>(Key,value));
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										  //关闭数据库

		CString v_sError = (char *)e.Description();						  //获得：出错信息
		theApp.m_MyLog.WriteText("异常："+v_sError+"\r\n");	              //log
	}
}

//------------------------------------------------------------------------------------------------------
//	函数功能：接口变量清零
//  参    数：ObjType：接口
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::ResetObjType(map_ObjType &ObjType)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjType指示器
	list_Object::iterator	v_pIterator_Object;							//list_Object迭代器
	v_pIterator_ObjType = ObjType.begin();
	while(v_pIterator_ObjType != ObjType.end())
	{
		v_pIterator_Object=v_pIterator_ObjType->second.begin();
		while(v_pIterator_Object!=v_pIterator_ObjType->second.end())
		{
			v_pIterator_Object->v_mCounter.clear();						//Counter容器：清除
			v_pIterator_Object++;
		}
		v_pIterator_ObjType->second.clear();							//Object链表：清除
		v_pIterator_ObjType++;
	}
	ObjType.clear();													//ObjType容器：清除
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	函数功能：获得文件信息
//  参   数：socket_file_name:socket传输过来文件名称；zip_file_name:压缩文件名称；zip_file_path:压缩文件路径；xml_file_name：xml文件名称；xml_file_path：解压文件路径；file_name：文件名称；file_time：文件时间;file_type:文件类型【中兴或华为】
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CServer_DataDlg::GetFileInformation(CString socket_file_name,CString &zip_file_name,CString &zip_file_path,CString &xml_file_name,CString &xml_file_path,CString &file_name,CString &file_time,CString &file_type)
{
	CString temp;
	//压缩文件路径
	temp = socket_file_name;
	temp.Delete(0,3);
	zip_file_path = theApp.a_sPm_Path_ + "\\" + temp;  

	//获得压缩文件名称
	temp = zip_file_path;
	temp.Delete(0,24);
	zip_file_name = temp;

	//解压XML文件名称及路径
	xml_file_name = "Pmfile.xml";
	xml_file_path = ".\\DATA\\" + xml_file_name;
	file_name = zip_file_name;
	file_name.Delete(file_name.GetLength()-7,7);

	//获得文件时间
	GetFileTime(zip_file_name,file_time);

	//文件类型判断(中兴还是华为)
	if (zip_file_name.Left(1) == "C")
		file_type = "中兴";
	else
		file_type = "华为";
}

//-------------------------------------------------------------------------------------------------------------------		
//函数功能:处理计时
//参   数：key:名称；message;显示消息;before_time:起始时间
//-------------------------------------------------------------------------------------------------------------------
void CServer_DataDlg::KeepTime(CString key,CString message,COleDateTime &before_time)
{
	CString span_time;
	COleDateTimeSpan Ole_span_time;
	Ole_span_time = COleDateTime::GetCurrentTime() - before_time;//更新历时
	span_time = Ole_span_time.Format("%H:%M:%S");
	ShowListLog(span_time,key,message);
	if (message == "完成15分钟粒度数据处理总历时")
	{
		theApp.m_MyLog.WriteText(key + "： " + span_time + "\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------
//	函数功能：删除重复粒度数据
//------------------------------------------------------------------------------------------------------
void CServer_DataDlg::DeleteNextGranularityData(CString granlarity)
{
	_RecordsetPtr   v_pRecordset;									                         //记录集
	CMyADOConn      v_cAdoConn;												                 //数据库实例
	CString			sql,sql_key="",sql_value="",time;							             //临时变量
	COleDateTime    delete_next_granularity;		                                         //更新历时
	CString         sdelete_next_granularity;
	delete_next_granularity.ParseDateTime(granlarity);
	delete_next_granularity += COleDateTimeSpan(0,0,15,0);
	sdelete_next_granularity = delete_next_granularity.Format("%Y-%m-%d %H:%M");
	map_Counter::iterator	iterator_whole_network_performance_updata;				         //count性能迭代器
	try
	{   
		time = sdelete_next_granularity.Mid(0,4)+sdelete_next_granularity.Mid(4,4);
		time.Replace("-","");
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);			
		v_cAdoConn.SetCommandTimeout(60000);
		sql = "delete from PM_Alarm where ztime = '" + sdelete_next_granularity + "'";	
		v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
		sql = "delete from Pm_LTE_Region where ztime = '" + sdelete_next_granularity + "'";			
		v_cAdoConn.ExecuteSQL((_bstr_t)sql);	
		//删除重复小时粒度数据的数据
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
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_MAC_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PDCP_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PHY_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_RRC_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_THO_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_TXT_" + time  + "_1 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				break;
			case 1:
				sql = "delete from Pm_LTE_Cal_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_MAC_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PDCP_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PHY_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_RRC_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_THO_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_TXT_" + time  + "_2 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				break;
			case 2:
				sql = "delete from Pm_LTE_Cal_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_MAC_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PDCP_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PHY_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_RRC_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_THO_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_TXT_" + time  + "_3 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				break;
			case 3:
				sql = "delete from Pm_LTE_Cal_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_MAC_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PDCP_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PHY_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_RRC_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_THO_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_TXT_" + time  + "_4 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				break;
			case 4:
				sql = "delete from Pm_LTE_Cal_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_MAC_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PDCP_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PHY_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_RRC_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_THO_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_TXT_" + time  + "_5 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				break;
			default:
				sql = "delete from Pm_LTE_Cal_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_MAC_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PDCP_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_PHY_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_RRC_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_THO_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				sql = "delete from Pm_LTE_TXT_" + time  + "_6 where ztime = '" + sdelete_next_granularity + "'";			
				v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
				break;
			}

		}
		//删除重复15分钟粒度数据的数据
		{
			CString temp_time = sdelete_next_granularity;
			temp_time.Replace("-","");
			temp_time = temp_time.Left(8);
			sql = "delete from Pm_LTE_Cal_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
			sql = "delete from Pm_LTE_MAC_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
			sql = "delete from Pm_LTE_PDCP_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
			sql = "delete from Pm_LTE_PHY_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
			sql = "delete from Pm_LTE_RRC_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
			sql = "delete from Pm_LTE_THO_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
			sql = "delete from Pm_LTE_TXT_" + temp_time + " where ztime = '" + sdelete_next_granularity + "'";			
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//执行数据库操作
		}
		v_cAdoConn.ExitConnect();										//断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//关闭数据库
		CString v_sError = (char *)e.Description();						//获得：出错信息
		My_ShowList_Bug("保存数据表","异常："+v_sError);				    //显示：异常
		theApp.m_MyLog.WriteText("异常："+v_sError+"  "+sql+"\r\n");	    //log
	}
}

//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------
