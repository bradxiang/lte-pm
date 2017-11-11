#pragma once
#include <afxtempl.h>
#include   <afxmt.h>

struct ClientInfo
{
	SOCKADDR_IN Addr;					       //客户端地址
	CTime LastBeat;                                     //上次心跳时间
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
	CWinThread * m_thRecv;                        //接收线程
	BOOL m_bThreadAlive;                          //线程是否开启
	CArray<ClientInfo,ClientInfo> m_ClientIfo;//登陆的客户端信息
	CCriticalSection ArrayLock;                      //客户信息数组锁
	CCriticalSection SendLock;                      //发送锁，防止两个线程同时发送导致冲突

public:
	BOOL Start();                                          //开启	
	BOOL SendMessage(CString msg);         //向所有客户端发送信息
private:
	static UINT _DataRecv(LPVOID lparam);//接收线程
	void  RecvFun();                                     //具体接收函数
	void ProcessFrame(u_char* recvdate,int len,SOCKADDR_IN addr);      //对接收到的帧处理	
	BOOL JudgeCheckBits(u_char* recvdate,int len);//判断校验是否正确
	BOOL AddCheckBits(u_char* sendData);//添加校验位
	void HearBeat(SOCKADDR_IN addr);    //心跳处理
	void MySendTo(char *sendData,SOCKADDR_IN addr);                                //数据发送函数
	void ReTransmit(u_char* recvData);        //转发短信申告的内容
	void NotifyAllClient(u_char* sendData);     //把异常发送个所有的客户端，并判断客户端是否仍在线

	//DWORD WINAPI _DataRecv1(LPVOID lparam);

};
