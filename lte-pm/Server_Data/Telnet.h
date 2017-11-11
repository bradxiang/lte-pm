// Telnet.h: interface for the CTelnet class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include  <afxmt.h> 


#if !defined(AFX_SOCKETDX_H__DFEE9DED_9AF1_11D2_A6CE_204C4F4F5020__INCLUDED_)
#define AFX_SOCKETDX_H__DFEE9DED_9AF1_11D2_A6CE_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTelnet  
{
public:
	CTelnet(void);
	~CTelnet(void);

	//------------------------------------------------------------------------------------------------------
	//	��������	
	//------------------------------------------------------------------------------------------------------
	int			Create();												//����Socket
	int			Connect(CString strIP,int nPort);						//���ӣ��豸
	void		Set_TelnetManager(LPVOID lparam);						//���ã�TelnetManagerָ��

	SOCKET		TelnetConnect(BSCLoginInfo* v_cBSC);					//���ӣ�Telnet
	void		Close_Telnet();											//�رգ�Telnet
	CString		Rece();													//���գ�Telnet
	int			Send(CString v_sSend,BSCLoginInfo* v_cBSC);				//���ͣ�Telnet

	//------------------------------------------------------------------------------------------------------
	//	��������
	//------------------------------------------------------------------------------------------------------
private:
	sockaddr_in			m_sockaddr_in;
	SOCKET				m_hSocket;

	LPVOID				v_pTelnetManager;								//TelnetManagerָ��

	int					v_iIOCP_Num;									//��ӦIOCP�����
};

//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------

#endif // !defined(AFX_SOCKETDX_H__DFEE9DED_9AF1_11D2_A6CE_204C4F4F5020__INCLUDED_)
