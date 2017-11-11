#include "StdAfx.h"
#include "ThreadPool_IOCP.h"
#include "process.h"


//extern CServerTestApp theApp;

//-------------------------------------------------------------------------------------------------
//	功能：	构造函数
//-------------------------------------------------------------------------------------------------
CThreadPool::CThreadPool()
{
	m_hManagerIO	=	NULL;											//管理完全IO端口
	m_hWorkerIO		=	NULL;											//工作线程完全IO端口

	m_hManagerThread=	NULL;											//管理线程据柄
	m_pThread_Data	=	NULL;											//线程池数据结构
	m_pIocp_Data	=	NULL;											//IOCP数据池数据结构
	m_pIocp_Send	=	NULL;											//IOCP数据池数据结构
	
	m_dwMSeconds	=	200;											//延时设置
	m_bQuitManager	=	false;											//管理线程退出标志

	v_fRun			=	0;												//工作线程数量

	::InitializeCriticalSection(&m_csLock);								//初使化互斥体
}

//-------------------------------------------------------------------------------------------------
//	功能：	析构函数
//-------------------------------------------------------------------------------------------------
CThreadPool::~CThreadPool()
{
	if(m_hManagerIO)													//关闭管理完全IO端口
	{
		::CloseHandle(m_hManagerIO);
	}
	if(m_hWorkerIO)														//关闭线程完全IO端口
	{
		::CloseHandle(m_hWorkerIO);
	}
	if(m_pThread_Data)													//删除线程数据结构
	{
		delete [] m_pThread_Data;
	}
	if(m_pIocp_Data)
	{
		delete [] m_pIocp_Data;											//删除IOCP数据池结构数据			
	}
	if(m_pIocp_Send)
	{
		delete [] m_pIocp_Send;											//删除IOCP数据池结构数据			
	}

	v_mIOCP_Rece.clear();												//删除IOCP接收数据池

	::DeleteCriticalSection(&m_csLock);									//关闭互斥体
}

//-------------------------------------------------------------------------------------------------
//	功能：	启动完全IO端口线程池
//	参数：	(1)wStatic：静态线程数；
//			(2)wMax：动态最大的线程数
//			(3)lpDealFunc：回调函数
//-------------------------------------------------------------------------------------------------
bool CThreadPool::Start(WORD wStatic,WORD wMax,DEALPROC lpDealFunc,DEALPROC_Close lpDealFunc_Close)
{
	if(!(wStatic && wMax))												//全为零
		return false;													//退出
	
	m_wStaticThreadNum	=	wStatic;									//静态线程数
	m_wMaxThreadNum		=	wMax;										//动态最大的线程数

	::EnterCriticalSection(&m_csLock);									//LOCK互斥体
	
	//创建工作线程池数据---------------------------------------------------------------------------
	if(m_pThread_Data)													//如果存在
	{
		delete [] m_pThread_Data;										//则删除
		m_pThread_Data = NULL;
	}
	m_pThread_Data = new PRE_THREAD_DATA[wMax]();						//创建工作线程池(空)，wMax个
	if(m_pThread_Data == NULL)											//创建失败
		return false;													//退出

	//创建IOCP接收数据池数据-----------------------------------------------------------------------
	if(m_pIocp_Data)													//如果存在
	{
		delete [] m_pIocp_Data;											//则删除
		m_pIocp_Data = NULL;
	}
	m_pIocp_Data = new PER_IOCP_DATA[ACCEPT_MAX]();						//创建IOCP接收数据池(空)
	if(m_pIocp_Data == NULL)											//创建失败
		return false;													//退出
	for(int v_iNum=0;v_iNum<THREAD_MAX;v_iNum++)
		m_pIocp_Data[v_iNum].v_iOperationType=-1;						//初始状态

	//创建IOCP发送数据池数据-----------------------------------------------------------------------
	if(m_pIocp_Send)													//如果存在
	{
		delete [] m_pIocp_Send;											//则删除
		m_pIocp_Send = NULL;
	}
	m_pIocp_Send = new PER_IOCP_DATA[THREAD_MAX]();						//创建IOCP发送数据池(空)
	if(m_pIocp_Send == NULL)											//创建失败
		return false;													//退出
	for(int v_iNum=0;v_iNum<THREAD_MAX;v_iNum++)
		m_pIocp_Send[v_iNum].v_iOperationType=-1;

	//创建异步的不关联文件的完全IO端口-------------------------------------------------------------
	if(m_hManagerIO)													//如果存在
	{
		::CloseHandle(m_hManagerIO);									//则关闭
		m_hManagerIO = NULL;
	}
	m_hManagerIO = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
																		//创建管理完全IO端口
	if(m_hManagerIO	== NULL)											//创建失败
		return false;													//退出

	if(m_hWorkerIO)														//如果存在
	{
		::CloseHandle(m_hWorkerIO);										//则关闭
		m_hManagerIO = NULL;
	}
	m_hWorkerIO	= ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
																		//创建工作完全IO端口
	if(m_hWorkerIO == NULL)												//创建失败
		return false;													//退出

	//创建管理线程---------------------------------------------------------------------------------
// 	m_bQuitManager = false;												//管理线程强制退出标志(false:不退出)
// 	if(m_hManagerThread)												//如果存在
// 	{
// 		::TerminateThread(m_hManagerThread,0);							//则关闭
// 		m_hManagerThread = NULL;
// 	}
// 	unsigned int ManagerThreadID;
// 	m_hManagerThread = (HANDLE)_beginthreadex(NULL,0,ManagerProc,this,0,&ManagerThreadID);
// 																		//创建管理线程
// 	if(m_hManagerThread == NULL)										//创建失败
// 		return false;													//退出

	m_pDealFunc=lpDealFunc;												//设置回调函数
	m_pDealFunc_Close=lpDealFunc_Close;									//设置回调函数

	//创建工作线程---------------------------------------------------------------------------------
	for(int v_iNum=0;v_iNum<wStatic;v_iNum++)							//创建静态工作线程，wStatic个
	{
		m_pThread_Data[v_iNum].hThreadHandle = (HANDLE)_beginthreadex(NULL,0,WorkerProc,this,0,&m_pThread_Data[v_iNum].dwThreadID);
																		//创建工作线程
	}
	
	::LeaveCriticalSection(&m_csLock);									//UNLOCK互斥体

	return true;
}

//-------------------------------------------------------------------------------------------------
//	功能：	关闭完全IO端口线程池
//-------------------------------------------------------------------------------------------------
void CThreadPool::Stop(void)
{
	DWORD dwRes;
	int v_iNum;

	::EnterCriticalSection(&m_csLock);									//LOCK互斥体	

	//判断并关闭管理线程---------------------------------------------------------------------------
// 	m_bQuitManager = true;												//向管理线程发送强制关闭线程消息
// 	for(v_iNum=0;v_iNum<10;v_iNum++)									//循环10次，等待退出
// 	{
// 		::GetExitCodeThread(m_hManagerThread,&dwRes);					//获得线程退出码
// 		if(dwRes == CThreadPool::MANAGERPROC_RETURN_VALUE)				//退出码为管理线程推出码
// 			break;														//退出
// 		if(v_iNum == 9)
// 			::TerminateThread(m_hManagerThread,0);						//最后强制关闭管理线程
//		else
//			::Sleep(100);												//延时1s等待退出
//	}

	//关闭所有工作线程-----------------------------------------------------------------------------	
	for(v_iNum=0;v_iNum<m_wMaxThreadNum;v_iNum++)						//所有工作线程
	{
		if(m_pThread_Data[v_iNum].dwThreadID == 0)
			continue;
		m_pThread_Data[v_iNum].bIsQuit = true;							//退出标志有效
		for(int v_iNum1=0;v_iNum1<10;v_iNum1++)
		{
			::GetExitCodeThread(m_pThread_Data[v_iNum].hThreadHandle,&dwRes);
																		//获得线程退出码
			if(dwRes == CThreadPool::WORKERPROC_RETURN_VALUE)			//退出码为工作线程推出码
				break;													//退出
			if(v_iNum1 == 9)
				::TerminateThread(m_pThread_Data[v_iNum].hThreadHandle,0);
																		//最后强制关闭工作线程
//			else
//				::Sleep(10000);											//延时0.5s等待退出
		}
	}

	//关闭完全IO端口-------------------------------------------------------------------------------
	::CloseHandle(m_hManagerIO);										//关闭管理
	m_hManagerIO = NULL;
	::CloseHandle(m_hWorkerIO);											//关闭工作线程
	m_hWorkerIO = NULL;

	//删除数据结构---------------------------------------------------------------------------------
	if(m_pThread_Data)
	{
		delete [] m_pThread_Data;										//删除线程池结构数据			
		m_pThread_Data = NULL;
	}
	if(m_pIocp_Data)
	{
		delete [] m_pIocp_Data;											//删除IOCP接收数据池结构数据			
		m_pIocp_Data = NULL;
	}
	if(m_pIocp_Send)
	{
		delete [] m_pIocp_Send;											//删除IOCP发送数据池结构数据			
		m_pIocp_Send = NULL;
	}

	//删除所有待处理数据---------------------------------------------------------------------------
	unsigned long		pN1;
	unsigned long long  pN2;
	OVERLAPPED*			pOverLapped;	
	while(::GetQueuedCompletionStatus(m_hWorkerIO,&pN1,&pN2,&pOverLapped,0))
																		//获得工作IOCP完成端口数据
	{

	}
	
	::LeaveCriticalSection(&m_csLock);									//UNLOCK互斥体
}

//-------------------------------------------------------------------------------------------------
//	功能：	管理线程
//	性能：	根据当前工作线程的工作状态，确定增加线程数量(<=动态数)，或减少线程数量(>=静态数)；
//-------------------------------------------------------------------------------------------------
unsigned __stdcall CThreadPool::ManagerProc(void* pThread)
{
// 	CServerTestDlg* pDlg;												//对话窗指针
// 	pDlg=(CServerTestDlg*)theApp.m_pMainWnd;							//获得对话窗窗口指针
	unsigned long	pN1;											//IOCP调用参数
	unsigned long long pN2;
	OVERLAPPED*		pOverLapped;										//IOCP调用参数
	
	CThreadPool* pServer = reinterpret_cast<CThreadPool*>(pThread);		//类指针
	
	while(!pServer->m_bQuitManager)										//强制退出标志？
	{
		if(::GetQueuedCompletionStatus(pServer->m_hManagerIO,&pN1,&pN2,&pOverLapped,pServer->m_dwMSeconds)	==	TRUE)
																		//线程挂起，直到m_hManagerIO队列中出现了一项或超时
		{
			if(pOverLapped == (OVERLAPPED*)0xFFFFFFFF)					//收到退出码
			{
				break;													//强制退出
			}
		}
		else
		{
			EThreadStatus stat = pServer->GetWorkThreadStatus();		//超时，获得工作线程状态
			if(stat	==	CThreadPool::BUSY)								//如果为：BUSY
			{
//				pServer->AddThread();									//增加线程
			}
			else
			{
				if(stat == CThreadPool::IDLE)							//如果为：IDLE
				{
//					pServer->DelThread();								//删除线程
				}
			}
		}
	}
		
	_endthreadex(CThreadPool::MANAGERPROC_RETURN_VALUE);				//退出线程
	
	return CThreadPool::MANAGERPROC_RETURN_VALUE;						//返回退出码
}

//-------------------------------------------------------------------------------------------------
//	功能：	工作线程
//	性能：	在线程中完成接入的SOCKET的数据接收处理；
//	函数：	::GetQueuedCompletionStatus(pServer->m_hWorkerIO,lpNumberOfBytes,lpCompletionKey,lpOverLapped,m_dwMSeconds))
//	参数：	(1)把当前线程与 m_hWorkerIO 完成端口相关联，即把线程ID放入IOCP完成端口等待线程队列中,
//			IOCP内核对象就总能知道哪个线程在等待处理完成的I/O请求；
//			当端口的I/O完成队列中出现一项时，完成端口就唤醒等待线程队列中的一个线程，
//			该线程将得到完成的I/O项中的信息：传输的字节数、完成键和OVERLAPPED结构的地址，即2、3、4参数；
//			(2)lpNumberOfBytes 传输的字节数；
//			(3)lpCompletionKey称为完成键，由它传递的数据称为单句柄数据，用于传递通信中的 客户 SOCKET 参数；
//			其结构地址与 CreateIoCompletionPort 中的 completion key对应；
//			(4)lpOverlapped称为重叠结构体，由它传递的数据称为单IO数据，用于传递通信中的 信息数据 参数；
//			其结构地址与 wsarecv 或 wsasend 中的OverLapped 对应。
//			(5)如果在 m_dwMSeconds 规定的时间内IOCP上还没有出现一个Completion Packet，就返回超时；
//-------------------------------------------------------------------------------------------------
unsigned __stdcall CThreadPool::WorkerProc(void* pThread)
{
	CString				v_sTemp;										//临时变量
	unsigned long		v_lBuff_Len;							//IOCP调用参数
	unsigned long long v_pSocket;
	OVERLAPPED*			pOverLapped;									//IOCP调用参数
	CThreadPool*		pServer = reinterpret_cast<CThreadPool*>(pThread);
																		//IOCP类指针
	LPPER_IOCP_DATA		v_pIocp;										//IOCP数据指针

	DEALPROC		lpDealFunc=pServer->m_pDealFunc;					//回调函数：数据接收
	DEALPROC_Close	lpDealFunc_Close=pServer->m_pDealFunc_Close;		//回调函数：客户端关闭
	DWORD threadID = ::GetCurrentThreadId();							//获得线程ID
	int	v_iSeq = pServer->GetThreadbyID(threadID);						//获得线程池序号	
	if(v_iSeq<0)														//无效序号
		return 0;														//线程无效退出

	//IOCP工作线程完成端口循环体
	while(!pServer->m_pThread_Data[v_iSeq].bIsQuit)						//退出标志有效？
	{
		if(::GetQueuedCompletionStatus(pServer->m_hWorkerIO,&v_lBuff_Len,&v_pSocket,&pOverLapped,pServer->m_dwMSeconds))
																		//线程挂起，直到m_hWorkerIO队列中出现了一项或超时
		{
			//(1)数据指针处理
			v_pIocp=(LPPER_IOCP_DATA)pOverLapped;						//指向重叠IO数据区

			//(2)判断客户socket工作状态：先检查一下，看看是否在套接字上已有错误发生
		    if (v_lBuff_Len==0 )										//客户退出？
			{
				try		
				{	
					lpDealFunc_Close((unsigned long long*)v_pSocket);	//回调函数：关闭SOCKET
					v_pIocp->v_iOperationType=-1;						//标志：空闲的缓冲区
				}
				catch(...)	
				{	
					break;	
				}
				continue;
			}
			//(3)设置线程工作状态：
			pServer->m_pThread_Data[v_iSeq].bIsBusy = true;				//在工作之前将状态设置为Busy

			//(4)数据处理	
			if(v_pIocp->v_iOperationType==0)							//数据接收标志
			{															
				v_pIocp->v_bBuffer[v_lBuff_Len]=0;						//数据结束标志
				try		
				{	
					v_pIocp->v_iOperationType=-1;						//标志：空闲的缓冲区
					lpDealFunc((unsigned long long*)v_pSocket,v_pIocp->v_bBuffer,v_lBuff_Len);	//回调函数传递数据
				}
				catch(...)	
				{	
					break;	
				}
			}
			else														//数据发送
			{															
				v_pIocp->v_iOperationType=-1;							//空闲标志
			}
			//(5)复位线程工作状态：
			pServer->m_pThread_Data[v_iSeq].bIsBusy = false;			//在工作后将状态设置为非Busy
		}
		else															//超时或故障
		{
			//(6)超时处理
		}
	}
	//(7)线程退出处理
	pServer->m_pThread_Data[v_iSeq].dwThreadID = 0;						//退出之前将线程ID设置为0
	_endthreadex(CThreadPool::WORKERPROC_RETURN_VALUE);					//退出线程

	return CThreadPool::WORKERPROC_RETURN_VALUE;						//返回退出码
}

//-------------------------------------------------------------------------------------------------
//	功能：	获得工作线程正在工作的数量
//	返回：	工作数量;
//-------------------------------------------------------------------------------------------------
int CThreadPool::GetWorkThread()
{
	return (int)v_fRun;													//工作数量
}

//-------------------------------------------------------------------------------------------------
//	功能：	获得工作线程状态
//	返回：	EThreadStatus	{BUSY,NORMAL,IDLE};
//-------------------------------------------------------------------------------------------------
CThreadPool::EThreadStatus CThreadPool::GetWorkThreadStatus()
{
	float	v_fAll;														//线程总数

	v_fAll=0.0;
	v_fRun=0.0;
	for(int v_iNum=0;v_iNum<m_wMaxThreadNum;v_iNum++)					//所有线程
	{
		if(m_pThread_Data[v_iNum].dwThreadID)							//有效线程？
		{
			v_fAll++;													//统计++
			if(m_pThread_Data[v_iNum].bIsBusy)							//线程工作？
				v_fRun++;												//统计++
		}
	}
	if(v_fAll == 0)														//无有效线程
		return CThreadPool::IDLE;										//返回IDLE
	if(v_fRun/(1.0*v_fAll)>=0.8)										//使用率>80%
		return CThreadPool::BUSY;										//返回BUSY
	if(v_fRun/(1.0*v_fAll)<=0.2)										//使用率<20%
		return CThreadPool::IDLE;										//返回IDLE
	return CThreadPool::NORMAL;											//其它：返回NORMAL
}

//-------------------------------------------------------------------------------------------------
//	功能：	增加线程(1个)
//-------------------------------------------------------------------------------------------------
void CThreadPool::AddThread(void)
{
	for(int v_iNum=m_wStaticThreadNum;v_iNum<m_wMaxThreadNum;v_iNum++)	//在静态线程数量与动态线程数量间
	{
		if(!m_pThread_Data[v_iNum].dwThreadID)
		{
			m_pThread_Data[v_iNum].bIsBusy = false;
			m_pThread_Data[v_iNum].bIsQuit = false;
			m_pThread_Data[v_iNum].hThreadHandle = (HANDLE)_beginthreadex(NULL,0,WorkerProc,this,0,&m_pThread_Data[v_iNum].dwThreadID);
																		//创建工作线程			
			break;														//退出
		}
	}
}

//-------------------------------------------------------------------------------------------------
//	功能：	关闭线程
//-------------------------------------------------------------------------------------------------
void CThreadPool::DelThread(void)
{
	for(int v_iNum=m_wMaxThreadNum-1;v_iNum>=m_wStaticThreadNum;v_iNum--)
																		//在静态线程数量与动态线程数量间
	{
		if(m_pThread_Data[v_iNum].dwThreadID != 0)
		{
			m_pThread_Data[v_iNum].bIsQuit = true;
			::Sleep(m_dwMSeconds);										//退出等待
			break;														//退出
		}
	}	
}

//-------------------------------------------------------------------------------------------------
//	功能：	获得线程ID的池序号
//	分析：	通过线程ID查找线程池序号
//-------------------------------------------------------------------------------------------------
int	CThreadPool::GetThreadbyID(DWORD dwID)
{
	for(int v_iNum=0;v_iNum<m_wMaxThreadNum;v_iNum++)
	{
		if(m_pThread_Data[v_iNum].dwThreadID == dwID)					//线程ID相等？
		{
			return v_iNum;												//返回线程池号
		}
	}

	return -1;
}

//-------------------------------------------------------------------------------------------------
//	功能：	投递接收数据请求
//	参数：	client_s：投递的客户SOCKET；
//	分析：	在接收数据区，寻找空闲的缓冲区，没有就增加；
//-------------------------------------------------------------------------------------------------
bool CThreadPool::RecvDataRequest(SOCKET client_s)
{
	bool	v_bOK=false;												//临时变量
	int		v_iNum=0;													//临时变量

	map<int,PER_IOCP_DATA>::iterator	v_pIter;						//迭代器
	
	v_cLock_Rece.Lock();												//加锁
	v_pIter=v_mIOCP_Rece.begin();										//遍历
	while(v_pIter!=v_mIOCP_Rece.end())
	{
		if (v_pIter->second.v_iOperationType==-1)						//查找空闲的缓冲区？？？
		{
			v_bOK=true;													//标志：找到空闲的缓冲区
			break;
		}
		v_pIter++;														//迭代器++
		v_iNum++;														//计算器
	}
	if (!v_bOK)															//没有找到？？？
	{																	//增加1个缓冲区
		PER_IOCP_DATA v_cIOCP_Rece;										//
		v_cIOCP_Rece.v_iOperationType=-1;								//标志：空闲的缓冲区
		v_mIOCP_Rece.insert(pair<int,PER_IOCP_DATA>(v_iNum,v_cIOCP_Rece));		//容器：增加
		v_pIter=v_mIOCP_Rece.find(v_iNum);								//获得：迭代器
	}
	v_cLock_Rece.Unlock();												//解锁

	// 单I/O操作数据
	DWORD	RecvBytes = 0;												//接收字节数
	DWORD	Flags = 0;													//标志
	ZeroMemory(&(v_pIter->second.v_pOverlapped), sizeof(OVERLAPPED));	//清重叠缓冲区
	v_pIter->second.v_dDataBuf.len = DATA_MAX ;							//缓冲区大小
	v_pIter->second.v_dDataBuf.buf = v_pIter->second.v_bBuffer;			//缓冲区地址
	v_pIter->second.v_iOperationType = 0;								//操作码：0：read ；1：write；
	WSARecv(client_s,													//
			&(v_pIter->second.v_dDataBuf),								//
			1,															//
			&RecvBytes,													//
			&Flags,														//
			(OVERLAPPED*)&(v_pIter->second),							//
			NULL);														//投递客户接收数据请求

	return true;														//返回投递状态
}

//-------------------------------------------------------------------------------------------------
//	功能：	投递发送数据请求
//	参数：	(1)client_s：投递的客户SOCKET；
//			(2)v_pBuffer：发送数据缓冲区地址
//			(3)v_iBuffer_Len：发送数据长度
//	分析：	把要发送的内容放入IOCP发送OVERLAPPED缓冲区，再投递发送请求
//-------------------------------------------------------------------------------------------------
bool CThreadPool::SendDataRequest(SOCKET client_s,char *v_pBuffer,int v_iBuffer_Len)
{
	DWORD	Flags = 0;													//标志
	DWORD	v_lSend_Len;												//发送数据长度
	int		v_iConnect_Num;												//发送缓冲区序号

	//数据COPY到发送数据区
	v_iConnect_Num=0;													//默认：发送缓冲区序号
	v_lSend_Len=(DWORD)v_iBuffer_Len;									//获得实际发送数据长度
	ZeroMemory(&(m_pIocp_Send[v_iConnect_Num].v_pOverlapped), sizeof(OVERLAPPED));			//清重叠缓冲区
	memcpy(m_pIocp_Send[v_iConnect_Num].v_bBuffer,(LPCTSTR)v_pBuffer,v_iBuffer_Len);		//把发送数据COPY到IOCP数据发送区
	m_pIocp_Send[v_iConnect_Num].v_dDataBuf.len = v_lSend_Len ;								//发送缓冲区大小
	m_pIocp_Send[v_iConnect_Num].v_dDataBuf.buf = m_pIocp_Send[v_iConnect_Num].v_bBuffer;	//发送缓冲区指针
	m_pIocp_Send[v_iConnect_Num].v_iOperationType = v_iConnect_Num+1;						//操作码：0：read ；>1：write；
	WSASend(client_s,													//
			&(m_pIocp_Send[v_iConnect_Num].v_dDataBuf),					//
			1,															//
			&v_lSend_Len,												//
			Flags,														//
			(OVERLAPPED*)&(m_pIocp_Send[v_iConnect_Num]),				//
			NULL);														//投递客户发送数据请求
	return true;														//返回投递状态
}

//-------------------------------------------------------------------------------------------------
//	结束
//-------------------------------------------------------------------------------------------------
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         