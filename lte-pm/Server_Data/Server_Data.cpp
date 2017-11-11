
// Server_Data.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Server_Data.h"
#include "Server_DataDlg.h"
#include "RSA.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServer_DataApp

BEGIN_MESSAGE_MAP(CServer_DataApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServer_DataApp ����

CServer_DataApp::CServer_DataApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CServer_DataApp ����

CServer_DataApp theApp;


//------------------------------------------------------------------------------------------------------			
//	�������ܣ�CServer_DataApp ��ʼ��
//------------------------------------------------------------------------------------------------------
BOOL CServer_DataApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	//Ӧ�ó�����
	AfxEnableControlContainer();										//��ʼ��������

	::CoInitialize(NULL);												//��ʼ��com	

	// ��׼��ʼ�����δʹ����Щ���ܲ�ϣ����С���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ�����̸������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ����������޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	////////////////////////////////////////////////////////////////////////////////////////////////////			
	//����һ����Ӧ�ó����������ź��������ڣ�ֻ������һ�θ���� [�˳����ʱ����Ҫ�Ƴ����ź���]
	HANDLE hSem = CreateSemaphore(NULL,1,1,m_pszAppName);				//���ɣ��ź������  
	if   (GetLastError()   ==   ERROR_ALREADY_EXISTS)					//�ź����Ѵ��ڣ�   
	{   																//ʧ�ܣ����������ڣ����������һ��ʵ������   
		CloseHandle(hSem);   											//�رգ��ź������   
		//Ѱ����ǰ�Ѿ��򿪵Ĵ���   
		HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(),GW_CHILD);	//��ã�����
		while (::IsWindow(hWndPrevious))								//��Ч���壿
		{   
			if(::GetProp(hWndPrevious,m_pszAppName))					//�жϣ�[�Ӹ������ڵ������б��м������ݾ��]
			{															//�У�����Ѱ�ҵ�����   
				if (!::IsIconic(hWndPrevious))							//������û����С����
					ShowWindow(hWndPrevious,SW_SHOWMINIMIZED);		//��С����������
				ShowWindow(hWndPrevious,SW_SHOWMAXIMIZED);				//��󻯣�������
				return   false;   										//�˳���ʵ��   
			}   
			hWndPrevious = ::GetWindow(hWndPrevious,GW_HWNDNEXT); 		//��ã�����Ѱ����һ������
		}   
		AfxMessageBox("���������У�");									//ǰһʵ���Ѵ��ڣ����Ҳ��������������ܳ�����   
		return   false;													//�˳���ʵ��   
	}  

	////////////////////////////////////////////////////////////////////////////////////////////////////			
	//��ȡ����Ĭ��Ŀ¼
	const int nBufSize = 512;											//�ߴ磺������
	TCHAR chBuf[nBufSize];												//���壺������
	ZeroMemory(chBuf,nBufSize);											//��λ��������
	::GetModuleFileNameA(NULL,chBuf,nBufSize);							//��ȡĬ���ļ�·��
	TCHAR* lpStrPath = chBuf;											//ָ��
	PathRemoveFileSpec(lpStrPath);										//ȥ�����ļ�����
	m_sPath=chBuf;														//��ȡ��Ĭ��·��

	////////////////////////////////////////////////////////////////////////////////////////////////////			
	//��ȡ�����ļ���config.ini
	char v_cBuffer[512];
	GetPrivateProfileString("DATABASE_SERVER","DATABASE_SERVER",NULL,v_cBuffer,512,".\\config.ini");
	a_sDatabase_Ip.Format("%s",v_cBuffer);																//���ݿ��������ַ
	GetPrivateProfileString("DATABASE_NAME","DATABASE_NAME",NULL,v_cBuffer,512,".\\config.ini");
	a_sDatabase_Name.Format("%s",v_cBuffer);															//������ݿ�����
	GetPrivateProfileString("DATABASE_USER","DATABASE_USER",NULL,v_cBuffer,512,".\\config.ini");
	a_sDatabase_User.Format("%s",v_cBuffer);															//������ݿ��û�
	GetPrivateProfileString("DATABASE_PASSWORD","DATABASE_PASSWORD",NULL,v_cBuffer,512,".\\config.ini");
	a_sDatabase_Password.Format("%s",v_cBuffer);														//������ݿ�����
	a_iDatabase_Type=GetPrivateProfileInt("DATABASE_TYPE","DATABASE_TYPE",0,".\\config.ini");			//������ݿ�����
	GetPrivateProfileString("ALARMNOTIFY_PORT","ALARMNOTIFY_PORT",NULL,v_cBuffer,512,".\\config.ini");
	CString v_sPort="";
	v_sPort.Format("%s",v_cBuffer);
	a_iAlarmNotifyPort=atoi(v_sPort);									//UDP���澯֪ͨ�˿�
	GetPrivateProfileString("LICENCE_KEY","LICENCE_KEY",NULL,v_cBuffer,512,".\\config.ini");
	CString v_sKey="";
	v_sKey.Format("%s",v_cBuffer);
	GetPrivateProfileString("PM_PATH","PM_PATH",NULL,v_cBuffer,512,".\\config.ini");
	a_sPm_Path_.Format("%s",v_cBuffer);
	GetPrivateProfileString("SAVE_DIRECTORY","SAVE_DIRECTORY",NULL,v_cBuffer,512,".\\config.ini");
	a_save_directory_path_.Format("%s",v_cBuffer);
	GetPrivateProfileString("NET_DISK_IP","NET_DISK_IP",NULL,v_cBuffer,512,".\\config.ini");
	a_net_disk_IP_.Format("%s",v_cBuffer);
	GetPrivateProfileString("NET_DISK_DRIVE","NET_DISK_DRIVE",NULL,v_cBuffer,512,".\\config.ini");
	a_net_disk_drive_.Format("%s",v_cBuffer);
	GetPrivateProfileString("NET_DISK_USER","NET_DISK_USER",NULL,v_cBuffer,512,".\\config.ini");
	a_net_disk_user_.Format("%s",v_cBuffer);
	GetPrivateProfileString("NET_DISK_PASSWD","NET_DISK_PASSWD",NULL,v_cBuffer,512,".\\config.ini");
	a_net_disk_passwd_.Format("%s",v_cBuffer);
	GetPrivateProfileString("AUTO_START","AUTO_START",NULL,v_cBuffer,512,".\\config.ini");
	a_auto_start_.Format("%s",v_cBuffer);
	//�����ݿ������key����
	InitMap();															//������Ϣ��ʼ��
	if(!DecrypteParam(v_sKey.Trim()))									//��ã���������[���ݿ����롢�绰���š���Ч����]
		return FALSE;													//���أ���Ч

	//λ����Ϣ
	a_sLocation[0]="����";
	a_sLocation[1]="DL";

	if(a_sDatabase_Ip.IsEmpty())
	{
		AfxMessageBox("ϵͳ�����ļ�( config.ini )�����ڣ�����");										//�쳣�������ļ�û�ҵ�   
		return   false;																					//ϵͳ�˳�
	}

	//Log������ʼ
	theApp.m_MyLog.WriteText("ϵͳ��ʼ********************\r\n");		//Log

	m_sAppName=m_pszAppName;											//Ӧ�ó������� [��������һ����Ҫ]
	CServer_DataDlg dlg;												//������
	m_pMainWnd = &dlg;

	dlg.DoModal();														//�򿪣�������

	//Log�������˳�
	theApp.m_MyLog.WriteText("�����˳�--------------------\r\n");		//log

	return FALSE;
}

//------------------------------------------------------------------------------------------------------
//	���ܣ�������Ϣ��ʼ��
//------------------------------------------------------------------------------------------------------
void CServer_DataApp::InitMap(void)
{
	m_mapProvince.SetAt("0024","����;SY");
	m_mapProvince.SetAt("0410","����;TL");
	m_mapProvince.SetAt("0411","����;DL");								//����:���
	m_mapProvince.SetAt("0412","��ɽ;AS");
	m_mapProvince.SetAt("0413","��˳;FS");
	m_mapProvince.SetAt("0414","��Ϫ;BX");
	m_mapProvince.SetAt("0415","����;DD");
	m_mapProvince.SetAt("0416","����;JZ");
	m_mapProvince.SetAt("0417","Ӫ��;YK");
	m_mapProvince.SetAt("0418","����;FX");
	m_mapProvince.SetAt("0419","����;LY");	
	m_mapProvince.SetAt("0421","����;CY");
	m_mapProvince.SetAt("0427","�̽�;PJ");	
	m_mapProvince.SetAt("0429","��«��;HL");	
}

//------------------------------------------------------------------------------------------------------
//	���ܣ�������֤
//------------------------------------------------------------------------------------------------------
BOOL CServer_DataApp::DecrypteParam(CString key)
{
	CRSA Rsa;
	Rsa.SetDecrypteParam(PASSWORDRSAD,PASSWORDRSAN);
	a_sDatabase_Password=Rsa.DecryptStr(a_sDatabase_Password);			//���������ݿ�����

	Rsa.SetDecrypteParam(KEYRSAD,KEYRSAN);
	key=Rsa.DecryptStr(key);											//�������绰���š���Ч����
	if(key.GetLength()!=12)												
	{
		AfxMessageBox("Key ����");									//����
		return FALSE;													//���أ���Ч
	}
	//����Key�е���Ϣ���绰���š���Ч����
	CString v_sPostCode="",v_sDate="",v_sProInfo="",v_sTemp="";
	int v_iYear,v_iMonth,v_iDay;
	v_sPostCode=key.Left(4);											//�绰����
	key.Delete(0,4);
	v_sDate=key;														//��Ч����

	//ʱ��ת��--------------------------------------------------
	if(v_sDate.GetLength()<8)											//���ڳ���<8
	{
		AfxMessageBox("Key ����");									//����
		return FALSE;													//���أ���Ч
	}
	v_sTemp=v_sDate.Left(4);											//��
	v_sDate.Delete(0,4);
	v_iYear=atoi(v_sTemp);
	v_sTemp=v_sDate.Left(2);
	v_sDate.Delete(0,2);
	v_iMonth=atoi(v_sTemp);												//��
	if(v_iMonth<0||v_iMonth>12)
	{
		AfxMessageBox("Key ����");
		return FALSE;
	}
	v_iDay=atoi(v_sDate);												//��
	if(v_iDay<0||v_iDay>31)
	{
		AfxMessageBox("Key ����");
		return FALSE;
	}
	CTime v_tDeadLine(v_iYear,v_iMonth,v_iDay,0,0,0);					//���ã���Ч����
	a_tDeadLine=v_tDeadLine;

	//��ȡ����λ����Ϣ------------------------------------------
	m_mapProvince.Lookup(v_sPostCode,v_sProInfo);						//�������ң����ݣ��绰���� [�磺0411]
	if(v_sProInfo=="")													//û�ҵ�������		
	{
		AfxMessageBox("Key ����");									//���أ�����
		return FALSE;
	}
	int index=v_sProInfo.Find(";");	
	a_sLocation[0]=v_sProInfo.Left(index);								//���ƣ�[0]="����"
	a_sLocation[1]=v_sProInfo.Right(v_sProInfo.GetLength()-index-1);	//��ǣ�[1]="DL"
	return TRUE;														//���أ���Ч
}

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
