#pragma once
#include <afxtempl.h>
#include   <afxmt.h>

struct ClientInfo
{
	SOCKADDR_IN Addr;					       //�ͻ��˵�ַ
	CTime LastBeat;                                     //�ϴ�����ʱ��
};

class CAlarmNotify
{
public:
	CAlarmNotify(void);
	~CAlarmNotify(void);
public:
	int ServerPort;
private:		
	SOCKET m_socket;
	CWinThread * m_thRecv;                        //�����߳�
	BOOL m_bThreadAlive;                          //�߳��Ƿ���
	CArray<ClientInfo,ClientInfo> m_ClientIfo;//��½�Ŀͻ�����Ϣ
	CCriticalSection ArrayLock;                      //�ͻ���Ϣ������
	CCriticalSection SendLock;                      //����������ֹ�����߳�ͬʱ���͵��³�ͻ

public:
	BOOL Start();                                          //����	
	BOOL SendMessage(CString msg);         //�����пͻ��˷�����Ϣ
private:
	static UINT _DataRecv(LPVOID lparam);//�����߳�
	void  RecvFun();                                     //������պ���
	void ProcessFrame(u_char* recvdate,int len,SOCKADDR_IN addr);      //�Խ��յ���֡����	
	BOOL JudgeCheckBits(u_char* recvdate,int len);//�ж�У���Ƿ���ȷ
	BOOL AddCheckBits(u_char* sendData);//���У��λ
	void HearBeat(SOCKADDR_IN addr);    //��������
	void MySendTo(char *sendData,SOCKADDR_IN addr);                                //���ݷ��ͺ���
	void ReTransmit(u_char* recvData);        //ת��������������
	void NotifyAllClient(u_char* sendData);     //���쳣���͸����еĿͻ��ˣ����жϿͻ����Ƿ�������

	//DWORD WINAPI _DataRecv1(LPVOID lparam);

};
