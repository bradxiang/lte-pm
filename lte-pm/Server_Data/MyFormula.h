#pragma once
#include "string.h"
#include "math.h"
#define MAXOP 1000
#define NUMBER '0'
#define MAXVAL 100
#define BUFSIZE 1000
//******公式已修改无法做除法，切记切记//******公式已修改无法做除法，切记切记
class CMyFormula
{
public:
	//------------------------------------------------------------------------------------------------------
	//	函数定义
	//------------------------------------------------------------------------------------------------------
	CMyFormula(void);													//构造函数
	~CMyFormula(void);													//析构函数


	//自定义
	void	replacesign(char *os,char *ts);								//替换表达式中的特定变量
	float	simcalc(char *os);											//计算字符型表达式
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
	//	变量定义
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
