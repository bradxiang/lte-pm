
// Server_Data.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CServer_DataApp:
// �йش����ʵ�֣������ Server_Data.cpp
//

class CServer_DataApp : public CWinAppEx
{
public:
	CServer_DataApp();

// ��д
	DECLARE_MESSAGE_MAP()												//��Ϣӳ��	

	virtual BOOL InitInstance();										//��ʼ��

public:
	//------------------------------------------------------------------------------------------------------
	//	����
	//------------------------------------------------------------------------------------------------------
	void	InitMap(void);												//������Ϣ��ʼ��
	BOOL	DecrypteParam( CString key);								//������֤

	//------------------------------------------------------------------------------------------------------
	//	����
	//------------------------------------------------------------------------------------------------------
	//���ݿ����
	CString				a_sDatabase_Ip;									//���ݿ��������ַ
	CString				a_sDatabase_Name;								//���ݿ�����
	CString				a_sDatabase_User;								//���ݿ��û�
	CString				a_sDatabase_Password;							//���ݿ�����
	int					a_iDatabase_Type;								//���ݿ�����

	//ϵͳ����
	CString				a_sLocation[2];									//���õ�����Ϣ��  [0]�������ƣ�	[1]���б�ǣ�		�磺[0]="����"��	[1]="DL"��
	CTime				a_tDeadLine;									//�����Ч����
	int					a_iAlarmNotifyPort;								//�澯֪ͨ�˿�
	CString				a_sLogFile_Path;								//Log�ļ�·��
	CString             a_sPm_Path_;                                    //�����ļ�����Ŀ¼
	CString             a_save_directory_path_;                         //TXT�ļ�����Ŀ¼
	CString				a_net_disk_IP_;                                 //����IP
	CString				a_net_disk_drive_;								//�����̷�
	CString             a_net_disk_user_;                               //�����û���
	CString             a_net_disk_passwd_;								//��������
	CString             a_auto_start_;
	//�Զ������	
	CMyLogWriter		m_MyLog;										//д��־
	CString				m_sPath;										//Ĭ���ļ�·��
	CString				m_sAppName;										//Ĭ�ϣ�Ӧ�ó������� [��������һ����Ҫ]

	CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapProvince;				//������������Ϣ
};

extern CServer_DataApp theApp;

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
