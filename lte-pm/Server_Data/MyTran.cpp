#include "StdAfx.h"
#include "MyTran.h"

CMyTran::CMyTran(void)
{
}

CMyTran::~CMyTran(void)
{
}

CString CMyTran::u_char2string(u_char Num)
{
	u_char temp;
	CString v_sResult,v_sTemp;
	v_sResult="";
	int v_iHarfByte;
	for (int i=0;i<2;i++)
	{
		temp=0x00;
		if (i==0)
		{
			temp=Num&0xF0;
			temp=temp>>4;
			v_iHarfByte=(int)temp;			
		}
		else
		{
			temp=Num&0x0F;
			v_iHarfByte=(int)temp;
		}

		if (v_iHarfByte>=0&&v_iHarfByte<=9)
		{
			v_sTemp.Format("%d",v_iHarfByte);
		}
		if (v_iHarfByte==10)
		{
			v_sTemp="A";
		}
		else if (v_iHarfByte==11)
		{
			v_sTemp="B";
		}
		else if (v_iHarfByte==12)
		{
			v_sTemp="C";
		}
		else if (v_iHarfByte==13)
		{
			v_sTemp="D";
		}
		else if (v_iHarfByte==14)
		{
			v_sTemp="E";
		}
		else if (v_iHarfByte==15)
		{
			v_sTemp="F";
		}
		v_sResult+=v_sTemp;
	}
	return v_sResult;
}
CString CMyTran::u_chars2string(u_char Data[], int len)
{	
	CString v_temp;	
	for (int i=0;i<len;i++)
	{
		v_temp+=u_char2string(Data[i]);
		v_temp+="  ";
	}
	return v_temp;
}



CString CMyTran::int2string(int myvalue)
{
	CString v_sResult;
	v_sResult.Format("%d",myvalue);
	return v_sResult;
}

u_char CMyTran::string2u_char(CString Num)
{
	//CString temp;
	u_char v_chResult;
	//temp=Num.Left(1);
	v_chResult=string2u_char_Harf(Num.Left(1))<<4;
	v_chResult+=string2u_char_Harf(Num.Right(1));
	return v_chResult;
}
u_char CMyTran::string2u_char_Harf(CString Num)
{
	char temp[2],v_chResult=0x00;
	strcpy( (char*)temp,(LPCSTR)Num);	
	if (0x30<=temp[0]&&temp[0]<=0x39)
	{
		v_chResult=(u_char)atoi(Num);
	}
	else if (0x41<=temp[0]&&temp[0]<=0x46)
	{
		v_chResult=(u_char)temp[0]-0x37;
	}
	else if (0x61<=temp[0]&&temp[0]<=0x66)
	{
		v_chResult=(u_char)temp[0]-0x57;
	}
	return v_chResult;
}

//输入一个int型，输出ＢＣＤ码
u_char CMyTran::int2BCD(int Value)
{
	u_char Result;
	Result=((u_char)(Value/10)<<4)+((u_char)(Value%10));	
	return Result;
}

CString CMyTran::GetDuration(CString v_sStart)
{

	CString v_sDuration  ;
	CTime v_tTime1,v_tTime2;
	v_tTime1 = StrToTime(v_sStart);
	v_tTime2 =CTime::GetCurrentTime();

	CTimeSpan v_t = v_tTime2-v_tTime1;
	long v_lDuration = (long)v_t.GetTotalMinutes();
	v_sDuration.Format("%d",v_lDuration);

	return v_sDuration;
}

// 将一个字符型时间转换为CTime类型，格式2012-04-10 06:51:00
COleDateTime CMyTran::StrToTime(CString v_sTime)
{
	int v_iYear,v_iMouth,v_iDay,v_iHour,v_iMinute ,v_iSecond;
	CString v_sTemp ;
	v_sTime.Trim();
	v_sTemp		= v_sTime.Left(4);
	v_iYear		= atoi(v_sTemp);
	v_sTemp		= v_sTime.Mid(5,2);
	v_iMouth	= atoi(v_sTemp);
	v_sTemp		= v_sTime.Mid(8,2);
	v_iDay		= atoi(v_sTemp);
	v_sTemp		= v_sTime.Mid(11,2);
	v_iHour		= atoi(v_sTemp);
	v_sTemp		= v_sTime.Mid(14,2);
	v_iMinute	= atoi(v_sTemp);
	v_sTemp		= v_sTime.Mid(17,2);
	v_iSecond	= atoi(v_sTemp);
	COleDateTime v_tTime(v_iYear, v_iMouth, v_iDay, v_iHour, v_iMinute, v_iSecond);
	return v_tTime;
}