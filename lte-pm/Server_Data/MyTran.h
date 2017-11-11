#pragma once

class CMyTran
{
public:
	CMyTran(void);
	~CMyTran(void);
	static CString		u_char2string(u_char Num);						//u_char ת�ɶ�Ӧ�ַ���
	static CString		u_chars2string(u_char Data[], int len);			//u_char ����ת�ɶ�Ӧ�ַ���
	static CString		int2string(int myvalue);
	static u_char		string2u_char(CString Num);
	static u_char		string2u_char_Harf(CString Num);
	static u_char		int2BCD(int Value);								//int��ת��ΪBCD��
	static COleDateTime StrToTime(CString v_sTime);						//�ַ�ʱ��תΪCTime
	static CString		GetDuration(CString v_sStart);					//��ǰʱ���������ʱ��ķ��Ӽ��
};
