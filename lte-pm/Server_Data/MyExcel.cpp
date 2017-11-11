// MyExcel.cpp: implementation of the CMyExcel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comdef.h" 
#include "MyExcel.h"
#include "Server_Data.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// 构造函数
//-------------------------------------------------------------------------------------------------
CMyExcel::CMyExcel()													//构造函数
{

}

//-------------------------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------------------------
CMyExcel::~CMyExcel()													//析构函数
{

}

//-------------------------------------------------------------------------------------------------
// 导出Excel处理：写入CSV文件，再用Excel打开；
//-------------------------------------------------------------------------------------------------
void CMyExcel::Show(CListCtrl *v_cList) 
{
	Show_Name(v_cList,"",1);											//写入CSV，文件名称：默认；用Excel打开文件
}
//-------------------------------------------------------------------------------------------------
// 导出Excel处理：写入CSV文件；
// 参数：v_cList：列表窗指针
//		 v_sFile_Name：输出的文件名称
//		 v_bFile_Open：文件打开标志；[1：打开；0：不打开；]
//-------------------------------------------------------------------------------------------------
void CMyExcel::Show_Name(CListCtrl *v_cList,CString v_sFile_Name,bool v_bFile_Open) 
{
	int		v_iNum1,v_iNum2;											//临时变量
	int		v_iColumn,v_iRow;											//列表列数量、行数量
	CString	v_sStr,v_sCSV,v_sPath_Data,v_sCSV_File;						//临时变量
	CFile	v_fWriter;													//实例化
	CFileFind	finder;

	//目录处理
	v_sPath_Data=theApp.m_sPath+"\\DATA";								//文件目录
	if (!finder.FindFile(v_sPath_Data))									//存在？
	{
		if (!::CreateDirectory(v_sPath_Data, NULL))						//不存在：创建目录
		{
			MessageBox(NULL,"异常："+v_sPath_Data+" 目录创建失败。", "提示", MB_OK);		//失败
			return;
		}
	}

	try
	{
		//生成CSV文件：文件名称v_sCSV_File
		if (v_sFile_Name.IsEmpty())
			v_sCSV_File = theApp.m_sPath+"\\DATA\\Excel.csv";			//设置：CSV默认文件名称Excel.csv
		else
			v_sCSV_File = theApp.m_sPath+"\\DATA\\"+v_sFile_Name+".csv";//设置：CSV指定的文件名称
		if(v_fWriter.Open(v_sCSV_File, CFile::modeCreate | CFile::modeWrite))		//打开文件，如不存在则生成，如存在则清空文件；
		{
			v_iRow = v_cList->GetItemCount();							//取列表框行数
			CHeaderCtrl*  pHeaderCtrl = v_cList->GetHeaderCtrl();		//列表框表头
			v_iColumn=pHeaderCtrl->GetItemCount();						//取列表框列数

			HDITEM hdi;													//列结构
			TCHAR lpBuffer[256]; 
			hdi.mask = HDI_TEXT; 
			hdi.pszText = lpBuffer; 
			hdi.cchTextMax = 256; 	
			//表头处理：取列表的宽及列名称，逐列写入CSV
			for(v_iNum1=0;v_iNum1<v_iColumn;v_iNum1++)					//列
			{
				pHeaderCtrl->GetItem(v_iNum1,&hdi);						//取List列名称
				v_sStr=hdi.pszText;										//列名称
				v_sStr.Replace(" ","");									//剔除空格
				if(v_iNum1==0)
					v_sCSV=v_sStr;										//第一列
				else
					v_sCSV += "," + v_sStr;								//第二列、第三列、...、第n列
			}
			v_sCSV +="\r\n";											//回车换行
			v_fWriter.Write(v_sCSV,v_sCSV.GetLength());					//写入字符串

			//表内容处理：取列表数据，写入CSV
			for(v_iNum1=0;v_iNum1<v_iRow;v_iNum1++)						//行
			{
				for(v_iNum2=0;v_iNum2<v_iColumn;v_iNum2++)				//列
				{
					v_sStr=v_cList->GetItemText(v_iNum1,v_iNum2);		//取列表数据
					v_sStr.Replace(",",";");
					if(v_iNum2==0)
						v_sCSV=v_sStr;									//第一列
					else
						v_sCSV += "," + v_sStr;							//第二列、第三列、...、第n列
				}
				v_sCSV +="\r\n";										//回车换行
				v_fWriter.Write(v_sCSV,v_sCSV.GetLength());				//写入字符串
			}
			v_fWriter.Close();											//文件关闭

			//打开文件
			if (v_bFile_Open)
				ShellExecute(NULL,"open", v_sCSV_File, NULL, NULL, SW_SHOWNORMAL);		//打开文件：Excel.csv
		}
		else
			MessageBox(NULL,"请关闭已经打开的导出文件，再重新导出！！！", "提示", MB_OK);	//提示信息
	}	
	catch(_com_error e)
	{
		CString v_sError = (char *)e.Description();						//获得：出错信息
		MessageBox(NULL,"异常"+v_sError, "提示", MB_OK);				//提示信息
	}
}

//----------------------------------------------------------------------------------------------------
//	函数功能：导入CSV文件到list控件中
//	参数：CSV文件路径和list控件名，		v_bHead：文件带表头标记；
//----------------------------------------------------------------------------------------------------
bool CMyExcel::Get_CSV(CString v_sFile,CListCtrl *v_cList,bool v_bHead)
{
	CStdioFile	v_fRead;												//实例化	
	CString		v_sStr,v_sTemp;											//临时变量
	bool		v_Flag_Head=true;										//临时变量
	int			v_iLen,v_iNum,v_iItem;									//临时变量

	if (!v_fRead.Open(_T(v_sFile),CFile::modeRead))						//打开文件
	{
		MessageBox(NULL,"异常：文件打开失败！！！", "提示", MB_OK);		//提示信息
		return false;													//返回：失败
	}
	
	while(v_fRead.ReadString(v_sStr))									//数据读取：一行
	{
		if (v_bHead && v_Flag_Head)										//带表头？
		{
			v_Flag_Head=false;											//带表头：忽略掉
			continue;													//
		}
		//一行内容处理
		v_iNum=0;														//列号
		while (true)
		{
			if (v_sStr.IsEmpty())										//为空？？？
				break;													//退出
			v_iLen=v_sStr.Find(",",0);									//查找","
			if (v_iLen<0)												//找到？？？
			{
				v_sTemp=v_sStr;											//获取：有效信息
				v_sStr="";												//计算：下一个信息位置
			}
			else
			{
				v_sTemp=v_sStr.Left(v_iLen);							//获取：有效信息
				v_sStr=v_sStr.Mid(v_iLen+1);							//计算：下一个信息位置
			}
			if (v_iNum==0)
				v_iItem=v_cList->InsertItem(0xffff,v_sTemp);			//列：0
			else
				v_cList->SetItemText(v_iItem,v_iNum,(_bstr_t)v_sTemp);	//列：1、2、3...
			v_iNum++;													//列号：++
		}
	}
	v_fRead.Close();													//关闭文件
	return true;														//返回：成功
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
bool CMyExcel::do_events()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) > 0)
	{
		
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		GetMessage(&msg, NULL, 0, 0);
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// 结束
//-------------------------------------------------------------------------------------------------
