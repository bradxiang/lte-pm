// Telnet.cpp: implementation of the CTelnet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Telnet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------			
//	�������ܣ����캯��
//------------------------------------------------------------------------------------------------------			
CTelnet::CTelnet(void)
{
	v_iIOCP_Num=0;														//
}
//------------------------------------------------------------------------------------------------------			
//	�������ܣ���������
//------------------------------------------------------------------------------------------------------			
CTelnet::~CTelnet(void)
{

}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�����TelnetManagerָ��
//------------------------------------------------------------------------------------------------------			
void CTelnet::Set_TelnetManager(LPVOID lparam)
{
	v_pTelnetManager=lparam;											//TelnetManagerָ��
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�Socket����
//------------------------------------------------------------------------------------------------------			
int CTelnet::Create()
{
	m_hSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_hSocket == INVALID_SOCKET)									//����������
		return -1;														//�쳣����
	return 0;															//��������
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�Socket����
//------------------------------------------------------------------------------------------------------			
int CTelnet::Connect(CString strIP,int nPort)
{
	unsigned long ip;

	if((ip = inet_addr(strIP.GetBuffer(0))) == INADDR_NONE)
		printf("invalid host ip given");

	m_sockaddr_in.sin_family = AF_INET;
	m_sockaddr_in.sin_port = htons(nPort);
	m_sockaddr_in.sin_addr = *(in_addr*)&ip;

	int nRet;
	nRet = connect(m_hSocket,(sockaddr*)&m_sockaddr_in,sizeof(sockaddr));		//Telnet����
	if ( nRet == SOCKET_ERROR )											//���������� 
		return -1;														//�쳣����
	return 0;															//��������
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�Telnet����
//------------------------------------------------------------------------------------------------------			
SOCKET CTelnet::TelnetConnect(BSCLoginInfo* v_cBSC)
{
// 	CTelnetManager* pDlg=(CTelnetManager*)v_pTelnetManager;
// 
// 	int v_iRet;
// 	
// 	v_iRet = Create();													//����Socket
// 	if ( v_iRet < 0 ) return NULL;										//���أ���Ч
// 
// 	v_iRet = Connect(v_cBSC->IP,v_cBSC->Port);							//���ɣ�����Telnet
// 	if ( v_iRet < 0 ) return NULL;										//���أ���Ч
// 
// 	v_iIOCP_Num=v_cBSC->v_iNum;											//IOCP�������
// 	v_cBSC->v_sSocket=m_hSocket;
// 
// 	//(1)��ɿͻ�SOCKET�빤��IOCP��ɶ˿ڰ󶨣�����1���ͻ�socket������2������IOCP�˿ڣ�
// 	if (CreateIoCompletionPort((HANDLE)m_hSocket,						//Socket
// 								pDlg->v_cIocp.m_hWorkerIO,				//����IOCP��ɶ˿�
// 								(DWORD)v_cBSC,							//CompletionKey [���KEY���������ʱ�����ظ�ֵ������ƥ���������]
// 								0										//NumberOfConcurrentThreads
// 								)==NULL)								//Ͷ��ʧ�ܣ�����
// 	{
// 		closesocket(m_hSocket);											//�رգ�Socket
// 		return NULL;													//���أ���Ч
// 	}
// 	pDlg->v_cIocp.RecvDataRequest(m_hSocket,v_cBSC->v_iNum);			//Ͷ�ݿͻ��˽�����Ϣ����

	return m_hSocket;													//���أ���ЧSocket
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ��ر�Telnet����
//------------------------------------------------------------------------------------------------------			
void	CTelnet::Close_Telnet()
{
 	if (m_hSocket!=NULL)
 		closesocket(m_hSocket);											//�رգ�Socket
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ����ؽ�����Ϣ
//  ���չ��̣�(1) ����ǰ���ȴ�ָ��ִ�н�����4000ms��
//			  (2) ��������
//------------------------------------------------------------------------------------------------------
CString	CTelnet::Rece()
{
// 	CTelnetManager* pDlg=(CTelnetManager*)v_pTelnetManager;
// 
 	CString		v_sText;												//��ʱ����
// 	int			v_iNum=0;
// 
// 	//�����ݴ���ָ��ִ����Ҫʱ�䡡[�ȴ����ݵ������2000ms]
// 	while (true)
// 	{
// 		if (!pDlg->v_sTelnet_Text[v_iIOCP_Num].IsEmpty())
// 			break;
// 
// 		v_iNum++;														//������++
// 		if (v_iNum<100)													//����100�ν���
// 		{
// 			Sleep(20);													///��ʱ
// 			continue;													//����
// 		}
// 		else															//ָ����������
// 			break;														//�˳���������ָ����Ϣ
// 	}
// 	
// 	//���ݻ�ȡ
// 	pDlg->v_cTelnet_Text_Lock.Lock();									//����
// 	v_sText=pDlg->v_sTelnet_Text[v_iIOCP_Num];							//��ã�Telnet��Ϣ
// 	pDlg->v_sTelnet_Text[v_iIOCP_Num]="";								//��λ��Telnet��Ϣ
// 	pDlg->v_cTelnet_Text_Lock.Unlock();									//����

	return v_sText;
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�����ָ��
//------------------------------------------------------------------------------------------------------
int CTelnet::Send(CString v_sSend,BSCLoginInfo* v_cBSC)
{
// 	CTelnetManager* pDlg=(CTelnetManager*)v_pTelnetManager;
// 
// 	pDlg->v_cIocp.SendDataRequest(m_hSocket,v_cBSC->v_iNum,v_sSend.GetBuffer(0),v_sSend.GetLength());

	return true;
}

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
