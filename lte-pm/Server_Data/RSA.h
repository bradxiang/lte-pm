#pragma once

#define long LONG64
#define random(min,max)(rand()%(max+1-min))//获得[min,max]的
#define  MAXLENGTH 1000                            //最大加密长度
#define  MAXN 3037000490                              //N最大值
#define  INTERlEN 8
#define  CODELEN 4


class CRSA
{
public:
	CRSA(void);
	~CRSA(void);
//public:
private:
	long m_lp;
	long m_lq;
	long m_ln;
	long m_ld;
	long m_le;


public:
	BOOL SetEncrypteParam(long e,long n);              //设定加密密钥
	CString EncryptStr(CString msg);                         //对一个字符串进行加密
	BOOL SetDecrypteParam(long d,long n);              //设定解密密钥
	CString DecryptStr(CString enmsg);                     //对一个字符串进行解密
	BOOL GetRandomParam();                                 //随机获得参数
	BOOL GetParByPQ(long p,long q);                     //根据给定的 p和q计算其他参数
	void GetAllParam(long &p,long &q,long &n,long &d,long &e);
	                                                                           //获取所有参数	
private:
	long powmod(long a, long b, long n);
	BOOL TestPrime(long n);
	long Encrypt(long msg);
	long Decrypt(long enmsg);
	long GetPrime(long N);
	long Invmod(long a, long b);
};
