#include "StdAfx.h"
#include "AlarmNotify.h"



CAlarmNotify::CAlarmNotify(void)
:m_bThreadAlive(FALSE)
{
}

CAlarmNotify::~CAlarmNotify(void)
{
}
//����
BOOL CAlarmNotify::Start()
{	
	m_socket= socket(AF_INET, SOCK_DGRAM, 0);
	if(-1==m_socket)
		return FALSE;
	SOCKADDR_IN		v_addr;
	v_addr.sin_family=AF_INET;
	v_addr.sin_addr.s_addr =htonl(INADDR_ANY);
	v_addr.sin_port=htons(ServerPort);
	
	int Ret=bind(m_socket,(SOCKADDR*)&v_addr,sizeof(v_addr));
	if(-1==Ret)
		return FALSE;

	//RecvFun();
	if(!m_bThreadAlive)
	{
		m_bThreadAlive=FALSE;
		m_thRecv=AfxBeginThread(_DataRecv,(LPVOID)this);
		//CreateThread(NULL,0,_DataRecv1,(LPVOID)this,0,NULL);
	}

	return TRUE;
}
//�����߳�
UINT CAlarmNotify::_DataRecv(LPVOID lparam)
{
	CAlarmNotify *pthis=(CAlarmNotify*)lparam;	
	ClientInfo NewClient;	
	pthis->ServerPort=3;
	pthis->m_ClientIfo.Add(NewClient);
	pthis->RecvFun();
	return 0;
}

void CAlarmNotify::RecvFun()
{
	char recvBuff[1024];
	int Ret=0;
	SOCKADDR_IN v_addFrom;
	int len=sizeof(SOCKADDR_IN);

	while (TRUE)
	{
		memset(&v_addFrom,0,sizeof(v_addFrom));
		memset(recvBuff,0,1024);
		//�趨��ʱ
		fd_set cli_Set;
		FD_ZERO(&cli_Set);  
		FD_SET(m_socket,&cli_Set);  
		timeval time_Out={500,0};
		int res=select(0,&cli_Set,NULL,NULL,&time_Out);
		if(res==0 || res==-1)
		{
			Sleep(1);			
		}
		//����
		else if(FD_ISSET(m_socket,&cli_Set))
		{
			Ret=recvfrom(m_socket,recvBuff,1024,0,(SOCKADDR*)&v_addFrom,&len);
			if(Ret<=0)
			{
				Sleep(50);
				continue;
			}
			if(Ret==SOCKET_ERROR)                                                 //�쳣����������
			{
				closesocket(m_socket);
				Start();
				continue;
			}
			
			ProcessFrame((u_char*)recvBuff,Ret,v_addFrom);
// 			sendto(m_socket,recvBuff,Ret,0,(SOCKADDR*)&v_addFrom,len);
// 			recvBuff[Ret]='\0';
// 			CString temp=recvBuff;
// 			AfxMessageBox(temp);
		}
	}//end of while
}
//���ݷ��ͺ���
void CAlarmNotify::MySendTo(char *sendData,SOCKADDR_IN addr) 
{
	int v_ilen=0;
	v_ilen=(u_char)sendData[2]*256+(u_char)sendData[3];
	if(v_ilen<6)
		return;
	SendLock.Lock();
	sendto(m_socket,sendData,v_ilen,0,(SOCKADDR*)&addr,sizeof(addr));
	SendLock.Unlock();
}
 //�����пͻ��˷�����Ϣ
BOOL CAlarmNotify::SendMessage(CString msg)
{
	int len=msg.GetLength();
	if(len>65000)
		return FALSE;
	char SendData[65535]={0x43,0x4d,0x00,0x00,0x82};
	char *pMsg=msg.GetBuffer();
	memcpy(SendData+5,pMsg,len);
	msg.ReleaseBuffer();
	SendData[2]=(len+6)/256;
	SendData[3]=(len+6)%256;
	AddCheckBits((u_char*)SendData);
	NotifyAllClient((u_char*)SendData);
	return TRUE;
}
#pragma region Frame Process

 //�Խ��յ���֡����
void CAlarmNotify:: ProcessFrame(u_char* recvdate,int len,SOCKADDR_IN addr)
{
	//�ж�У��
	if(!JudgeCheckBits(recvdate,len))
		return;
	//���崦��
	u_char v_chFunCode=recvdate[4];
	switch(v_chFunCode)
	{
	case 0x01:

		HearBeat(addr);
		break;
	case 0x10:
	case 0x11:
		ReTransmit(recvdate);
		break;
	default:
		break;
	}
	
}
//�ж�У���Ƿ���ȷ
BOOL CAlarmNotify::JudgeCheckBits(u_char* recvdate,int len)
{
	if(len<6)
		return FALSE;
	//�����ж�
	int v_ilen=0;
	v_ilen=(u_char)recvdate[2]*256+(u_char)recvdate[3];
	if(v_ilen>len)
		return FALSE;
	if(v_ilen!=len)
		len=v_ilen;
	u_char v_chSun=0x00,v_chCheckBits=recvdate[len-1];
	for (int i=0;i<len-1;i++)
		v_chSun+=recvdate[i];
	if(0xFF-v_chSun!=v_chCheckBits)
		return FALSE;
	return TRUE;
}
//���У��λ
BOOL CAlarmNotify::AddCheckBits(u_char* sendData)
{
	int v_ilen=0;
	v_ilen=(u_char)sendData[2]*256+(u_char)sendData[3];
	if(v_ilen<6)
		return FALSE;
	u_char v_chSun=0x00;
	for (int i=0;i<v_ilen-1;i++)
		v_chSun+=sendData[i];
	sendData[v_ilen-1]=0xFF-v_chSun;
	return TRUE;
}
//��������
void CAlarmNotify::HearBeat(SOCKADDR_IN addr)
{
	//�ж����޴˿ͻ���
	int i=0;
	for ( i=0;i<m_ClientIfo.GetCount();i++)
	{
		if(addr.sin_addr.s_addr==m_ClientIfo.GetAt(i).Addr.sin_addr.s_addr&&addr.sin_port==m_ClientIfo.GetAt(i).Addr.sin_port)
			break;
	}
	if(i>=m_ClientIfo.GetCount())                     //�޴˿ͻ��˼���
	{
		ClientInfo NewClient;
		NewClient.Addr=addr;
		NewClient.LastBeat=CTime::GetCurrentTime();
		m_ClientIfo.Add(NewClient);
	}
	
	//�ظ�
	u_char SendData[6]={0x43,0x4D,0x00,0x06,0x81};
	AddCheckBits(SendData);
	MySendTo((char*)SendData,addr);
}
//ת��������������
void CAlarmNotify::ReTransmit(u_char* recvData)
{
	NotifyAllClient(recvData);	
}
//���쳣���͸����еĿͻ��ˣ����жϿͻ����Ƿ�������
void CAlarmNotify::NotifyAllClient(u_char* sendData)
{
	CTime v_tCur=CTime::GetCurrentTime();
	CTimeSpan v_tSpan;
	ArrayLock.Lock();
	for (int i=0;i<m_ClientIfo.GetCount();i++)
	{
		v_tSpan=v_tCur-m_ClientIfo[i].LastBeat;
		if(v_tSpan.GetTotalMinutes()>15)                               //3��δ��������ɾ��
		{
			m_ClientIfo.RemoveAt(i);
			i--;
			continue;
		}
		MySendTo((char*)sendData,m_ClientIfo[i].Addr);
	}
	ArrayLock.Unlock();
}
#pragma endregion Frame Process