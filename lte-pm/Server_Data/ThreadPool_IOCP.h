#ifndef OTA_THREADPOOL_H_CAESAR__DEF
#define OTA_THREADPOOL_H_CAESAR__DEF

#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

#define DATA_MAX			2000										//数据缓冲区字节数(接收、发送)
#define	THREAD_MAX			8											//工作线程数量
#define ACCEPT_MAX			100											//可以连接的客户端数量

//-------------------------------------------------------------------------------------------------
//	功能：	定义回调函数
//	参数：	(1)v_pClient：客户端完成Key；
//			(2)v_pBuffer：发送数据缓冲区；
//			(3)v_iLength：发送数据长度；
//-------------------------------------------------------------------------------------------------
typedef void (CALLBACK* DEALPROC)(unsigned long long* v_pClient, const char *v_pBuffer, int v_iLength);
typedef void (CALLBACK* DEALPROC_Close)(unsigned long long* v_pClient);

class CThreadPool
{
	friend	static	unsigned __stdcall ManagerProc(void* pThread);
	friend	static	unsigned __stdcall WorkerProc(void* pThread);

	//定义数据结构---------------------------------------------------------------------------------
	typedef struct PRE_THREAD_DATA										//线程池数据结构
	{
		PRE_THREAD_DATA()
		{
			hThreadHandle	=	NULL;
			dwThreadID		=	0;
			bIsBusy			=	false;
			bIsQuit			=	false;
		}
		HANDLE			hThreadHandle;									//线程句柄
		unsigned int	dwThreadID;										//线程序号
		volatile bool	bIsBusy;										//线程工作状态：忙/空闲
		volatile bool	bIsQuit;										//线程强制退出标志
	}*LPPRE_THREAD_DATA;

	typedef struct PER_IOCP_DATA										//IOCP数据池,或单I/O操作数据
	{
		OVERLAPPED	v_pOverlapped;										//重叠结构
		WSABUF		v_dDataBuf;											//数据缓冲结构
		char		v_bBuffer[DATA_MAX];								//数据缓冲区
		int			v_iBufferLen;										//发送数据长度
		int			v_iOperationType;									//操作类型：0：空闲的缓冲区；1：接收；
	}*LPPER_IOCP_DATA;

	enum	EReturnValue												//线程返回值
	{
		MANAGERPROC_RETURN_VALUE = 10001,								//管理线程返回值
		WORKERPROC_RETURN_VALUE	= 10002									//工作线程返回值
	};
	enum	EThreadStatus												//线程工作状态
	{
		BUSY,															//忙
		NORMAL,															//正常
		IDLE															//空闲
	};

public:
	//系统公共函数---------------------------------------------------------------------------------

	CThreadPool();
	virtual ~CThreadPool();

	//定义公共函数---------------------------------------------------------------------------------
	bool	Start(WORD	wStatic,WORD wMax,DEALPROC lpDealFunc,DEALPROC_Close lpDealFunc_Close);		//启动IOCP
	void	Stop();														//关闭IOCP
	bool	RecvDataRequest(SOCKET client_s);							//投递接收数据请求
	bool	SendDataRequest(SOCKET client_s,char *v_pBuffer,int v_iBuffer_Len);		//投递发送数据请求
																		//投递发送数据请求
	int		GetWorkThread();											//获得工作线程数量

	//定义公共变量---------------------------------------------------------------------------------
	HANDLE				m_hWorkerIO;									//工作IOCP完成端口

protected:

	static	unsigned __stdcall ManagerProc(void* pThread);
	static	unsigned __stdcall WorkerProc(void* pThread);
	//定义保护函数---------------------------------------------------------------------------------
	

	//定义保护变量---------------------------------------------------------------------------------
	volatile WORD		m_wStaticThreadNum;								//静态工作线程数量(最少工作线程数)
	volatile WORD		m_wMaxThreadNum;								//动态工作线程数量(最多工作线程数)
	volatile bool		m_bQuitManager;									//管理线程强制退出标志
	DWORD				m_dwMSeconds;									//IOCP延时时间
	HANDLE				m_hManagerThread;								//管理线程句柄
	CRITICAL_SECTION	m_csLock;										//临界区变量

private:
	//定义私有函数---------------------------------------------------------------------------------
	CThreadPool::EThreadStatus	GetWorkThreadStatus();					//获得工作线程池的工作状态
	void	AddThread(void);											//增加工作线程
	void	DelThread(void);											//删除工作线程
	int		GetThreadbyID(DWORD dwID);									//获得工作线程的工作池序号

	
	//定义私有变量---------------------------------------------------------------------------------
	HANDLE				m_hManagerIO;									//管理IOCP完成端口
	LPPRE_THREAD_DATA	m_pThread_Data;									//线程池指针
	LPPER_IOCP_DATA		m_pIocp_Data;									//IOCP接收数据池指针
	LPPER_IOCP_DATA		m_pIocp_Send;									//IOCP发送数据池指针
	
	map<int,PER_IOCP_DATA>	v_mIOCP_Rece;								//容器：IOCP接收数据池
	CCriticalSection		v_cLock_Rece;								//同步锁：接收缓冲区

	DEALPROC_Close		m_pDealFunc_Close;								//回调函数：客户端关闭
	DEALPROC			m_pDealFunc;									//回调函数：数据接收

	float				v_fRun;											//线程工作数量
};
#endif

//-------------------------------------------------------------------------------------------------
//	结束
//-------------------------------------------------------------------------------------------------




















