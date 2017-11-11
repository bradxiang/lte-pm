#include "StdAfx.h"
#include "MillerRabin.h"
#include "RSA.h"
#include "MyTran.h"


CRSA::CRSA(void)
{
	m_lp=0;m_lq=0;
	m_ln=0;m_ld=0;m_le;
}

CRSA::~CRSA(void)
{
}


//Miller-Rabin素性检验算法
BOOL CRSA::TestPrime(long n)
{
	if(miller_rabin(n,6))
		return TRUE;
	else
		return FALSE;
// 	int T=10;
// 	long i,j,s,t,a,y;
// 	if(n%2==0) return 0;
// 	s=0;t=n-1;
// 	while(t%2==0)
// 	{
// 		t=t/2;
// 		s++;
// 	}
// 	for(i=0;i<T;i++)//T为安全参数
// 	{
// 		a=random(2,n-2);//获得[2,n-2]随机数作为基
// 		y=powmod(a,t,n);
// 		if(y!=1&&y!=n-1)
// 		{
// 			j=1;
// 			while(j<=s&&y!=n-1)
// 			{
// 				y=(y*y)%n;
// 				if(y==1) return false;
// 				j++;
// 			}
// 			if(y!=n-1) return false;
// 		}
// 	}
// 	return true;
}
//模重复平方法求模幂
long CRSA::powmod(long a, long b, long n)
{
	long A=1;
	while(b!=0)
	{
		if(b%2) A=(A*a)%n;
		a=(a*a)%n;
		b=b>>1;
	}	
	return A;	
}
//加密函数
long CRSA::Encrypt(long msg)
{
	return powmod(msg,m_le,m_ln);
}
//解密一个数字
long CRSA:: Decrypt(long enmsg)
{
	return powmod(enmsg,m_ld,m_ln);
}
 //设定加密密钥
BOOL CRSA::SetEncrypteParam(long e,long n)
{
	if(n>MAXN)
	{
		AfxMessageBox("N过大，超出计算范围");
		return FALSE;
	}
	m_le=e;
	m_ln=n;
	return TRUE;
}
//加密一个字符串
CString CRSA:: EncryptStr(CString msg)
{
	char buff[MAXLENGTH];
	long enbuff[MAXLENGTH];
	strcpy(buff,(LPCSTR)msg);
	for(int i=0;i<msg.GetLength();i++)
		enbuff[i]=Encrypt((long)buff[i]);
	//把long按字节转化为string
	u_char longbuff[INTERlEN];
	CString v_sEnstr="",v_stemp;
	for(int i=0;i<msg.GetLength();i++)
	{
		memcpy(longbuff,&enbuff[i],INTERlEN);
		for (int j=0;j<CODELEN;j++)
		{
			v_stemp.Format("%X",longbuff[j]);
			if(v_stemp.GetLength()<2)
				v_stemp="0"+v_stemp;
			v_sEnstr+=v_stemp;
		}
	}
	return v_sEnstr;
}
 //设定解密密钥
BOOL CRSA:: SetDecrypteParam(long d,long n)
{
	if(n>MAXN)
	{
		AfxMessageBox("N过大，超出计算能力");
		return FALSE;
	}
	m_ld=d;
	m_ln=n;
	return TRUE;
}
//解密一个字符串
CString CRSA:: DecryptStr(CString enmsg)
{
	enmsg.Trim();
	if(enmsg.GetLength()%CODELEN!=0)
	{
		AfxMessageBox("密文不完整");
		return "";
	}
	if(enmsg.GetLength()>1000*CODELEN)
	{
		AfxMessageBox("密文超过最大长度");
		return "";
	}
	//转化为long型数组
	int v_iCharCount=enmsg.GetLength()/CODELEN;               //获取明文长度
	u_char buff[INTERlEN];                                                     //一个long型的内存空间
	long longbuff[MAXLENGTH];
	CString temp;
	CMyTran Mytran;
	for (int i=0;i<v_iCharCount;i++)
	{
		memset(buff,0,INTERlEN);
		for (int j=0;j<CODELEN;j++)
		{
			temp=enmsg.Left(2);
			enmsg.Delete(0,2);
			buff[j]=Mytran.string2u_char(temp);
		}
		memcpy(&longbuff[i],buff,INTERlEN);                              //把4个字节转化为一个long型	
	}
	//解密
	CString v_sDeStr="";
	char v_chDeBuff[MAXLENGTH+1];
	long v_lDecTemp;
	for (int i=0;i<v_iCharCount;i++)
	{
		v_lDecTemp=Decrypt(longbuff[i]);
		v_chDeBuff[i]=(u_char)v_lDecTemp;
	}
	v_chDeBuff[v_iCharCount]='\0';
	v_sDeStr=v_chDeBuff;
	return v_sDeStr;
}
//获取随机的参数
BOOL CRSA:: GetRandomParam()
{
	long temp;
	srand((unsigned)time(NULL));//初始化随机数生成器
	m_lp=GetPrime(2147483647);
	m_lq=GetPrime(2147483647);
	m_ln=m_lp*m_lq;
	if(m_ln>MAXN)
	{
		AfxMessageBox("N过大,请重新计算");
		return FALSE;
	}
	temp=(m_lp-1)*(m_lq-1);
	m_ld=GetPrime(temp);
	m_le=Invmod(m_ld,temp);
	return true;
}
//获取所有参数
void CRSA::GetAllParam(long &p,long &q,long &n,long &d,long &e)
{
	p=m_lp;q=m_lq;n=m_ln;d=m_ld;e=m_le;
}
//根据pq获得个参数
BOOL CRSA:: GetParByPQ(long p,long q)
{
	if(!TestPrime(p))
	{
		AfxMessageBox("P 为非素数");
		return FALSE;
	}
	if(!TestPrime(q))
	{
		AfxMessageBox("Q 为非素数");
		return FALSE;
	}
	long temp;
	srand((unsigned)time(NULL));//初始化随机数生成器
	m_lp=p;
	m_lq=q;
	m_ln=m_lp*m_lq;
	if(m_ln>MAXN)
	{
		AfxMessageBox("N过大,请重新选择p和q的值");
		return FALSE;
	}
	temp=(m_lp-1)*(m_lq-1);
	m_ld=GetPrime(temp);
	m_le=Invmod(m_ld,temp);
	return TRUE;
}
//调用素性检验算法获得不超过N的一个随机数
long CRSA:: GetPrime(long N)
{
	long n;
	for(;;)
	{
		n=random(3,N);
		if(TestPrime(n)&&n>9999) return n;
		//Sleep(50);
	}
}
//扩展的欧几里德算法求模逆元
long CRSA::Invmod(long a, long b)
{
	long s0=1,s1=0,s2,q,t,b0;
	b0=b;
	while(b)
	{
		q=a/b;
		s2=s0-q*s1;
		if(s2>=0)
			s2=s2%b0;
		else
			s2=b0-(-s2)%b0;
		s0=s1;s1=s2;
		t=b;b=a%b;a=t;
	}
	if(a==1) return s0;
	else return 0;
}