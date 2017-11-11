#include "stdafx.h"
#include "MyFilter.h"


CMyFilter::CMyFilter(void)
{
}


CMyFilter::~CMyFilter(void)
{
}

//------------------------------------------------------------------------------------------------------			
//	函数功能：过滤非大连小区
//------------------------------------------------------------------------------------------------------
void CMyFilter::FilterNonDalianCell(map_ObjType &cell_objtype)
{
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	list_Object::iterator	iterator_Object;							//count小区迭代器
	map<CString,map_Counter>::iterator iterator_cell;                   //小区查询迭代器
	iterator_objtype = cell_objtype.begin();
	while(iterator_objtype != cell_objtype.end())
	{
        iterator_Object = iterator_objtype->second.begin();
		while(iterator_Object != iterator_objtype->second.end())
		{
			iterator_Object->v_sObject_ID.Replace("（","(");
			iterator_Object->v_sObject_ID.Replace("）",")");
			iterator_cell = cell_information_.find(iterator_Object->v_sObject_ID);
			if (iterator_cell == cell_information_.end())
			{
				iterator_objtype->second.erase(iterator_Object ++);
			}
			else
			{
				iterator_Object++;
			}			
		}
		iterator_objtype ++;
	}
}
//------------------------------------------------------------------------------------------------------			
//	函数功能：判断工程状态小区
//------------------------------------------------------------------------------------------------------
bool CMyFilter::JudgeEngineeringCell(CString cell_name)
{
	map<CString,map_Counter>::iterator iterator_cell;                   //小区查询迭代器
	map<CString,CString>::iterator iterator_state;
	iterator_cell = cell_information_.find(cell_name);
	if (iterator_cell != cell_information_.end())
	{
	   iterator_state = iterator_cell->second.find("zOnline");
       if (iterator_state != iterator_cell->second.end())
       {
		   if (iterator_state->second == "在网")
		   {
			   return true;
		   } 
		   else
		   {
			   return false;
		   }
       } 
       else
       {
		   return false;	
       }
	}
	else
	{
		return false;		
	}			
}
//-----------------------------------------------------------------------------------------------------------------
//  功能描述:CON_LTE获取小区及其区域及在网状态
//-----------------------------------------------------------------------------------------------------------------
void  CMyFilter::GetCell()
{
	cell_information_.clear();
	_RecordsetPtr v_pRecordset;											  //记录集
	CMyADOConn v_cAdoConn;												  //数据库实例
	CString			sql,sTemp,Key,value;							      //临时变量
	int             cell_ID;
	map<CString,CString> map_cell;
	try
	{
		sql="SELECT zCell_ID,zRegion,zOnline FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		      //执行SELETE语句	
		while(!v_pRecordset->adoEOF)
		{
			cell_ID = v_cAdoConn.GetField_Int("zCell_ID");
			Key.Format("%d",cell_ID);
			sTemp=v_cAdoConn.GetField_String("zRegion");
			map_cell.insert(pair<CString,CString>("zRegion",sTemp));
			sTemp=v_cAdoConn.GetField_String("zOnline");
			map_cell.insert(pair<CString,CString>("zOnline",sTemp));
			cell_information_.insert(pair<CString,map_Counter>(Key,map_cell));
			map_cell.clear();
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										     //断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										  //关闭数据库

		CString v_sError = (char *)e.Description();						  //获得：出错信息
		theApp.m_MyLog.WriteText("异常："+v_sError+"\r\n");	              //log
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  功能描述类功能初始化
//-----------------------------------------------------------------------------------------------------------------
void  CMyFilter::Init()
{
	//获得大连小区及其区域在网信息
	GetCell();
}