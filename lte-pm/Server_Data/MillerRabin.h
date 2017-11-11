#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>

typedef __int64 lltype;
const int MAX_COUNT = 6;//测试次数
lltype allFactor[65], nFactor;//质因分解结果(刚返回的时候是无序的)
void fFindFactor(const lltype &num);//主函数(num > 1)

lltype pollard_rho(const lltype &c, const lltype &num);
bool miller_rabin(const lltype &n, int count);//miller-rabin
bool fWitNess(const lltype &a, const lltype &n, char t, const lltype &u);

lltype modular_exp(const lltype &a, lltype b, const lltype &n);//d = a^b(mod n)
lltype fMultiModular(const lltype &a, lltype b, const lltype &n);//(a*b)%num
lltype fGCD(lltype a, lltype b);


void fFindFactor(const lltype &num)
{
	if ( miller_rabin(num, MAX_COUNT) == true )
	{
		allFactor[++nFactor] = num;
		return;
	}
	lltype factor;
	do
	{
		factor = pollard_rho(rand()%(num-1) + 1, num);
	}while ( factor >= num );
	//printf("%I64d ",factor);
	fFindFactor(factor);
	fFindFactor(num/factor);
}

lltype pollard_rho(const lltype &c, const lltype &num)
{
	int i(1), k(2);
	lltype x = rand() % num;
	lltype y = x, comDiv;
	do
	{
		++i;
		if ( (x = fMultiModular(x, x, num) - c) < 0 )
			x += num;
		if ( x == y )
			break;
		comDiv = fGCD((y-x+num)%num, num);
		if ( comDiv > 1 && comDiv < num )
			return comDiv;
		if ( i == k )
		{
			y = x;
			k <<= 1;
		}
	}while ( true );
	return num;
}

//miller-rabin法测试素数, count 测试次数
bool miller_rabin(const lltype &n, int count) 
{
	if ( n == 1 )
		return false;
	if ( n == 2 )
		return true;
	lltype a, u;    //为了谨慎起见用了 lltype, 而没有用 int
	char t(0);      //n-1 == (2^t)*u
	for (u = n-1; !(u & 0x1); u>>=1)
		++t;
	for (int i = 1; i <= count; ++i)
	{
		a = rand() % (n-1) + 1;
		if ( fWitNess(a, n, t, u) )
			return false;
	}
	return true;
}

bool fWitNess(const lltype &a, const lltype &n, char t, const lltype &u)
{//if n must not be prime return true, else return false
	lltype x, y=0;
	x = modular_exp(a, u, n);
	while ( t-- )
	{
		y = fMultiModular(x, x, n);
		if ( y == 1 && x != 1 && x != n-1 )
			return true; //must not
		x = y;
	}
	return y != 1;
}

// d == a^b(mod n) 
lltype modular_exp(const lltype &a, lltype b, const lltype &n)
{
	lltype d(1), dTemp(a % n);//当前二进制位表示的是进制数值
	while ( b > 0 )
	{
		if ( b & 0x1 )
			d = fMultiModular(d, dTemp, n);//不直接 d*dTemp 是怕溢出
		dTemp = fMultiModular(dTemp, dTemp, n);
		b >>= 1;
	}
	return d;
}

//back = (a*b)%num      ( n <= 2^62)
lltype fMultiModular(const lltype &a, lltype b, const lltype &n)
{
	lltype back(0), temp(a % n);
	//b %= n;
	while ( b > 0 )
	{
		if ( b & 0x1 )
		{
			if ( (back = back + temp) > n )
				back -= n;
		}
		if ( (temp <<= 1) > n )
			temp -= n;
		b >>= 1;
	}
	return back;
}

lltype fGCD(lltype a, lltype b)
{
	lltype c;
	while ( b ) 
	{
		c = b;
		b = a%b;
		a = c;
	}
	return a;
}