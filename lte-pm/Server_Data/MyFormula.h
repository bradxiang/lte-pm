#pragma once
#include "string.h"
#include "math.h"
#define MAXOP 1000
#define NUMBER '0'
#define MAXVAL 100
#define BUFSIZE 1000
//******��ʽ���޸��޷����������м��м�//******��ʽ���޸��޷����������м��м�
class CMyFormula
{
public:
	//------------------------------------------------------------------------------------------------------
	//	��������
	//------------------------------------------------------------------------------------------------------
	CMyFormula(void);													//���캯��
	~CMyFormula(void);													//��������


	//�Զ���
	void	replacesign(char *os,char *ts);								//�滻���ʽ�е��ض�����
	float	simcalc(char *os);											//�����ַ��ͱ��ʽ
	float	calc(int sign, float op1, float op2);
	int		cmp(int s1, int s2);
	int	push_d(float f);
	float	pop_d(void);
	void	push_s(int c);
	int		pop_s(void);
	int		getop(char *os,char *s);
	int		getch(char *os);
	void	ungetch(int c);
	int		isdigit( int ch );

	//------------------------------------------------------------------------------------------------------
	//	��������
	//------------------------------------------------------------------------------------------------------
	int		sp_d;
	float	val_d[MAXVAL];

	/* a stack for sign */
	int		sp_s;
	int		val_s[MAXVAL];


	char	buf[BUFSIZE];
	int		bufp;
	int		j;
};

//------------------------------------------------------------------------------------------------------			
//	END
//------------------------------------------------------------------------------------------------------
