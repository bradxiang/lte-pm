#include "stdafx.h"
#include "RegionPmProcessing.h"
//��ͼ��
#include "Server_DataDlg.h"
//XML�ļ�����
//#include "MyFile_XML.h"
RegionPmProcessing::RegionPmProcessing(void)
{
}

RegionPmProcessing::~RegionPmProcessing(void)
{
}
//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ��๦�ܳ�ʼ��
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::Init()
{
	//�����ݿ��ж�ȡ���ܹ�ʽ
	GetRegionFomula(); 
	//��ø������������Ʊ���
	GetBasePm();
	//���С����������
	GetCellAndRegion();
	//����������������
	GetRegionandPm();
	//���ļ�ȫ�������������������ʼ��
	network_Pm_iterm_ = basePm_name_;
	//����ȫ������������������
	network_grantularity_Pm_iterm_ = basePm_name_;
	//���ļ���������������������ʼ��
	other_Pm_iterm_ = basePm_name_;
	//�����������������������
	other_grantularity_Pm_iterm_ = basePm_name_;
	//���ļ���Ϊ�����������������ʼ��
	hw_Pm_iterm_ = basePm_name_;
	//���Ȼ�Ϊ����������������
	hw_grantularity_Pm_iterm_ = basePm_name_;
	//���ļ����˻����������������ʼ��
	zte_Pm_iterm_ = basePm_name_;
	//�������˻���������������
	zte_grantularity_Pm_iterm_ = basePm_name_;
	//����Ԥ����ʼ����
	Pm_warning_.Init();

}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ��๦�ܳ�ʼ��
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::Updata()
{
	//���С����������
	GetCellAndRegion();
	//����Ԥ����ʼ����
	Pm_warning_.Init();

}

//-----------------------------------------------------------------------------------------------------------------
//  ��������:����������������
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::GetRegionandPm()
{
	region_Pm_iterm_.clear();
	_RecordsetPtr v_pRecordset;											  //��¼��
	CMyADOConn v_cAdoConn;												  //���ݿ�ʵ��
	CString			sql,sTemp,key,value;							      //��ʱ����
	try
	{
		sql="SELECT zRegion FROM CON_Region";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		      //ִ��SELETE���	
		while(!v_pRecordset->adoEOF)
		{
			sTemp=v_cAdoConn.GetField_String("zRegion");
			key = sTemp;
			region_Pm_iterm_.insert(pair<CString,map_Counter>(key,basePm_name_));
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
	region_grantularity_Pm_iterm_.clear();
	region_grantularity_Pm_iterm_ = region_Pm_iterm_;
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ������ݿ��ж�ȡ�������ܴ���ʽ
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::GetRegionFomula()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString v_sSql,formula,formula_name;
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	map_formula_.clear();
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);
	v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Region";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
	while (!v_pRecordset->adoEOF)
	{
		formula_name = v_cAdoConn.GetField_String("zPm");			//��ʽ����
		formula = v_cAdoConn.GetField_String("zPm_Formula");			       //��ʽ����ֵ
		map_formula_.insert(pair<CString,CString> (formula_name,formula));
		v_pRecordset->MoveNext();
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ���û�����������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::GetBasePm()
{
	int				v_iItem,v_iID=0;									//KPI����
	CString			v_sTemp,v_sSql,v_sLog_Key;							//��ʱ����
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//��ʱ����
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	basePm_name_.clear();
	try
	{											//��Ŀѡ����Ч

		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";
		v_sSql += "SELECT zPm FROM CON_Pm_LTE ";
		v_sSql+=" order by ztype,ztype_1,zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			basePm_name_.insert(pair<CString,CString>(v_sPm,""));
			v_pRecordset->MoveNext();									//
		}
		v_cAdoConn.ExitConnect();										//�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�ȫ�����ļ���������������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmForWholeNetwork(map_ObjType &v_mObjType,CMyFilter &cell_filter)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_performance_iterm;				    //���ܹ�ʽ������
	map_Counter::iterator	iterator_sum_iterm;                         //ȫ�����ܹ�ʽ������������
	map_Counter::iterator   iterator_cell,iterator_performance;         //С��������
	CString cell,region,performance,stemp;                              //С������
	map<CString,map_Counter>::iterator iterator_region;
	float performance_number(0);
	bool cell_state;                                                     //С��״̬�ж�
	//������͵���
	iterator_sum_iterm = network_Pm_iterm_.begin();
	//����ѭ��
	while (iterator_sum_iterm != network_Pm_iterm_.end())
	{
		iterator_performance_objtype = v_mObjType.begin();
		//�ļ�ѭ��
		while(iterator_performance_objtype != v_mObjType.end())
		{
			iterator_performance_object = iterator_performance_objtype->second.begin();
			//С��ѭ��
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell = iterator_performance_object->v_sObject_ID;
				//���˹���С��
				cell_state = cell_filter.JudgeEngineeringCell(cell);
				//�ж�С��״̬
				if (cell_state)
				{  
					iterator_performance_iterm = iterator_performance_object->v_mCounter.find(iterator_sum_iterm->first);
					if (iterator_performance_iterm != iterator_performance_object->v_mCounter.end())
					{
					performance = iterator_sum_iterm->first;
					//�������
					performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
					} 
				} 
				iterator_performance_object++;
			}
			iterator_performance_objtype++;
		}
		//���ܸ�ֵ
		iterator_sum_iterm->second.Format("%f",performance_number);
		performance_number = 0;
		iterator_sum_iterm++;
	}	
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���Ϊ���ļ���������������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmForHW(map_ObjType &v_mObjType,CMyFilter &cell_filter)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_performance_iterm;				    //���ܹ�ʽ������
	map_Counter::iterator	iterator_sum_iterm;                         //ȫ�����ܹ�ʽ������������
	map_Counter::iterator   iterator_cell,iterator_performance;         //С��������
	CString cell,region,performance,stemp;                              //С������
	map<CString,map_Counter>::iterator iterator_region;
	float performance_number(0);
	bool cell_state;                                                     //С��״̬�ж�
	//������͵���
	iterator_sum_iterm = hw_Pm_iterm_.begin();
	//����ѭ��
	while (iterator_sum_iterm != hw_Pm_iterm_.end())
	{
		iterator_performance_objtype = v_mObjType.begin();
		//�ļ�ѭ��
		while(iterator_performance_objtype != v_mObjType.end())
		{
			iterator_performance_object = iterator_performance_objtype->second.begin();
			//С��ѭ��
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell = iterator_performance_object->v_sObject_ID;
				//���˹���С��
				cell_state = cell_filter.JudgeEngineeringCell(cell);
				//�ж�С��״̬
				if (cell_state)
				{  
					iterator_performance_iterm = iterator_performance_object->v_mCounter.find(iterator_sum_iterm->first);
					if (iterator_performance_iterm != iterator_performance_object->v_mCounter.end())
					{
						performance = iterator_sum_iterm->first;
						//�������
						performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
					} 
				} 
				iterator_performance_object++;
			}
			iterator_performance_objtype++;
		}
		//���ܸ�ֵ
		iterator_sum_iterm->second.Format("%f",performance_number);
		performance_number = 0;
		iterator_sum_iterm++;
	}	
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ����˵��ļ���������������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmForZTE(map_ObjType &v_mObjType,CMyFilter &cell_filter)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_performance_iterm;				    //���ܹ�ʽ������
	map_Counter::iterator	iterator_sum_iterm;                         //ȫ�����ܹ�ʽ������������
	map_Counter::iterator   iterator_cell,iterator_performance;         //С��������
	CString cell,region,performance,stemp;                              //С������
	map<CString,map_Counter>::iterator iterator_region;
	float performance_number(0);
	bool cell_state;                                                     //С��״̬�ж�
	//������͵���
	iterator_sum_iterm = zte_Pm_iterm_.begin();
	//����ѭ��
	while (iterator_sum_iterm != zte_Pm_iterm_.end())
	{
		iterator_performance_objtype = v_mObjType.begin();
		//�ļ�ѭ��
		while(iterator_performance_objtype != v_mObjType.end())
		{
			iterator_performance_object = iterator_performance_objtype->second.begin();
			//С��ѭ��
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell = iterator_performance_object->v_sObject_ID;
				//���˹���С��
				cell_state = cell_filter.JudgeEngineeringCell(cell);
				//�ж�С��״̬
				if (cell_state)
				{  
					iterator_performance_iterm = iterator_performance_object->v_mCounter.find(iterator_sum_iterm->first);
					if (iterator_performance_iterm != iterator_performance_object->v_mCounter.end())
					{
						performance = iterator_sum_iterm->first;
						//�������
						performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
					} 
				} 
				iterator_performance_object++;
			}
			iterator_performance_objtype++;
		}
		//���ܸ�ֵ
		iterator_sum_iterm->second.Format("%f",performance_number);
		performance_number = 0;
		iterator_sum_iterm++;
	}	
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�ȫ��ÿ�����Ȼ��������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForWholeNetwork()
{
	map_Counter::iterator	iterator_whole_network_grantularity_Pm_iterm;		//������
	map_Counter::iterator	iterator_network_Pm_iterm;				    //������
	float value;
	iterator_whole_network_grantularity_Pm_iterm = network_grantularity_Pm_iterm_.begin();
	while (iterator_whole_network_grantularity_Pm_iterm != network_grantularity_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm = network_Pm_iterm_.find(iterator_whole_network_grantularity_Pm_iterm->first);
		if (iterator_network_Pm_iterm != network_Pm_iterm_.end())
		{
			value = atof(iterator_network_Pm_iterm->second) + atof(iterator_whole_network_grantularity_Pm_iterm->second);
			iterator_whole_network_grantularity_Pm_iterm->second.Format("%f",value);
		}
		iterator_whole_network_grantularity_Pm_iterm ++;
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���Ϊÿ�����Ȼ��������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForHW()
{
	map_Counter::iterator	iterator_whole_network_grantularity_Pm_iterm;		//������
	map_Counter::iterator	iterator_network_Pm_iterm;				    //������
	float value;
	iterator_whole_network_grantularity_Pm_iterm = hw_grantularity_Pm_iterm_.begin();
	while (iterator_whole_network_grantularity_Pm_iterm != hw_grantularity_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm = hw_Pm_iterm_.find(iterator_whole_network_grantularity_Pm_iterm->first);
		if (iterator_network_Pm_iterm != hw_Pm_iterm_.end())
		{
			value = atof(iterator_network_Pm_iterm->second) + atof(iterator_whole_network_grantularity_Pm_iterm->second);
			iterator_whole_network_grantularity_Pm_iterm->second.Format("%f",value);
		}
		iterator_whole_network_grantularity_Pm_iterm ++;
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�����ÿ�����Ȼ��������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForZTE()
{
	map_Counter::iterator	iterator_whole_network_grantularity_Pm_iterm;		//������
	map_Counter::iterator	iterator_network_Pm_iterm;				    //������
	float value;
	iterator_whole_network_grantularity_Pm_iterm = zte_grantularity_Pm_iterm_.begin();
	while (iterator_whole_network_grantularity_Pm_iterm != zte_grantularity_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm = zte_Pm_iterm_.find(iterator_whole_network_grantularity_Pm_iterm->first);
		if (iterator_network_Pm_iterm != zte_Pm_iterm_.end())
		{
			value = atof(iterator_network_Pm_iterm->second) + atof(iterator_whole_network_grantularity_Pm_iterm->second);
			iterator_whole_network_grantularity_Pm_iterm->second.Format("%f",value);
		}
		iterator_whole_network_grantularity_Pm_iterm ++;
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ�ʵ�����ܹ�ʽ�ֽ�
//  ��    ��:v_sFormula��������㹫ʽ v_mFormula���ֽ⹫ʽ
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula)
{
	CString		v_sStr = "";												//��ʱ����
	bool		v_bFlag = false;											//��ǣ������
	char*		v_pFormula;												    //��ʱ����
	PM_Formula_Struct	v_cPM_Formula;									    //�ṹ
	v_mFormula->clear();
	v_pFormula = v_sFormula.GetBuffer(0);
	for (int vi = 0; vi<v_sFormula.GetLength(); vi++)						//�������㹫ʽ
	{
		if ((unsigned char)v_pFormula[vi]<0x30)							    //�����������
		{
			if (v_bFlag)
			{
				if (!v_sStr.IsEmpty())
				{
					v_cPM_Formula.v_sPM_Formula = v_sStr;
					v_cPM_Formula.v_bFormula = v_bFlag;
					if (v_sStr.Left(1) <= "9")							    //��Ԥ��ֵ�ĵ�һλ����Ϊ�ַ�������Ϊ�������磺100�ȣ�����������������û�ȡCounterֵ��
						v_cPM_Formula.v_bFormula = false;					//�����
					v_mFormula->push_back(v_cPM_Formula);				    //���ӣ�����ֵ
					v_sStr = "";											//��λ
				}
				v_bFlag = false;											//��ǣ������
			}
		}
		else
		{
			if (!v_bFlag)
			{
				if (!v_sStr.IsEmpty())
				{
					v_cPM_Formula.v_sPM_Formula = v_sStr;
					v_cPM_Formula.v_bFormula = v_bFlag;
					v_mFormula->push_back(v_cPM_Formula);				    //���ӣ������
					v_sStr = "";											//��λ
				}
				v_bFlag = true;											    //��ǣ�����ֵ
			}
		}
		v_sStr += v_sFormula.Mid(vi, 1);									//��ȡ���ַ���
	}
	if (!v_sStr.IsEmpty())
	{
		v_cPM_Formula.v_sPM_Formula = v_sStr;
		v_cPM_Formula.v_bFormula = v_bFlag;
		if (v_cPM_Formula.v_bFormula)									    //����ֵ������
		{
			if (v_sStr.Left(1) <= "9")									    //��Ԥ��ֵ�ĵ�һλ����Ϊ�ַ�������Ϊ�������磺100�ȣ�����������������û�ȡCounterֵ��
				v_cPM_Formula.v_bFormula = false;							//�����
		}
		v_mFormula->push_back(v_cPM_Formula);							    //���ӣ�����ֵ/�����
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ�ʵ�����ܹ�ʽ�ϳ�
//  ��    ��:CSstringformula:�ϳɹ�ʽ StructFormula���ֽ⹫ʽ
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mPM_Formula::iterator PMiterator;
	CMyFormula calculateformula;
	CString CSstringtempary;
	PMiterator = StructFormula.begin();
	*CSstringformula = "";
	while (PMiterator != StructFormula.end())
	{
		if (PMiterator->v_bFormula == true)
		{
			*CSstringformula += PMiterator->v_sPM_Formula;
		}
		else
		{
			*CSstringformula += PMiterator->v_sPM_Formula;
		}
		PMiterator++;
	}

}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�ȫ���������ܼ���
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculatePmForWholeNetwork()
{
	network_Pm_.clear();                                          //�������
	//��ʽ����
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	mPM_Formula::iterator   iterator_decompose_formula;                 //�ֽ⹫ʽ������
	map_Counter::iterator	iterator_counter;						    //count���ܵ�����
	map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
	map_Counter::iterator	iterator_performance_iterm;
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//���㹫ʽ����ʽԭ��;��������;����ֵ
	CMyFormula calculate_formula;//��������ָ����
	char * Pcharformula = NULL;//��������ָ������
	bool performance_flag = false;//����ֵƥ���ǩ
	float number_result = 0;//����ֵ
	mPM_Formula  decompose_formula;//�ֽ⹫ʽ
	//���ܹ�ʽѭ��
	iterator_map_formula = map_formula_.begin();
	while(iterator_map_formula != map_formula_.end())
	{
		//��������
		performace_name = iterator_map_formula->first;
		//�����������ݽṹ
		formula = iterator_map_formula->second;
		//�滻"."Ϊaaa
		formula.Replace(".", "aaa");
		//����ʽ�ֽ�;�ɱ����������
		DecomposeFormula(formula, &decompose_formula);
		//��ԭ���������е�"."
		iterator_decompose_formula = decompose_formula.begin();
		while (iterator_decompose_formula != decompose_formula.end())
		{
			iterator_decompose_formula->v_sPM_Formula.Replace("aaa", ".");
			iterator_decompose_formula++;
		}
		//��ʽ�����滻Ϊ����ֵ
		iterator_decompose_formula = decompose_formula.begin();
		while (iterator_decompose_formula != decompose_formula.end())
		{
			//�����ʽ�еı���
			if (iterator_decompose_formula->v_bFormula == true)
			{
				iterator_counter =  network_grantularity_Pm_iterm_.begin();
				performance_flag = false;
				while (iterator_counter !=  network_grantularity_Pm_iterm_.end() && (!performance_flag))
				{
					//�ҵ�ƥ�������ֵ
					if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
					{
						iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						performance_flag = true;
					}
					iterator_counter++;
				}
				if (!performance_flag)
				{
					//Ӧ�ø�Ϊ�쳣��ʾ
					//MessageBoxA("����δƥ��");
				}
				iterator_decompose_formula++;
			}
			else
			{
				iterator_decompose_formula++;
				continue;
			}

		}
		//��ԭ���㹫ʽ
		CompositionFormula(&formula_result, decompose_formula);
		//���ܼ���
		Pcharformula = formula_result.GetBuffer(0);
		number_result = calculate_formula.simcalc(Pcharformula);
		performace_cstring_value.Format("%f", number_result);
		network_Pm_.insert(pair <CString,CString>(performace_name,performace_cstring_value));   
		iterator_map_formula++;
	}
	//���ݸ���
	iterator_performance_iterm = network_grantularity_Pm_iterm_.begin();
	while (iterator_performance_iterm != network_grantularity_Pm_iterm_.end())
	{
		iterator_performance_iterm->second = "";
		iterator_performance_iterm++;
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���Ϊ�������ܼ���
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculatePmForHW()
{
	hw_Pm_.clear();                                          //�������
	//��ʽ����
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	mPM_Formula::iterator   iterator_decompose_formula;                 //�ֽ⹫ʽ������
	map_Counter::iterator	iterator_counter;						    //count���ܵ�����
	map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
	map_Counter::iterator	iterator_performance_iterm;
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//���㹫ʽ����ʽԭ��;��������;����ֵ
	CMyFormula calculate_formula;//��������ָ����
	char * Pcharformula = NULL;//��������ָ������
	bool performance_flag = false;//����ֵƥ���ǩ
	float number_result = 0;//����ֵ
	mPM_Formula  decompose_formula;//�ֽ⹫ʽ
	//���ܹ�ʽѭ��
	iterator_map_formula = map_formula_.begin();
	while(iterator_map_formula != map_formula_.end())
	{
		//��������
		performace_name = iterator_map_formula->first;
		//�����������ݽṹ
		formula = iterator_map_formula->second;
		//�滻"."Ϊaaa
		formula.Replace(".", "aaa");
		//����ʽ�ֽ�;�ɱ����������
		DecomposeFormula(formula, &decompose_formula);
		//��ԭ���������е�"."
		iterator_decompose_formula = decompose_formula.begin();
		while (iterator_decompose_formula != decompose_formula.end())
		{
			iterator_decompose_formula->v_sPM_Formula.Replace("aaa", ".");
			iterator_decompose_formula++;
		}
		//��ʽ�����滻Ϊ����ֵ
		iterator_decompose_formula = decompose_formula.begin();
		while (iterator_decompose_formula != decompose_formula.end())
		{
			//�����ʽ�еı���
			if (iterator_decompose_formula->v_bFormula == true)
			{
				iterator_counter =  hw_grantularity_Pm_iterm_.begin();
				performance_flag = false;
				while (iterator_counter !=  hw_grantularity_Pm_iterm_.end() && (!performance_flag))
				{
					//�ҵ�ƥ�������ֵ
					if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
					{
						iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						performance_flag = true;
					}
					iterator_counter++;
				}
				if (!performance_flag)
				{
					//Ӧ�ø�Ϊ�쳣��ʾ
					//MessageBoxA("����δƥ��");
				}
				iterator_decompose_formula++;
			}
			else
			{
				iterator_decompose_formula++;
				continue;
			}

		}
		//��ԭ���㹫ʽ
		CompositionFormula(&formula_result, decompose_formula);
		//���ܼ���
		Pcharformula = formula_result.GetBuffer(0);
		number_result = calculate_formula.simcalc(Pcharformula);
		performace_cstring_value.Format("%f", number_result);
		hw_Pm_.insert(pair <CString,CString>(performace_name,performace_cstring_value));   
		iterator_map_formula++;
	}
	//���ݸ���
	iterator_performance_iterm = hw_grantularity_Pm_iterm_.begin();
	while (iterator_performance_iterm != hw_grantularity_Pm_iterm_.end())
	{
		iterator_performance_iterm->second = "";
		iterator_performance_iterm++;
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ������������ܼ���
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculatePmForZTE()
{
	zte_Pm_.clear();                                          //�������
	//��ʽ����
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	mPM_Formula::iterator   iterator_decompose_formula;                 //�ֽ⹫ʽ������
	map_Counter::iterator	iterator_counter;						    //count���ܵ�����
	map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
	map_Counter::iterator	iterator_performance_iterm;
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//���㹫ʽ����ʽԭ��;��������;����ֵ
	CMyFormula calculate_formula;//��������ָ����
	char * Pcharformula = NULL;//��������ָ������
	bool performance_flag = false;//����ֵƥ���ǩ
	float number_result = 0;//����ֵ
	mPM_Formula  decompose_formula;//�ֽ⹫ʽ
	//���ܹ�ʽѭ��
	iterator_map_formula = map_formula_.begin();
	while(iterator_map_formula != map_formula_.end())
	{
		//��������
		performace_name = iterator_map_formula->first;
		//�����������ݽṹ
		formula = iterator_map_formula->second;
		//�滻"."Ϊaaa
		formula.Replace(".", "aaa");
		//����ʽ�ֽ�;�ɱ����������
		DecomposeFormula(formula, &decompose_formula);
		//��ԭ���������е�"."
		iterator_decompose_formula = decompose_formula.begin();
		while (iterator_decompose_formula != decompose_formula.end())
		{
			iterator_decompose_formula->v_sPM_Formula.Replace("aaa", ".");
			iterator_decompose_formula++;
		}
		//��ʽ�����滻Ϊ����ֵ
		iterator_decompose_formula = decompose_formula.begin();
		while (iterator_decompose_formula != decompose_formula.end())
		{
			//�����ʽ�еı���
			if (iterator_decompose_formula->v_bFormula == true)
			{
				iterator_counter =  zte_grantularity_Pm_iterm_.begin();
				performance_flag = false;
				while (iterator_counter !=  zte_grantularity_Pm_iterm_.end() && (!performance_flag))
				{
					//�ҵ�ƥ�������ֵ
					if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
					{
						iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						performance_flag = true;
					}
					iterator_counter++;
				}
				if (!performance_flag)
				{
					//Ӧ�ø�Ϊ�쳣��ʾ
					//MessageBoxA("����δƥ��");
				}
				iterator_decompose_formula++;
			}
			else
			{
				iterator_decompose_formula++;
				continue;
			}

		}
		//��ԭ���㹫ʽ
		CompositionFormula(&formula_result, decompose_formula);
		//���ܼ���
		Pcharformula = formula_result.GetBuffer(0);
		number_result = calculate_formula.simcalc(Pcharformula);
		performace_cstring_value.Format("%f", number_result);
		zte_Pm_.insert(pair <CString,CString>(performace_name,performace_cstring_value));   
		iterator_map_formula++;
	}
	//���ݸ���
	iterator_performance_iterm = zte_grantularity_Pm_iterm_.begin();
	while (iterator_performance_iterm != zte_grantularity_Pm_iterm_.end())
	{
		iterator_performance_iterm->second = "";
		iterator_performance_iterm++;
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ����������ϴ����ݿ�
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::UploadingPm(CString time,map_Counter &Pm,CString region)
{
	_RecordsetPtr   v_pRecordset;									//��¼��
	CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
	CString			sql,sql_key="",sql_value="";							            //��ʱ����
	map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
	try
	{  
		sql="INSERT INTO Pm_LTE_Region ( zTime,zCell";
		iterator_whole_network_performance_updata = Pm.begin();
		while (iterator_whole_network_performance_updata != Pm.end())
		{   sql_key += ",[" + iterator_whole_network_performance_updata->first + "]";
		sql_value += "," + iterator_whole_network_performance_updata->second;
		iterator_whole_network_performance_updata ++;
		}
		sql += sql_key + ")VALUES('" + time + "','" + region + "'" + sql_value + ")";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
		v_cAdoConn.ExitConnect();										     //�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										  //�ر����ݿ�
		CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
	}
	//�ļ�ʱ��
	Pm_warning_.file_time_ = time;
	//����Ԥ��
	if (region = "����")
	{
		Pm_warning_.CompareToOtherPerformance(Pm);
	}
	else if (region = "ȫ��")
	{
		Pm_warning_.CompareToWholeNetworkPerformance(Pm);
	}
	else if (region = "����")
	{
		Pm_warning_.CompareToZTEPerformance(Pm);
	}
	else if (region = "��Ϊ")
	{
		Pm_warning_.CompareToHWPerformance(Pm);
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ������������ܼ���
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculatePmForOther()
{
	other_Pm_.clear();                                                    //�������
	//��ʽ����
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	mPM_Formula::iterator   iterator_decompose_formula;                 //�ֽ⹫ʽ������
	map_Counter::iterator	iterator_counter;						    //count���ܵ�����
	map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
	map_Counter::iterator	iterator_performance_iterm;
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//���㹫ʽ����ʽԭ��;��������;����ֵ
	CMyFormula calculate_formula;//��������ָ����
	char * Pcharformula = NULL;//��������ָ������
	bool performance_flag = false;//����ֵƥ���ǩ
	float number_result = 0;//����ֵ
	mPM_Formula  decompose_formula;//�ֽ⹫ʽ
	//���ܹ�ʽѭ��
	iterator_map_formula = map_formula_.begin();
	while(iterator_map_formula != map_formula_.end())
	{
		//��������
		performace_name = iterator_map_formula->first;
		//�����������ݽṹ
		formula = iterator_map_formula->second;
		//�滻"."Ϊaaa
		formula.Replace(".", "aaa");
		//����ʽ�ֽ�;�ɱ����������
		DecomposeFormula(formula, &decompose_formula);
		//��ԭ���������е�"."
		iterator_decompose_formula = decompose_formula.begin();
		while (iterator_decompose_formula != decompose_formula.end())
		{
			iterator_decompose_formula->v_sPM_Formula.Replace("aaa", ".");
			iterator_decompose_formula++;
		}
		//��ʽ�����滻Ϊ����ֵ
		iterator_decompose_formula = decompose_formula.begin();
		while (iterator_decompose_formula != decompose_formula.end())
		{
			//�����ʽ�еı���
			if (iterator_decompose_formula->v_bFormula == true)
			{
				iterator_counter =  other_grantularity_Pm_iterm_.begin();
				performance_flag = false;
				while (iterator_counter !=  other_grantularity_Pm_iterm_.end() && (!performance_flag))
				{
					//�ҵ�ƥ�������ֵ
					if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
					{
						iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						performance_flag = true;
					}
					iterator_counter++;
				}
				if (!performance_flag)
				{
					//Ӧ�ø�Ϊ�쳣��ʾ
					//MessageBoxA("����δƥ��");
				}
				iterator_decompose_formula++;
			}
			else
			{
				iterator_decompose_formula++;
				continue;
			}

		}
		//��ԭ���㹫ʽ
		CompositionFormula(&formula_result, decompose_formula);
		//���ܼ���
		Pcharformula = formula_result.GetBuffer(0);
		number_result = calculate_formula.simcalc(Pcharformula);
		performace_cstring_value.Format("%f", number_result);
		other_Pm_.insert(pair <CString,CString>(performace_name,performace_cstring_value));   
		iterator_map_formula++;
	}
	//���ݸ���
	iterator_performance_iterm = other_grantularity_Pm_iterm_.begin();
	while (iterator_performance_iterm != other_grantularity_Pm_iterm_.end())
	{
		iterator_performance_iterm->second = "";
		iterator_performance_iterm++;
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ������ļ���������������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmForRegionandOther(map_ObjType &v_mObjType,CMyFilter &cell_filter)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_performance_iterm;				    //���ܹ�ʽ������
	map_Counter::iterator	iterator_sum_iterm;                         //ȫ�����ܹ�ʽ������������
	map_Counter::iterator   iterator_cell,iterator_performance;         //С��������
	CString cell,region,performance,stemp;                              //С������
	map<CString,map_Counter>::iterator iterator_region;
	float performance_number=0,region_performance_number=0;;
	bool cell_state;                                                     //С��״̬�ж�
	//����������͵���
	iterator_sum_iterm = basePm_name_.begin();
	//����ѭ��
	while (iterator_sum_iterm != basePm_name_.end())
	{
		//��������
		performance = iterator_sum_iterm->first;
		iterator_performance_objtype = v_mObjType.begin();
		//�ļ�ѭ��
		while(iterator_performance_objtype != v_mObjType.end())
		{
			iterator_performance_object = iterator_performance_objtype->second.begin();
			//С��ѭ��
			while (iterator_performance_object != iterator_performance_objtype->second.end())
			{
				cell = iterator_performance_object->v_sObject_ID;
				//���˹���С��
				cell_state = cell_filter.JudgeEngineeringCell(cell);
				//�ж�С��״̬
				if (cell_state)
				{
					iterator_performance_iterm = iterator_performance_object->v_mCounter.find(iterator_sum_iterm->first);
					if (iterator_performance_iterm != iterator_performance_object->v_mCounter.end())
					{
						//�������
						iterator_cell = cell_region_.find(cell);
						if (iterator_cell != cell_region_.end())
						{
							region = iterator_cell->second;
							iterator_region = region_Pm_iterm_.find(region);
							if (iterator_region != region_Pm_iterm_.end())
							{
								iterator_performance = iterator_region->second.find(performance);
								if (iterator_performance != iterator_region->second.end())
								{
									stemp = iterator_performance->second;
									region_performance_number = atof(stemp);
									region_performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
									iterator_performance->second.Format("%f",region_performance_number);
								}
							}
							else
							{
								iterator_performance = other_Pm_iterm_.find(performance);
								if (iterator_performance != other_Pm_iterm_.end())
								{
									stemp = iterator_performance->second;
									region_performance_number = atof(stemp);
									region_performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
									iterator_performance->second.Format("%f",region_performance_number);
								}
							}
						}
						else
						{
							iterator_performance = other_Pm_iterm_.find(performance);
							if (iterator_performance != other_Pm_iterm_.end())
							{
								stemp = iterator_performance->second;
								region_performance_number = atof(stemp);
								region_performance_number += atof(iterator_performance_iterm->second.GetBuffer(0));
								iterator_performance->second.Format("%f",region_performance_number);
							}
						}
					} 
				}
				iterator_performance_object++;			
			}
			iterator_performance_objtype++;
		}
		iterator_sum_iterm++;
	}	
}
//-----------------------------------------------------------------------------------------------------------------
//  ��������:��ȡС����������
//-----------------------------------------------------------------------------------------------------------------
void RegionPmProcessing::GetCellAndRegion()
{
	_RecordsetPtr v_pRecordset;											  //��¼��
	CMyADOConn v_cAdoConn;												  //���ݿ�ʵ��
	CString			sql,sTemp,Key,value;							      //��ʱ����
	int          cell_ID;
	try
	{
		sql="SELECT zCell_ID,zRegion FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		      //ִ��SELETE���	
		while(!v_pRecordset->adoEOF)
		{
			cell_ID = v_cAdoConn.GetField_Int("zCell_ID");
			Key.Format("%d",cell_ID);
			value = v_cAdoConn.GetField_String("zRegion");
			cell_region_.insert(pair<CString,CString>(Key,value));
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
//------------------------------------------------------------------------------------------------------			
//�������ܣ�����ÿ�����Ȼ��������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForRegion()
{
	map_Counter::iterator	iterator_performance_iterm;						    //ȫ�����ܹ�ʽ������������
	map_Counter::iterator	iterator_formula_performance_iterm;		            //ȫ�����ܹ�ʽ������������
	map_Counter::iterator	iterator_base254_performance_iterm;				    //ȫ�����ܹ�ʽ������������
	float performance_whole_network,sub_performance_whole_network;
	map_Counter whole_network_performance_updata;
	CString  region_name;
	map<CString,map_Counter>::iterator iterator_region_iterm, iterator_base254_region_iterm;                   //������
	iterator_region_iterm = region_grantularity_Pm_iterm_.begin();
	//����ѭ��
	while (iterator_region_iterm != region_grantularity_Pm_iterm_.end())
	{
		//��������
		region_name = iterator_region_iterm->first;           
		iterator_performance_iterm = iterator_region_iterm->second.begin();
		//����ѭ��
		while (iterator_performance_iterm != iterator_region_iterm->second.end())
		{
			iterator_base254_region_iterm = region_Pm_iterm_.find(region_name);
			if (iterator_base254_region_iterm != region_Pm_iterm_.end())
			{
				iterator_formula_performance_iterm = iterator_base254_region_iterm->second.find(iterator_performance_iterm->first);
				if (iterator_formula_performance_iterm != iterator_base254_region_iterm->second.end())
				{
					performance_whole_network = atof(iterator_performance_iterm->second);
					sub_performance_whole_network = atof(iterator_formula_performance_iterm->second);
					performance_whole_network += sub_performance_whole_network;
					iterator_performance_iterm->second.Format("%f",performance_whole_network);
				}

			}
			iterator_performance_iterm++;
		}
		iterator_region_iterm++ ;
	}
	//region_Pm_iterm_���ݹ���
	iterator_region_iterm = region_Pm_iterm_.begin();
	//����ѭ��
	while (iterator_region_iterm != region_Pm_iterm_.end())
	{
		iterator_performance_iterm = iterator_region_iterm->second.begin();
		//����ѭ��
		while (iterator_performance_iterm != iterator_region_iterm->second.end())
		{
			iterator_performance_iterm->second ="";
			iterator_performance_iterm++;
		}
		iterator_region_iterm++ ;
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ������������ܼ���
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::CalculateandUploadingPmForRegion(CString time)
{
	map_Counter::iterator	iterator_performance_iterm;						    //ȫ�����ܹ�ʽ������������
	map_Counter::iterator	iterator_formula_performance_iterm;		            //ȫ�����ܹ�ʽ������������
	map_Counter::iterator	iterator_base254_performance_iterm;				    //ȫ�����ܹ�ʽ������������
	float performance_whole_network,sub_performance_whole_network;
	map_Counter whole_network_performance_updata;
	CString  region_name;
	map<CString,map_Counter>::iterator iterator_region_iterm, iterator_base254_region_iterm;                   //������
	//��ʽ����
	iterator_region_iterm = region_grantularity_Pm_iterm_.begin();
	//����ѭ��
	while (iterator_region_iterm != region_grantularity_Pm_iterm_.end())
	{
		region_name = iterator_region_iterm->first;           //��������
		map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
		mPM_Formula::iterator   iterator_decompose_formula;                 //�ֽ⹫ʽ������
		map_Counter::iterator	iterator_counter;						    //count���ܵ�����
		map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
		CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//���㹫ʽ����ʽԭ��;��������;����ֵ
		CMyFormula calculate_formula;//��������ָ����
		char * Pcharformula = NULL;//��������ָ������
		bool performance_flag = false;//����ֵƥ���ǩ
		float number_result = 0;//����ֵ
		mPM_Formula  decompose_formula;//�ֽ⹫ʽ
		//���ܹ�ʽѭ��
		iterator_map_formula = map_formula_.begin();
		while(iterator_map_formula != map_formula_.end())
		{
			//��������
			performace_name = iterator_map_formula->first;
			//�����������ݽṹ
			formula = iterator_map_formula->second;
			//�滻"."Ϊaaa
			formula.Replace(".", "aaa");
			//����ʽ�ֽ�;�ɱ����������
			DecomposeFormula(formula, &decompose_formula);
			//��ԭ���������е�"."
			iterator_decompose_formula = decompose_formula.begin();
			while (iterator_decompose_formula != decompose_formula.end())
			{
				iterator_decompose_formula->v_sPM_Formula.Replace("aaa", ".");
				iterator_decompose_formula++;
			}
			//��ʽ�����滻Ϊ����ֵ
			iterator_decompose_formula = decompose_formula.begin();
			while (iterator_decompose_formula != decompose_formula.end())
			{
				//�����ʽ�еı���
				if (iterator_decompose_formula->v_bFormula == true)
				{
					iterator_counter =  iterator_region_iterm->second.begin();
					performance_flag = false;
					while (iterator_counter !=  iterator_region_iterm->second.end() && (!performance_flag))
					{
						//�ҵ�ƥ�������ֵ
						if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
						{
							iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
							performance_flag = true;
						}
						iterator_counter++;
					}
					if (!performance_flag)
					{
						//Ӧ�ø�Ϊ�쳣��ʾ
						//MessageBoxA("����δƥ��");
					}
					iterator_decompose_formula++;
				}
				else
				{
					iterator_decompose_formula++;
					continue;
				}

			}
			//��ԭ���㹫ʽ
			CompositionFormula(&formula_result, decompose_formula);
			//���ܼ���
			Pcharformula = formula_result.GetBuffer(0);
			number_result = calculate_formula.simcalc(Pcharformula);
			performace_cstring_value.Format("%f", number_result);
			whole_network_performance_updata.insert(pair <CString,CString>(performace_name,performace_cstring_value));
			iterator_map_formula++;
		}
		//�����ϴ�
		_RecordsetPtr   v_pRecordset;									//��¼��
		CMyADOConn      v_cAdoConn;												//���ݿ�ʵ��
		CString			sql,sql_key="",sql_value="";							            //��ʱ����
		map_Counter::iterator	iterator_whole_network_performance_updata;						    //count���ܵ�����
		try
		{  
			sql="INSERT INTO Pm_LTE_Region ( zTime,zCell";
			iterator_whole_network_performance_updata = whole_network_performance_updata.begin();
			while (iterator_whole_network_performance_updata != whole_network_performance_updata.end())
			{   sql_key += ",[" + iterator_whole_network_performance_updata->first + "]";
			sql_value += "," + iterator_whole_network_performance_updata->second;
			iterator_whole_network_performance_updata ++;
			}
			sql += sql_key + ")VALUES('" + time + "','"+ region_name +"'" + sql_value + ")";
			v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
			v_cAdoConn.SetCommandTimeout(100);
			v_cAdoConn.ExecuteSQL((_bstr_t)sql);							//ִ�����ݿ����
			v_cAdoConn.ExitConnect();										     //�Ͽ�����
		}
		catch(_com_error e)
		{
			v_cAdoConn.ExitConnect();										  //�ر����ݿ�
			CString v_sError = (char *)e.Description();						  //��ã�������Ϣ
			theApp.m_MyLog.WriteText("�쳣��"+v_sError+"  "+sql+"\r\n");	  //log
		}
		//����Ԥ��
		Pm_warning_.file_time_ = time;
		Pm_warning_.CompareToRegionPerformance(whole_network_performance_updata,region_name);
		whole_network_performance_updata.clear();
		iterator_region_iterm++ ;	
	}
	//���ݸ���
	iterator_region_iterm = region_grantularity_Pm_iterm_.begin();
	//����ѭ��
	while (iterator_region_iterm != region_grantularity_Pm_iterm_.end())
	{
		region_name = iterator_region_iterm->first;           //��������
		iterator_performance_iterm = iterator_region_iterm->second.begin();
		//����ѭ��
		while (iterator_performance_iterm != iterator_region_iterm->second.end())
		{
			iterator_performance_iterm->second ="";
			iterator_performance_iterm++;
		}
		iterator_region_iterm++ ;
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ�����ÿ�����Ȼ��������������
//------------------------------------------------------------------------------------------------------
void RegionPmProcessing::SumBasePmInGranularityForOther()
{
	map_Counter::iterator	iterator_whole_network_grantularity_Pm_iterm;		//������
	map_Counter::iterator	iterator_network_Pm_iterm;				            //������
	float value;
	iterator_whole_network_grantularity_Pm_iterm = other_grantularity_Pm_iterm_.begin();
	while (iterator_whole_network_grantularity_Pm_iterm != other_grantularity_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm = other_Pm_iterm_.find(iterator_whole_network_grantularity_Pm_iterm->first);
		if (iterator_network_Pm_iterm != other_Pm_iterm_.end())
		{
			value = atof(iterator_network_Pm_iterm->second) + atof(iterator_whole_network_grantularity_Pm_iterm->second);
			iterator_whole_network_grantularity_Pm_iterm->second.Format("%f",value);
		}
		iterator_whole_network_grantularity_Pm_iterm ++;
	}
	//���ݹ���
	iterator_network_Pm_iterm = other_Pm_iterm_.begin();
	while (iterator_network_Pm_iterm != other_Pm_iterm_.end())
	{
		iterator_network_Pm_iterm->second = "";
		iterator_network_Pm_iterm ++;
	}
}