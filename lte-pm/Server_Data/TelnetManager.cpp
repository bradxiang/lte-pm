//======================================================================================================
//	���ܣ�Telnet������������Telent�Ĵ����������ӡ����͡����ա��رա�
//  ��Χ���澯�ɼ����澯������
//======================================================================================================
#include "StdAfx.h"
#include "TelnetManager.h"
#include "Server_DataDlg.h"

void CALLBACK My_IOCP_Task(unsigned long long* v_pClient, const char *v_pBuffer, int v_iLength);
void CALLBACK My_IOCP_Colse(unsigned long long* v_pClient);

//------------------------------------------------------------------------------------------------------			
//	�������ܣ����캯��
//------------------------------------------------------------------------------------------------------			
CTelnetManager::CTelnetManager(void)
{
}

CTelnetManager::CTelnetManager(LPVOID pdlg)
:pTheDlg(pdlg),
v_bThreadAlive(TRUE)
{
	
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ���������
//------------------------------------------------------------------------------------------------------			
CTelnetManager::~CTelnetManager(void)
{
	v_mClient_Connect.clear();											//�������
}

//-------------------------------------------------------------------------------------------------
//	���ܣ�	IOCP������ص����� [����IOCP���������ݣ��������ݻ�����]
//	������	(1)v_pClient�����Key�ṹ��	(2)v_pBuffer������������ָ�룻	(3)v_iLength���������ݳ��ȣ�
//	���̣�	��ȡ��Ϣ������ָ����У�
//-------------------------------------------------------------------------------------------------
void CALLBACK My_IOCP_Task(unsigned long long* v_pClient, const char *v_pBuffer, int v_iLength)
{
	v_sCommand			v_cCommand;										//ָ��ṹ
	u_char				v_chFunCode;									//��ʱ����
	int					v_iLen,v_iHead;									//��ʱ����
	CString				v_sTemp;										//��ʱ����
	bool				v_bOK;											//��ʱ����

	CServer_DataDlg* pDlg;
	pDlg=(CServer_DataDlg*)theApp.m_pMainWnd;							//��öԻ�������ָ��
	CTelnetManager* pthis=(CTelnetManager*)&pDlg->TelnetManager;		//TelnetManagerָ��

	v_sClient_Connect* v_pClient_Data=(v_sClient_Connect*)v_pClient;	//��ã�����ָ��

	if (v_iLength<=2000)												//��Ϣ��ͳ��ȣ�����
	{
		//���ҿͻ�����Ϣ
		map<SOCKET,v_sClient_Connect>::iterator v_pIter;					//������
		v_pIter=pthis->v_mClient_Connect.find(v_pClient_Data->v_iSocket);	//���ң�����
		if (v_pIter!=pthis->v_mClient_Connect.end())						//�ҵ�������[SOCKET]
		{
			//�����������ݵ��ͻ��˽��ջ�����
			memcpy(v_pIter->second.v_cReceData+v_pIter->second.v_iLen,v_pBuffer,v_iLength);		//���ݿ���
			v_pIter->second.v_iLen+=v_iLength;							//���ݳ���
			//�������ݴ���
			v_iHead=0;
			while(v_pIter->second.v_iLen != 0)							//ѭ������[���������п����ж�֡����]
			{
				v_bOK=false;											//��λ���˳����
				if(v_pIter->second.v_cReceData[v_iHead]!='C'|| v_pIter->second.v_cReceData[v_iHead+1]!='M')		//���ݰ�ͷ������
				{
					v_iHead++;											//��ͷλ��++
					if (v_iHead>=v_pIter->second.v_iLen-1)				//�ﵽ����β��������
						break;											//�˳�
					continue;											//����
				}

				//�������ݴ���[֡����]
				while(v_pIter->second.v_cReceData[v_iHead]=='C'&& v_pIter->second.v_cReceData[v_iHead+1]=='M')	//���ݰ�ͷ������
				{
					v_iLen=(u_char)v_pIter->second.v_cReceData[v_iHead+2]*256+(u_char)v_pIter->second.v_cReceData[v_iHead+3];		//���ݳ���
					if (v_iLen>1100)									//֡�����쳣��������
					{
						v_iHead++;										//��ͷλ��++
						if (v_iHead>=v_pIter->second.v_iLen-1)			//�ﵽ����β��������
							break;										//�˳�
						continue;										//����
					}
					if (v_iHead+v_iLen>v_pIter->second.v_iLen)			//���ݲ�ȫ������
					{
						v_bOK=true;										//��λ���˳����
						break;
					}
					u_char v_chJundgeCode=v_pIter->second.v_cReceData[v_iHead+v_iLen-1];							//��ã���������У����
					u_char v_chJundgeCodeLast=pthis->AddCheckBits((u_char*)v_pIter->second.v_cReceData+v_iHead);	//���㣺��������У����
					if(v_chJundgeCode!=v_chJundgeCodeLast)				//����У�� OK ������
					{
						v_iHead++;										//��ͷλ��++
						if (v_iHead>=v_pIter->second.v_iLen-1)			//�ﵽ����β��������
							break;										//�˳�
						continue;										//����
					}
					v_chFunCode=v_pIter->second.v_cReceData[v_iHead+4];	//�������ݹ�����
					if (v_chFunCode==0x01 || v_chFunCode==0x02 || v_chFunCode==0x03 || v_chFunCode==0x05 || v_chFunCode==0x04)	//��������¼��ָ�BSC�ɼ��쳣��ָ��ִ�н��
					{
						v_sTemp="";										//��������
						for(int vi=5;vi<v_iLen-1;vi++)					//����ѭ��
							v_sTemp+=(u_char)v_pIter->second.v_cReceData[v_iHead+vi];		//��ã����ݡ��ַ�����
						v_cCommand.v_iCode=v_chFunCode;					//ָ�������
						v_cCommand.v_sText=v_sTemp;						//ָ���Ϣ
						v_cCommand.v_iSocket=v_pClient_Data->v_iSocket;	//ָ��ͻ���SOCKET
						pDlg->v_cLock_Command.Lock();					//����
						if(v_chFunCode==03 || v_chFunCode==05)
							pDlg->v_lCommand.push_back(v_cCommand);		//���ӣ�ָ������
						else
							pDlg->v_lConnect.push_back(v_cCommand);		//���ӣ���������
						pDlg->v_cLock_Command.Unlock();					//����
					}
					v_iHead+=v_iLen;									//����λ�õ���
					break;												//�˳�
				}

				//���ݰ�Ǩ
				if (v_iHead!=0)											//ͷλ����Ҫ��Ǩ
				{
					if (v_pIter->second.v_iLen>v_iHead)					//ʣ��������Ч������
					{
						for(int vi=0;vi<v_pIter->second.v_iLen-v_iHead;vi++)							//����ѭ��
							v_pIter->second.v_cReceData[vi]=v_pIter->second.v_cReceData[vi+v_iHead];	//���ݰ�Ǩ
					}
					v_pIter->second.v_iLen-=v_iHead;					//���㣺���ݳ���
					v_iHead=0;											//ͷλ�õ���
				}
				if (v_bOK)												//�˳���ǣ�����[���ݲ�ȫ������һ��������֡]
					break;												//�˳�
			}
		}
	}
	pthis->v_cIocp.RecvDataRequest(v_pClient_Data->v_iSocket);			//Ͷ�ݿͻ��˽�����Ϣ����
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�����У�麯��
//------------------------------------------------------------------------------------------------------
u_char  CTelnetManager::AddCheckBits(u_char* sendData)
{
	int v_ilen=(u_char)sendData[2]*256+(u_char)sendData[3];				//���ݳ���
	if(v_ilen<6)														//��Ч���ݣ�����
		return FALSE;													//��Ч����
	u_char v_chSun=0x00;												//Ĭ��У���
	for (int i=0;i<v_ilen-1;i++)										//���ݱ���
		v_chSun+=sendData[i];											//���㣺У���
	return 0xFF-v_chSun;												//���أ�У���
}

//-------------------------------------------------------------------------------------------------
//	���ܣ�	IOCP�ͻ��˹رջص����� [�ͻ��˹رգ�����ͻ�����Ϣ]
//	������	(1)v_cSocket��
//-------------------------------------------------------------------------------------------------
void CALLBACK My_IOCP_Colse(unsigned long long* v_pClient)
{
	CString v_sTemp;													//��ʱ����

	CServer_DataDlg* pDlg;
	pDlg=(CServer_DataDlg*)theApp.m_pMainWnd;							//��öԻ�������ָ��
	CTelnetManager* pthis=(CTelnetManager*)&pDlg->TelnetManager;		//Telentָ��

	v_sClient_Connect* v_pClient_Data=(v_sClient_Connect*)v_pClient;	//��ã�����ָ��

	//�ͻ�����Ϣ����(1)�رտͻ�Socket��(2)�ӿͻ��б�����ͻ���ʾ��Ϣ��(3)�ӿͻ���������ͻ���Ϣ����
	pthis->v_cTelnet_Lock.Lock();										//����

	//(1)�رտͻ�Socket
	if (v_pClient_Data->v_iSocket != NULL)
		closesocket(v_pClient_Data->v_iSocket);							//�ر�SOCKET
	//(2)�ӿͻ��б�����ͻ���ʾ��Ϣ
	for (int vi=0;vi<pDlg->m_cList_Client.GetItemCount();vi++)			//�������ͻ��б�
	{
		if (v_pClient_Data->v_iSocket==atoi(pDlg->m_cList_Client.GetItemText(vi,3)))	//�ҵ������������ݣ��ͻ�SOCKET��
		{
			if (pDlg->m_cList_Client.GetItemText(vi,4)=="�ɼ�������")	//�ǲɼ�������������
			{
				pDlg->v_sServer_Socket=NULL;							//��λ���ɼ���SOCKET
				pDlg->m_cServer.EnableWindow(false);					//��Ч���ɼ���
			}
			pDlg->m_cList_Client.DeleteItem(vi);						//����ͻ��б���Ϣ
			break;														//�˳�
		}
	}
	//�б���ŵ���
	for (int vi=0;vi<pDlg->m_cList_Client.GetItemCount();vi++)			//�������ͻ��б�
	{
		v_sTemp.Format("%04d",vi+1);
		pDlg->m_cList_Client.SetItemText(vi,0," "+v_sTemp);				//���
	}
	//(3)�ӿͻ���������ͻ���Ϣ
	map<SOCKET,v_sClient_Connect>::iterator v_pIter;					//������
	v_pIter=pthis->v_mClient_Connect.find(v_pClient_Data->v_iSocket);	//���ң�����
	if (v_pIter!=pthis->v_mClient_Connect.end())						//�ҵ�������
		pthis->v_mClient_Connect.erase(v_pIter);						//ɾ���ͻ�����Ϣ

	pthis->v_cTelnet_Lock.Unlock();										//����
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ�����ϵͳ
//------------------------------------------------------------------------------------------------------			
BOOL CTelnetManager::Start()
{
	//�澯��ȡ
	//�������ݳ�ʼ��-------------------------------------------------------------------------------
	WSADATA wsaData;													//socket���ݻ���
	if (WSAStartup(MAKEWORD(2,1),&wsaData))								//����Windows Sockets DLL
	{ 
		theApp.m_MyLog.WriteText("�쳣�������ʼ��ʧ�ܣ�����\r\n");		//log
		return false;													//��Ч����
	}
	//IOCP��ʼ��-----------------------------------------------------------------------------------
	v_cIocp.Start(THREAD_MAX,THREAD_MAX,My_IOCP_Task,My_IOCP_Colse);	//IOCP��ʼ��(���٣���󣬻ص�����)
	v_sTelnet_Text="";													//��λ

	//���������߳�---------------------------------------------------------------------------------
	int			v_iRret;												//��ʱ����
	CString		v_sTemp;												//��ʱ����
	SOCKADDR_IN	sockStruct;												//��ʱ����

	v_cSocket_Listen=socket(PF_INET,SOCK_STREAM,0);						//ʹ��TCP/IP����
	if(v_cSocket_Listen == INVALID_SOCKET)
	{
		theApp.m_MyLog.WriteText("�쳣���޷��������� socket  ������\r\n");		//log
		return false;
	}
	sockStruct.sin_family=AF_INET;										//ʹ��TCP/IPЭ��
	sockStruct.sin_port = htons(theApp.a_iAlarmNotifyPort);				//�˿�
	sockStruct.sin_addr.S_un.S_addr = 0;								//Ĭ�ϵ�ַ
	v_iRret=bind(v_cSocket_Listen,(LPSOCKADDR)&sockStruct,sizeof(sockStruct));
	v_iRret=listen(v_cSocket_Listen,ACCEPT_MAX);						//socket ������ʼ
	if(v_iRret==SOCKET_ERROR)
	{
		closesocket(v_cSocket_Listen);									//���socket
		theApp.m_MyLog.WriteText("�쳣������ʧ��  ������\r\n");			//log
		return false;
	}
	hMutex[0]=CreateEvent(NULL, FALSE, FALSE, NULL);					//�����¼�
	AlarmThread[0]=AfxBeginThread(My_Thread_Listen,this);				//���ɣ��߳�

	return TRUE;
}

//-------------------------------------------------------------------------------------------------
//	���ܣ������������߳�
//	������lparam�������򴰿�ָ�룻
//	���ܣ���ָ���Ķ˿ڵȴ� Client �Ľ���,���ѽ����client����Ϣ����ͻ����ӳأ�Ͷ��IOCP;
//-------------------------------------------------------------------------------------------------
UINT CTelnetManager::My_Thread_Listen(LPVOID lparam)	
{	
	sockaddr_in v_cStru_Connect;										//socket��ַ�ṹ
	int			v_iLength;												//��ʱ����
	CString		v_sTemp,v_sIp;											//��ʱ����
	v_sClient_Connect	v_cClient_Connect;								//��ʱ����

	CTelnetManager *pthis=(CTelnetManager *)lparam;						//��ô���ָ��
	CServer_DataDlg* pDlg=(CServer_DataDlg*)pthis->pTheDlg;				//�Ի���ָ��
	v_iLength = sizeof(SOCKADDR);

	while(pthis->v_bThreadAlive)										//����ѭ����
	{
		//(1)�߳������ｫ��������ֱ���ͻ��˵�����������(v_cSocket_Connect���´����Ŀͻ���socket)
		pthis->v_cSocket_Connect=accept(pthis->v_cSocket_Listen,(LPSOCKADDR)&v_cStru_Connect,(LPINT)&v_iLength);
		if(pthis->v_cSocket_Connect == INVALID_SOCKET)					//���ӹ��ϣ��������socket
			break;
		//(2)�ж��Ƿ���ǿ���˳���־��pDlg->v_bClose
		if(!pthis->v_bThreadAlive)
		{
			closesocket(pthis->v_cSocket_Connect);						//�������socket
			break;
		}
		//(3)�õ��ͻ�����Ϣ����������־
		v_sIp=inet_ntoa(v_cStru_Connect.sin_addr);						//�õ��ͻ���IP��ַ
		//(4)�ѿͻ�����Ϣ����ͻ�����Ϣ�У�����ʾ��
		v_cClient_Connect.v_sIP=v_sIp;									//IP
		v_cClient_Connect.v_iSocket=pthis->v_cSocket_Connect;			//Socket
		v_cClient_Connect.v_sName="";									//����
		v_cClient_Connect.v_iSum=0;										//��������
		v_cClient_Connect.v_iTimer=0;									//��ʱ������
		v_cClient_Connect.v_iLen=0;										//�������ݳ���

		map<SOCKET,v_sClient_Connect>::iterator	v_pIter;				//������
		pthis->v_mClient_Connect.insert(pair<SOCKET,v_sClient_Connect>(pthis->v_cSocket_Connect,v_cClient_Connect));
		v_pIter=pthis->v_mClient_Connect.find(pthis->v_cSocket_Connect);//�ͻ�������ָ��

		pDlg->My_ShowList_Client(v_sIp,pthis->v_cSocket_Connect,v_cClient_Connect.v_sName);		//��ʾ���ͻ�����Ϣ
		//(5)��ɿͻ�SOCKET�빤��IOCP��ɶ˿ڰ󶨣�����1���ͻ�socket������2������IOCP�˿ڣ�����3���ͻ����ӳص�ַ�ṹ��
		if (CreateIoCompletionPort((HANDLE)pthis->v_cSocket_Connect,pthis->v_cIocp.m_hWorkerIO,
						(unsigned long long)&v_pIter->second,0)!=NULL)
		{
			//(6)����IOCP�˿�Ͷ�ݿͻ����ݽ�������
			pthis->v_cIocp.RecvDataRequest(pthis->v_cSocket_Connect);	//Ͷ�ݿͻ��˽�����Ϣ����
		}
	}
	//(7)�ر��̴߳���
	closesocket(pthis->v_cSocket_Listen);								//���socket
	SetEvent(pthis->hMutex[0]);	
	return 0;
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ��ر�Telnet ������Telnet�����̣߳���Ҫ֪ͨ�߳��˳���
//------------------------------------------------------------------------------------------------------			
BOOL CTelnetManager::ShutDowm()
{
	v_bThreadAlive=FALSE;												//�����˳���־
	if (v_cSocket_Listen !=NULL)										//SOCKET��Ч������
		closesocket(v_cSocket_Listen);									//�ر�SOCKET
	WaitForMultipleObjects(1,hMutex,TRUE,INFINITE);						//�ȴ��߳��˳�
	v_cIocp.Stop();														//IOCP�ر�
	return TRUE;
}

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
