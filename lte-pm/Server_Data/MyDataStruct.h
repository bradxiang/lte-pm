#pragma once
#include <afxtempl.h>
#include <afxmt.h>
#include <map>
#include "afxwin.h"
using namespace std; 

#include <string>
#include <list>
#include <iostream>  
#include <fstream> 
#include <sstream>
#include <math.h>
#include <vector>
#include <queue>
#include <stack>
using std::stack;
#include <set>


//公共头文件
#include "MyADOConn.h"													//ADO读取类
#include "MyExcel.h"													//CSV文件导出类
#include "MyListCtrl.h"													//列表窗排序类
#include "MyLogWriter.h"												//写日志类

//----------------------------------------------------------------------------------------------------------
//	容器定义：通用容器
//----------------------------------------------------------------------------------------------------------

//mString_Bool		[通用容器：CString_Bool]
typedef	map<CString,bool> mString_Bool;									//map容器：mString_Bool

//mString_Int		[通用容器：CString_Int]
typedef	map<CString,int> mString_Int;									//map容器：mString_Int

//mCString_String		[通用容器：String_String]
typedef	map<CString,CString> mString_String;							//map容器：mString_String

//----------------------------------------------------------------------------------------------------------
//	结构定义
//----------------------------------------------------------------------------------------------------------

//结构体：BSC网元登陆信息
struct BSCLoginInfo
{
	CString		IP;														// BSC网元的IP地址
	int			Port;													// 端口号
	CString		Account;												// 登录账号
	CString		Password;												// 登录密码	
	int			v_iNum;													//工作序号
	int			v_sSocket;												//Socket
};

#include "Telnet.h"														//Telnet通信类

//#define  THREADCOUNT          6
#define  COLLECTINTERVAL        60                  //两次采集间隔，单位为s
#define  ALARMCOUNTMAX          300
#define  ALARMCOUNTMIN          200
#define  PASSWORDRSAD           32341
#define  PASSWORDRSAN           276995897
#define  KEYRSAD                16843
#define  KEYRSAN                453130003
#define  ANALYZECOUNT           10
#define  SAVELOGTODB            0
#define  SAVEERRORTODB          1

#define  TASK_UNPRO_MAX         5                  //最大未处理任务数
#define  INFOGOT_UNPRO_MAX      200                //最大未处理信息数

#define  PRO_THREAD_MARK_MAX    5					//处理线程标记计算最大值
#define  DBTIMEOUTTIMESMAX      3                   //数据库最大超时次数


#define  WM_DEBUGINFO         WM_USER+100

#define GET_PROHIBIT_INTERVAL   3
#define GET_HALTED_INVERTAL     15					//获取Cell Halt状态的周期[单位：min]
#define SYS_HALT_INTERVAL       2					//系统打死长时间停站任务的间隔
#define SYS_HALT_JUDGE_INTERVAL 3					//停站超过此时间，打死，单位分钟



//------------------------------------------------------------------------------------------------------			
//	结构定义
//------------------------------------------------------------------------------------------------------			
struct v_sClient_Connect												//客户端连接池结构
{
	SOCKET		v_iSocket;												//客户端Socket
	CString		v_sIP;													//客户端IP
	CString		v_sName;												//客户端姓名
	int			v_iSum;													//客户端心跳计数器【默认：0；每次心跳++；定时复位；若=0，则为掉线客户端】
	int			v_iTimer;												//定时器计数器【默认：0；每次定时++；3次复位；】
	BYTE		v_cReceData[4000];										//SOCKET接收缓冲区
	int			v_iLen;													//接收长度
//	int			v_
};

struct v_sCommand														//客户端发送指令结构
{
	u_char		v_iCode;												//指令功能码
	CString		v_sText;												//指令内容
	SOCKET		v_iSocket;												//客户端SOCKET
};

//容器：区域
struct  Region_Struct
{  
	int			v_iPos;													//列位置
	int			v_iSum_BTS;												//BTS总数
	int			v_iSum_NodeB;											//NodeB总数
	int			v_iSum_E_NodeB;											//E_NodeB总数
};  
typedef map<CString,Region_Struct>	mRegion;							//容器：区域

//容器：代维
struct  Department_Struct
{  
	mString_Int	v_mRegion_BTS;											//容器：代维--区域_BTS----数量
	mString_Int	v_mRegion_NodeB;										//容器：代维--区域_NodeB----数量
	mString_Int	v_mRegion_E_NodeB;										//容器：代维--区域_E_NodeB----数量
	int			v_iSum_BTS;												//BTS总数
	int			v_iSum_NodeB;											//NodeB总数
	int			v_iSum_E_NodeB;											//E_NodeB总数
};  
typedef map<CString,Department_Struct>	mDepartment;					//容器：代维

//容器：BTS
struct  BTS_Struct
{  
	CString v_sDate;													//停站时间
	CString v_sRegion;													//区域
	CString v_sDepartment;												//代维
	CString v_sBTS_ID;													//基站ID 
	CString v_sStation;													//站址名称
	CString v_sStation_Remote;											//站址名称_远端 [资源点]
	mString_String	v_mCell;											//小区ID 
	int		v_iCell_Stop;												//退服小区数
	int		v_iCell_Sum;												//小区总数
};  
typedef map<CString,BTS_Struct> mBTS;									//容器：BTS

//容器：站址
struct  Station_Struct
{  
	mBTS	v_mBTS;														//容器：2G基站
	mBTS	v_mNodeB;													//容器：3G基站
	mBTS	v_mE_NodeB;													//容器：4G基站
	int		v_iBTS_Stop;												//退服BTS数【Cell全退服】
	int		v_iBTS_Error;												//故障BTS数【部分Cell全退服】
	int		v_iBTS_Sum;													//BTS总数
	CString	v_sRegion;													//区域
};  
typedef map<CString,Station_Struct> mStation;							//容器：站址

//报文信息结构
struct Alarm_Type_GSM												//实时告警信息结构
{
	CString		v_sBSC;												//网元
	CString		v_sLevel;											//告警级别
	CString		v_sInfo;											//告警信息
	CString		v_sCell;											//小区
	CString		v_sName;											//中文名
	CString		v_sDate;											//发生时间
	CString		v_sDetail;											//报文信息
	CString		v_sRegion;											//区域
	CString		v_sAgency;											//代维
};

//容器：异常小区
struct  CELL_Struct
{  
	CString v_sBTS_ID;													//基站ID 
	CString v_sBTS_Name;												//基站Name
	CString v_sRegion;													//区域
	CString v_Department;												//代维
	CString v_sDate;													//停站时间
};  
typedef map<CString,CELL_Struct> mCell;									//容器：Cell

//------------------------------------------------------------------------------------------------------
//	ASN/XML文件变量结构定义
//------------------------------------------------------------------------------------------------------
typedef map<CString,mString_String>			map_Child_Data;				//容器：map_Child_Data【Cell-Counter子项】
typedef map<CString,mString_String>			map_C_Data;					//容器：map_C_Data【Cell-Counter子项】
typedef map<CString,map_C_Data>				map_Child;					//容器：map_Child【子项容器】

//Counter容器定义
typedef map<CString,CString>		map_Counter;						//counter的map容器
//Object结构体定义
struct File_Object														//Object结构体
{
	CString		v_sObject_ID;											//Object_ID
	CString		v_sObject_DN;											//Object_DN
	map_Counter	v_mCounter;												//COunter内容
};
typedef	list<File_Object>			list_Object;						//Object链表
//ObjType结构体定义
typedef map<CString,list_Object>	map_ObjType;						//Objtype的map容器
//Object结构体定义
struct File_Object_Child												//Object_Child结构体
{
	list_Object		v_lObject;											//Object内容
	map_Child		v_mChild;											//Child内容
};
//ObjType结构体定义
typedef map<CString,File_Object_Child>		map_ObjType_Child;			//Objtype_Child的map容器
//----------------------------------------------------------------------------------------------------------
//	End
//----------------------------------------------------------------------------------------------------------
