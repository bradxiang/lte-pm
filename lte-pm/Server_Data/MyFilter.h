#pragma once
#include "Server_Data.h"

class CMyFilter
{
public:
	CMyFilter(void);
	~CMyFilter(void);
	//------------------------------------------------------------------------------------------------------
	//	��Ա������
	//------------------------------------------------------------------------------------------------------
	map<CString,map_Counter>       cell_information_;                                  //С������������������״̬
	//------------------------------------------------------------------------------------------------------
	//	��Ա������
	//------------------------------------------------------------------------------------------------------
	void FilterNonDalianCell(map_ObjType &cell_objtype);                               //���˷Ǵ���С��
	bool JudgeEngineeringCell(CString cell_name);                                      //�ж�С��״̬�����̷���false
	void GetCell();                                                                    //���С������������״̬
	void Init();                                                                       //�๦�ܳ�ʼ��
};
