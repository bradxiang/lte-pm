#ifndef OTA_THREADPOOL_H_CAESAR__DEF
#define OTA_THREADPOOL_H_CAESAR__DEF

#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

#define DATA_MAX			2000										//���ݻ������ֽ���(���ա�����)
#define	THREAD_MAX			8											//�����߳�����
#define ACCEPT_MAX			100											//�������ӵĿͻ�������

//-------------------------------------------------------------------------------------------------
//	���ܣ�	����ص�����
//	������	(1)v_pClient���ͻ������Key��
//			(2)v_pBuffer���������ݻ�������
//			(3)v_iLength���������ݳ��ȣ�
//-------------------------------------------------------------------------------------------------
typedef void (CALLBACK* DEALPROC)(unsigned long long* v_pClient, const char *v_pBuffer, int v_iLength);
typedef void (CALLBACK* DEALPROC_Close)(unsigned long long* v_pClient);

class CThreadPool
{
	friend	static	unsigned __stdcall ManagerProc(void* pThread);
	friend	static	unsigned __stdcall WorkerProc(void* pThread);

	//�������ݽṹ---------------------------------------------------------------------------------
	typedef struct PRE_THREAD_DATA										//�̳߳����ݽṹ
	{
		PRE_THREAD_DATA()
		{
			hThreadHandle	=	NULL;
			dwThreadID		=	0;
			bIsBusy			=	false;
			bIsQuit			=	false;
		}
		HANDLE			hThreadHandle;									//�߳̾��
		unsigned int	dwThreadID;										//�߳����
		volatile bool	bIsBusy;										//�̹߳���״̬��æ/����
		volatile bool	bIsQuit;										//�߳�ǿ���˳���־
	}*LPPRE_THREAD_DATA;

	typedef struct PER_IOCP_DATA										//IOCP���ݳ�,��I/O��������
	{
		OVERLAPPED	v_pOverlapped;										//�ص��ṹ
		WSABUF		v_dDataBuf;											//���ݻ���ṹ
		char		v_bBuffer[DATA_MAX];								//���ݻ�����
		int			v_iBufferLen;										//�������ݳ���
		int			v_iOperationType;									//�������ͣ�0�����еĻ�������1�����գ�
	}*LPPER_IOCP_DATA;

	enum	EReturnValue												//�̷߳���ֵ
	{
		MANAGERPROC_RETURN_VALUE = 10001,								//�����̷߳���ֵ
		WORKERPROC_RETURN_VALUE	= 10002									//�����̷߳���ֵ
	};
	enum	EThreadStatus												//�̹߳���״̬
	{
		BUSY,															//æ
		NORMAL,															//����
		IDLE															//����
	};

public:
	//ϵͳ��������---------------------------------------------------------------------------------

	CThreadPool();
	virtual ~CThreadPool();

	//���幫������---------------------------------------------------------------------------------
	bool	Start(WORD	wStatic,WORD wMax,DEALPROC lpDealFunc,DEALPROC_Close lpDealFunc_Close);		//����IOCP
	void	Stop();														//�ر�IOCP
	bool	RecvDataRequest(SOCKET client_s);							//Ͷ�ݽ�����������
	bool	SendDataRequest(SOCKET client_s,char *v_pBuffer,int v_iBuffer_Len);		//Ͷ�ݷ�����������
																		//Ͷ�ݷ�����������
	int		GetWorkThread();											//��ù����߳�����

	//���幫������---------------------------------------------------------------------------------
	HANDLE				m_hWorkerIO;									//����IOCP��ɶ˿�

protected:

	static	unsigned __stdcall ManagerProc(void* pThread);
	static	unsigned __stdcall WorkerProc(void* pThread);
	//���屣������---------------------------------------------------------------------------------
	

	//���屣������---------------------------------------------------------------------------------
	volatile WORD		m_wStaticThreadNum;								//��̬�����߳�����(���ٹ����߳���)
	volatile WORD		m_wMaxThreadNum;								//��̬�����߳�����(��๤���߳���)
	volatile bool		m_bQuitManager;									//�����߳�ǿ���˳���־
	DWORD				m_dwMSeconds;									//IOCP��ʱʱ��
	HANDLE				m_hManagerThread;								//�����߳̾��
	CRITICAL_SECTION	m_csLock;										//�ٽ�������

private:
	//����˽�к���---------------------------------------------------------------------------------
	CThreadPool::EThreadStatus	GetWorkThreadStatus();					//��ù����̳߳صĹ���״̬
	void	AddThread(void);											//���ӹ����߳�
	void	DelThread(void);											//ɾ�������߳�
	int		GetThreadbyID(DWORD dwID);									//��ù����̵߳Ĺ��������

	
	//����˽�б���---------------------------------------------------------------------------------
	HANDLE				m_hManagerIO;									//����IOCP��ɶ˿�
	LPPRE_THREAD_DATA	m_pThread_Data;									//�̳߳�ָ��
	LPPER_IOCP_DATA		m_pIocp_Data;									//IOCP�������ݳ�ָ��
	LPPER_IOCP_DATA		m_pIocp_Send;									//IOCP�������ݳ�ָ��
	
	map<int,PER_IOCP_DATA>	v_mIOCP_Rece;								//������IOCP�������ݳ�
	CCriticalSection		v_cLock_Rece;								//ͬ���������ջ�����

	DEALPROC_Close		m_pDealFunc_Close;								//�ص��������ͻ��˹ر�
	DEALPROC			m_pDealFunc;									//�ص����������ݽ���

	float				v_fRun;											//�̹߳�������
};
#endif

//-------------------------------------------------------------------------------------------------
//	����
//-------------------------------------------------------------------------------------------------




















