#pragma once

class CMyTran
{
public:
	CMyTran(void);
	~CMyTran(void);
	static CString		u_char2string(u_char Num);						//u_char 转成对应字符串
	static CString		u_chars2string(u_char Data[], int len);			//u_char 数组转成对应字符串
	static CString		int2string(int myvalue);
	static u_char		string2u_char(CString Num);
	static u_char		string2u_char_Harf(CString Num);
	static u_char		int2BCD(int Value);								//int型转换为BCD码
	static COleDateTime StrToTime(CString v_sTime);						//字符时间转为CTime
	static CString		GetDuration(CString v_sStart);					//当前时间与给定的时间的分钟间隔
};
