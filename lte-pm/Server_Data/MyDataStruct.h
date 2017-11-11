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


//����ͷ�ļ�
#include "MyADOConn.h"													//ADO��ȡ��
#include "MyExcel.h"													//CSV�ļ�������
#include "MyListCtrl.h"													//�б�������
#include "MyLogWriter.h"												//д��־��

//----------------------------------------------------------------------------------------------------------
//	�������壺ͨ������
//----------------------------------------------------------------------------------------------------------

//mString_Bool		[ͨ��������CString_Bool]
typedef	map<CString,bool> mString_Bool;									//map������mString_Bool

//mString_Int		[ͨ��������CString_Int]
typedef	map<CString,int> mString_Int;									//map������mString_Int

//mCString_String		[ͨ��������String_String]
typedef	map<CString,CString> mString_String;							//map������mString_String

//----------------------------------------------------------------------------------------------------------
//	�ṹ����
//----------------------------------------------------------------------------------------------------------

//�ṹ�壺BSC��Ԫ��½��Ϣ
struct BSCLoginInfo
{
	CString		IP;														// BSC��Ԫ��IP��ַ
	int			Port;													// �˿ں�
	CString		Account;												// ��¼�˺�
	CString		Password;												// ��¼����	
	int			v_iNum;													//�������
	int			v_sSocket;												//Socket
};

#include "Telnet.h"														//Telnetͨ����

//#define  THREADCOUNT          6
#define  COLLECTINTERVAL        60                  //���βɼ��������λΪs
#define  ALARMCOUNTMAX          300
#define  ALARMCOUNTMIN          200
#define  PASSWORDRSAD           32341
#define  PASSWORDRSAN           276995897
#define  KEYRSAD                16843
#define  KEYRSAN                453130003
#define  ANALYZECOUNT           10
#define  SAVELOGTODB            0
#define  SAVEERRORTODB          1

#define  TASK_UNPRO_MAX         5                  //���δ����������
#define  INFOGOT_UNPRO_MAX      200                //���δ������Ϣ��

#define  PRO_THREAD_MARK_MAX    5					//�����̱߳�Ǽ������ֵ
#define  DBTIMEOUTTIMESMAX      3                   //���ݿ����ʱ����


#define  WM_DEBUGINFO         WM_USER+100

#define GET_PROHIBIT_INTERVAL   3
#define GET_HALTED_INVERTAL     15					//��ȡCell Halt״̬������[��λ��min]
#define SYS_HALT_INTERVAL       2					//ϵͳ������ʱ��ͣվ����ļ��
#define SYS_HALT_JUDGE_INTERVAL 3					//ͣվ������ʱ�䣬��������λ����



//------------------------------------------------------------------------------------------------------			
//	�ṹ����
//------------------------------------------------------------------------------------------------------			
struct v_sClient_Connect												//�ͻ������ӳؽṹ
{
	SOCKET		v_iSocket;												//�ͻ���Socket
	CString		v_sIP;													//�ͻ���IP
	CString		v_sName;												//�ͻ�������
	int			v_iSum;													//�ͻ���������������Ĭ�ϣ�0��ÿ������++����ʱ��λ����=0����Ϊ���߿ͻ��ˡ�
	int			v_iTimer;												//��ʱ����������Ĭ�ϣ�0��ÿ�ζ�ʱ++��3�θ�λ����
	BYTE		v_cReceData[4000];										//SOCKET���ջ�����
	int			v_iLen;													//���ճ���
//	int			v_
};

struct v_sCommand														//�ͻ��˷���ָ��ṹ
{
	u_char		v_iCode;												//ָ�����
	CString		v_sText;												//ָ������
	SOCKET		v_iSocket;												//�ͻ���SOCKET
};

//����������
struct  Region_Struct
{  
	int			v_iPos;													//��λ��
	int			v_iSum_BTS;												//BTS����
	int			v_iSum_NodeB;											//NodeB����
	int			v_iSum_E_NodeB;											//E_NodeB����
};  
typedef map<CString,Region_Struct>	mRegion;							//����������

//��������ά
struct  Department_Struct
{  
	mString_Int	v_mRegion_BTS;											//��������ά--����_BTS----����
	mString_Int	v_mRegion_NodeB;										//��������ά--����_NodeB----����
	mString_Int	v_mRegion_E_NodeB;										//��������ά--����_E_NodeB----����
	int			v_iSum_BTS;												//BTS����
	int			v_iSum_NodeB;											//NodeB����
	int			v_iSum_E_NodeB;											//E_NodeB����
};  
typedef map<CString,Department_Struct>	mDepartment;					//��������ά

//������BTS
struct  BTS_Struct
{  
	CString v_sDate;													//ͣվʱ��
	CString v_sRegion;													//����
	CString v_sDepartment;												//��ά
	CString v_sBTS_ID;													//��վID 
	CString v_sStation;													//վַ����
	CString v_sStation_Remote;											//վַ����_Զ�� [��Դ��]
	mString_String	v_mCell;											//С��ID 
	int		v_iCell_Stop;												//�˷�С����
	int		v_iCell_Sum;												//С������
};  
typedef map<CString,BTS_Struct> mBTS;									//������BTS

//������վַ
struct  Station_Struct
{  
	mBTS	v_mBTS;														//������2G��վ
	mBTS	v_mNodeB;													//������3G��վ
	mBTS	v_mE_NodeB;													//������4G��վ
	int		v_iBTS_Stop;												//�˷�BTS����Cellȫ�˷���
	int		v_iBTS_Error;												//����BTS��������Cellȫ�˷���
	int		v_iBTS_Sum;													//BTS����
	CString	v_sRegion;													//����
};  
typedef map<CString,Station_Struct> mStation;							//������վַ

//������Ϣ�ṹ
struct Alarm_Type_GSM												//ʵʱ�澯��Ϣ�ṹ
{
	CString		v_sBSC;												//��Ԫ
	CString		v_sLevel;											//�澯����
	CString		v_sInfo;											//�澯��Ϣ
	CString		v_sCell;											//С��
	CString		v_sName;											//������
	CString		v_sDate;											//����ʱ��
	CString		v_sDetail;											//������Ϣ
	CString		v_sRegion;											//����
	CString		v_sAgency;											//��ά
};

//�������쳣С��
struct  CELL_Struct
{  
	CString v_sBTS_ID;													//��վID 
	CString v_sBTS_Name;												//��վName
	CString v_sRegion;													//����
	CString v_Department;												//��ά
	CString v_sDate;													//ͣվʱ��
};  
typedef map<CString,CELL_Struct> mCell;									//������Cell

//------------------------------------------------------------------------------------------------------
//	ASN/XML�ļ������ṹ����
//------------------------------------------------------------------------------------------------------
typedef map<CString,mString_String>			map_Child_Data;				//������map_Child_Data��Cell-Counter���
typedef map<CString,mString_String>			map_C_Data;					//������map_C_Data��Cell-Counter���
typedef map<CString,map_C_Data>				map_Child;					//������map_Child������������

//Counter��������
typedef map<CString,CString>		map_Counter;						//counter��map����
//Object�ṹ�嶨��
struct File_Object														//Object�ṹ��
{
	CString		v_sObject_ID;											//Object_ID
	CString		v_sObject_DN;											//Object_DN
	map_Counter	v_mCounter;												//COunter����
};
typedef	list<File_Object>			list_Object;						//Object����
//ObjType�ṹ�嶨��
typedef map<CString,list_Object>	map_ObjType;						//Objtype��map����
//Object�ṹ�嶨��
struct File_Object_Child												//Object_Child�ṹ��
{
	list_Object		v_lObject;											//Object����
	map_Child		v_mChild;											//Child����
};
//ObjType�ṹ�嶨��
typedef map<CString,File_Object_Child>		map_ObjType_Child;			//Objtype_Child��map����
//----------------------------------------------------------------------------------------------------------
//	End
//----------------------------------------------------------------------------------------------------------
