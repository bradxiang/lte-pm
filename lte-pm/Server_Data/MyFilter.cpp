#include "stdafx.h"
#include "MyFilter.h"


CMyFilter::CMyFilter(void)
{
}


CMyFilter::~CMyFilter(void)
{
}

//------------------------------------------------------------------------------------------------------			
//	�������ܣ����˷Ǵ���С��
//------------------------------------------------------------------------------------------------------
void CMyFilter::FilterNonDalianCell(map_ObjType &cell_objtype)
{
	map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
	list_Object::iterator	iterator_Object;							//countС��������
	map<CString,map_Counter>::iterator iterator_cell;                   //С����ѯ������
	iterator_objtype = cell_objtype.begin();
	while(iterator_objtype != cell_objtype.end())
	{
        iterator_Object = iterator_objtype->second.begin();
		while(iterator_Object != iterator_objtype->second.end())
		{
			iterator_Object->v_sObject_ID.Replace("��","(");
			iterator_Object->v_sObject_ID.Replace("��",")");
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
//	�������ܣ��жϹ���״̬С��
//------------------------------------------------------------------------------------------------------
bool CMyFilter::JudgeEngineeringCell(CString cell_name)
{
	map<CString,map_Counter>::iterator iterator_cell;                   //С����ѯ������
	map<CString,CString>::iterator iterator_state;
	iterator_cell = cell_information_.find(cell_name);
	if (iterator_cell != cell_information_.end())
	{
	   iterator_state = iterator_cell->second.find("zOnline");
       if (iterator_state != iterator_cell->second.end())
       {
		   if (iterator_state->second == "����")
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
//  ��������:CON_LTE��ȡС��������������״̬
//-----------------------------------------------------------------------------------------------------------------
void  CMyFilter::GetCell()
{
	cell_information_.clear();
	_RecordsetPtr v_pRecordset;											  //��¼��
	CMyADOConn v_cAdoConn;												  //���ݿ�ʵ��
	CString			sql,sTemp,Key,value;							      //��ʱ����
	int             cell_ID;
	map<CString,CString> map_cell;
	try
	{
		sql="SELECT zCell_ID,zRegion,zOnline FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		      //ִ��SELETE���	
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
		v_cAdoConn.ExitConnect();										     //�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										  //�ر����ݿ�

		CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sError+"\r\n");	              //log
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  ���������๦�ܳ�ʼ��
//-----------------------------------------------------------------------------------------------------------------
void  CMyFilter::Init()
{
	//��ô���С����������������Ϣ
	GetCell();
}