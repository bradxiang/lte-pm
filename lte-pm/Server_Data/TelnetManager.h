//======================================================================================================
//	���ܣ�Telnet������������Telent�Ĵ����������ӡ����͡����ա��رա�
//  ��Χ���澯�ɼ����澯������
//======================================================================================================
#pragma once
#include  <afxmt.h> 
#include "Server_Data.h"
#include "ThreadPool_IOCP.h"


class CTelnetManager
{
public:
	//------------------------------------------------------------------------------------------------------
	//	��������	
	//------------------------------------------------------------------------------------------------------
	CTelnetManager(void);
	CTelnetManager(LPVOID pdlg);
	~CTelnetManager(void);

	//�澯��ȡ��Telnet��
	u_char		AddCheckBits(u_char* sendData);							//����У�麯��

	BOOL		Start();												//����ϵͳ
	BOOL		ShutDowm();												//telnet�ر�
	
	//�߳����
	CWinThread* AlarmThread[1];											//�߳�����	
	HANDLE		hMutex[1] ;												//��Դ�ͷ�ͬ��
	static UINT My_Thread_Listen(LPVOID lparam);						//���� �����߳�
	BOOL		v_bThreadAlive;											//�̱߳�־�����ú����߳��˳�

	//------------------------------------------------------------------------------------------------------
	//	��������
	//------------------------------------------------------------------------------------------------------
	LPVOID	pTheDlg;													//����ָ��
	CCriticalSection TaskListLock;										//TaskList��ͬ���� 
	SOCKET	v_cSocket_Listen;											//Listen Socket
	SOCKET	v_cSocket_Connect;											//Connect Socket 

	//��ɶ˿ڱ���
	CThreadPool			v_cIocp;										//IOCP��ɶ˿� 

	//��Ϣ��ȡ
	CCriticalSection	v_cTelnet_Text_Lock;							//ͬ������Telent��Ϣ
	CString				v_sTelnet_Text;									//ָ��ִ�н��
	CCriticalSection	v_cTelnet_Lock;									//ͬ�������ͻ�����Ϣ

	//�ͻ�����Ϣ
	map<SOCKET,v_sClient_Connect> v_mClient_Connect;					//�ͻ�����Ϣ
};

//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------
