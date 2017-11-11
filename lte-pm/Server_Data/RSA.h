#pragma once

#define long LONG64
#define random(min,max)(rand()%(max+1-min))//���[min,max]��
#define  MAXLENGTH 1000                            //�����ܳ���
#define  MAXN 3037000490                              //N���ֵ
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
	BOOL SetEncrypteParam(long e,long n);              //�趨������Կ
	CString EncryptStr(CString msg);                         //��һ���ַ������м���
	BOOL SetDecrypteParam(long d,long n);              //�趨������Կ
	CString DecryptStr(CString enmsg);                     //��һ���ַ������н���
	BOOL GetRandomParam();                                 //�����ò���
	BOOL GetParByPQ(long p,long q);                     //���ݸ����� p��q������������
	void GetAllParam(long &p,long &q,long &n,long &d,long &e);
	                                                                           //��ȡ���в���	
private:
	long powmod(long a, long b, long n);
	BOOL TestPrime(long n);
	long Encrypt(long msg);
	long Decrypt(long enmsg);
	long GetPrime(long N);
	long Invmod(long a, long b);
};
