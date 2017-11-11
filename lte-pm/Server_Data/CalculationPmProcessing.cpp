#include "stdafx.h"
#include "CalculationPmProcessing.h"
//��ͼ��
#include "Server_DataDlg.h"

CalculationPmProcessing::CalculationPmProcessing(void)
{
}


CalculationPmProcessing::~CalculationPmProcessing(void)
{
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ���ʼ��
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::Init()
{
	//��ȡС�����ܹ�ʽ
	GetFomula();                             
	Pm_warning_.Init();
	GetBasePmType();
	GetCellID(hourCellID_cnt_);
	hour_Pm_object_.clear();
	list_Object temp;
	hour_Pm_object_.insert(pair<CString,list_Object>("1",temp));
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ���ʼ��
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::Updata()
{
	//��ȡС�����ܹ�ʽ
	GetFomula();                             
	Pm_warning_.Init();
	GetCellID(hourCellID_cnt_);
}

//-----------------------------------------------------------------------------------------------------------------
//  ��������:���С����Ψһ����
//  ��    ��:hourCellID_cnt_ : С������뼰һ��Сʱ���ȳ��ִ���
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing:: GetCellID(map<CString,int> &hourCellID_cnt_)
{
	_RecordsetPtr v_pRecordset;											  //��¼��
	CMyADOConn v_cAdoConn;												  //���ݿ�ʵ��
	CString		   sql,sTemp,Key;							      //��ʱ����
	int            iTemp,value(0);
	try
	{
		hourCellID_cnt_.clear();
		sql="SELECT zCell_ID FROM LTE_Cell";
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)sql);		          //ִ��SELETE���	
		while(!v_pRecordset->adoEOF)
		{
			iTemp = v_cAdoConn.GetField_Int("zCell_ID");
			Key.Format("%d",iTemp);
			hourCellID_cnt_.insert(pair<CString,int>(Key,value));
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
//  �������ܣ������ݿ��ж�ȡ���ܴ���ʽ
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::GetFomula()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString v_sSql,formula,formula_name;
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	map_formula_.clear();                                               //�������
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);
	v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_This_Month";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���	
	while (!v_pRecordset->adoEOF)
	{
		formula_name = v_cAdoConn.GetField_String("zPm");			//��ʽ����
		formula = v_cAdoConn.GetField_String("zPm_Formula");			       //��ʽ����ֵ
		map_formula_.insert(pair<CString,CString> (formula_name,formula));
		v_pRecordset->MoveNext();
	}
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ�ʵ�����ܹ�ʽ�ϳ�
//  ��    ��:CSstringformula:�ϳɹ�ʽ StructFormula���ֽ⹫ʽ
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula)
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

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ�����һ���ӹ�ʽ���ܵ�TXT
//��    ����directory_path:·����file_name���ļ����ƣ�file_time �ļ�ʱ��
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::SaveFormulaPmToTXT(CString derictory_path,CString file_name,CString file_time)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	CString formula_performace="",cell_name;
	//�򿪹�ʽ�����ļ�
	formula_Pm_file_.Open(derictory_path + "\\Pm_LTE_Cal_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	iterator_performance_objtype = Pm_object_.begin();
	//��Cell��ʽ����д���ļ�
	while(iterator_performance_objtype != Pm_object_.end())
	{
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while(iterator_performance_object != iterator_performance_objtype->second.end())
		{
			cell_name = iterator_performance_object->v_sObject_ID;
			formula_performace = file_time+","+cell_name;
			iterator_map_formula = iterator_performance_object->v_mCounter.begin();
			while(iterator_map_formula != iterator_performance_object->v_mCounter.end())
			{
				formula_performace += "," + iterator_map_formula->second;
				iterator_map_formula++;
			}
			formula_performace += "\r\n";
			formula_Pm_file_.Write(formula_performace,formula_performace.GetLength());
			formula_performace = "";
			iterator_performance_object++;
		}
		iterator_performance_objtype++;
	}
	//�رչ�ʽ�����ļ�
	formula_Pm_file_.Close();
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ�ʵ�����ܹ�ʽ�ֽ�
//  ��    ��:v_sFormula��������㹫ʽ v_mFormula���ֽ⹫ʽ
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula)
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
//  �������ܣ�ʵ�����ܼ��㣬����ֵ������ȫ�ֱ���performance_object_�С���С��Ԥ����
//  ��   ����ObjType:�ӿ����ݣ�file_time���ļ�ʱ��
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::CalculateHourPerformance(CString file_time)
{
	//����Ϊʵ�ʹ���
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//���㹫ʽ����ʽԭ��;��������;����ֵ
	mPM_Formula::iterator   iterator_decompose_formula;                 //�ֽ⹫ʽ������
	map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
	list_Object::iterator	iterator_Object;							//countС��������
	map_Counter::iterator	iterator_counter;						    //count���ܵ�����
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	CMyFormula calculate_formula;//��������ָ����
	char * Pcharformula = NULL;//��������ָ������
	float number_result = 0;//����ֵ
	int icount = -1;
	mPM_Formula  decompose_formula;//�ֽ⹫ʽ
	//ѭ����������滻����ֵ������
	//�ļ�ѭ��
	Pm_object_ = hour_Pm_object_;
	iterator_objtype = hour_Pm_object_.begin();
	iterator_performance_objtype =Pm_object_.begin();
	while (iterator_objtype != hour_Pm_object_.end())
	{
		//С��ѭ��
		iterator_Object = iterator_objtype->second.begin();
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while (iterator_Object != iterator_objtype->second.end())
		{
			icount ++;
			//���ԭmap�е�count����
			iterator_performance_object->v_mCounter.clear();
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
						iterator_counter = iterator_Object->v_mCounter.find(iterator_decompose_formula->v_sPM_Formula);
						if (iterator_counter != iterator_Object->v_mCounter.end())
						{
							iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						} 
						else
						{
							//�쳣��0
							iterator_decompose_formula->v_sPM_Formula = "0";
						}
					}
					iterator_decompose_formula++;
				}
				//��ԭ���㹫ʽ
				CompositionFormula(&formula_result, decompose_formula);
				//���ܼ���
				Pcharformula = formula_result.GetBuffer(0);
				number_result = calculate_formula.simcalc(Pcharformula);
				performace_cstring_value.Format("%f", number_result);
				//��������
				iterator_performance_object->v_mCounter.insert(pair<CString, CString>(performace_name, performace_cstring_value));
				//Reset����
				formula_result = "";
				decompose_formula.clear();
				iterator_map_formula++;
			}
			iterator_Object++;
   			iterator_performance_object++;
		}
		iterator_objtype++;
		iterator_performance_objtype++;
	}
	//С��Ԥ��
	//Pm_warning_.file_time_ = file_time;
	//Pm_warning_.CompareToFormulaPerformance(Pm_object_);
	//Reset����
	decompose_formula.clear();
	hour_Pm_object_.clear();
	list_Object temp;
	hour_Pm_object_.insert(pair<CString,list_Object>("1",temp));
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ�ʵ��15�����������ܼ���
//  ��   ����ObjType:�ӿ����ݣ�file_time���ļ�ʱ��
//-----------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::CalculatePerformance(map_ObjType &ObjType,CString file_time)
{
	//����Ϊʵ�ʹ���
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//���㹫ʽ����ʽԭ��;��������;����ֵ
	mPM_Formula::iterator   iterator_decompose_formula;                 //�ֽ⹫ʽ������
	map_ObjType::iterator	iterator_objtype;						    //count�ļ�������
	list_Object::iterator	iterator_Object;							//countС��������
	map_Counter::iterator	iterator_counter;						    //count���ܵ�����
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	CMyFormula calculate_formula;//��������ָ����
	char * Pcharformula = NULL;//��������ָ������
	float number_result = 0;//����ֵ
	int icount = -1;
	mPM_Formula  decompose_formula;//�ֽ⹫ʽ
	//ѭ����������滻����ֵ������
	//�ļ�ѭ��
	Pm_object_ = ObjType;
	iterator_objtype = ObjType.begin();
	iterator_performance_objtype =Pm_object_.begin();
	while (iterator_objtype != ObjType.end())
	{
		//С��ѭ��
		iterator_Object = iterator_objtype->second.begin();
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while (iterator_Object != iterator_objtype->second.end())
		{
			icount ++;
			//���ԭmap�е�count����
			iterator_performance_object->v_mCounter.clear();
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
						iterator_counter = iterator_Object->v_mCounter.find(iterator_decompose_formula->v_sPM_Formula);
						if (iterator_counter != iterator_Object->v_mCounter.end())
						{
							iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
						} 
						else
						{
							//�쳣��0
							iterator_decompose_formula->v_sPM_Formula = "0";
						}
					}
					iterator_decompose_formula++;
				}
				//��ԭ���㹫ʽ
				CompositionFormula(&formula_result, decompose_formula);
				//���ܼ���
				Pcharformula = formula_result.GetBuffer(0);
				number_result = calculate_formula.simcalc(Pcharformula);
				performace_cstring_value.Format("%f", number_result);
				//��������
				iterator_performance_object->v_mCounter.insert(pair<CString, CString>(performace_name, performace_cstring_value));
				//Reset����
				formula_result = "";
				decompose_formula.clear();
				iterator_map_formula++;
			}
			iterator_Object++;
			iterator_performance_object++;
		}
		iterator_objtype++;
		iterator_performance_objtype++;
	}
	//С��Ԥ��
	Pm_warning_.file_time_ = file_time;
	Pm_warning_.CompareToFormulaPerformance(Pm_object_);
	//Reset����
	decompose_formula.clear();
	ObjType.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------		
//�������ܣ�bulk insert�����ջ
//��    ����save_directory_path��bulk insert�ļ�·��,table_suffix�����׺,control_count :��������������;file_name���ļ�����;sql_stack:����bulkinsert���ջ
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::PushSqlIntoStack(CString save_directory_path,CString table_suffix,int control_count,CString file_name,stack<CString> &sql_stack)
{
	CString sTemp,sMsg,count;
	COleDateTime monthTime;
	count.Format("%d",control_count);
	sTemp="bulk insert Pm_LTE_Cal" + table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_Cal_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���û�����������
//------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::GetBasePmType()
{
	int				v_iItem,v_iID=0,value;									//KPI����
	CString			v_sTemp,v_sSql,v_sLog_Key;							//��ʱ����
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//��ʱ����
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	basePm_type_.clear();
	try
	{											//��Ŀѡ����Ч

		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";
		v_sSql += "SELECT zPm,zHour FROM CON_Pm_LTE ";
		v_sSql+=" order by ztype,ztype_1,zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			value = v_cAdoConn.GetField_Int("zHour");                   //zHour
			basePm_type_.insert(pair<CString,int>(v_sPm,value));
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
//�������ܣ�����4�����Ȼ�������
//------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::SaveHourPmData(map_ObjType &ObjType)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjTypeָʾ��,XML��������
	map_ObjType::iterator	v_pIterator_hour_ObjType;				    //list_ObjTypeָʾ��,����Сʱ����
	list_Object::iterator	v_pIterator_Object;							//list_Object������������ȫ������Map�б�
	list_Object::iterator	v_pIterator_hour_Object;				    //list_Object������������С��
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter����������������ÿ��Map����������
	map_Counter::iterator	v_pIterator_hour_Counter;				    //map_Counter��������������������������
	map<CString,int>::iterator	v_pIterator_type;						//map_Counter������
	map<CString,int>::iterator	v_pIterator_cellID;						//map_Counter������
	CString cell_name;													//С������
	float value,value1;                                                 //����ֵ
	int   Pm_type;
	File_Object  temp;                                                  //��ʱ����
	v_pIterator_ObjType = ObjType.begin();		                        //��ȡ�������ݵĵ�����
	while (v_pIterator_ObjType != ObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType����Object��[����Object����Ϊ�գ�����]
		{
			v_pIterator_Object = v_pIterator_ObjType->second.begin();		    //��ã�Object��������ʼλ��
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//������Object��������ֹλ��
			{
				cell_name = v_pIterator_Object->v_sObject_ID;                   //С������
				v_pIterator_cellID = hourCellID_cnt_.find(cell_name);
				if(v_pIterator_cellID == hourCellID_cnt_.end()){
					v_pIterator_Object++;
					continue;
				}
				v_pIterator_cellID->second++;
				v_pIterator_hour_ObjType = hour_Pm_object_.find("1");
				if (v_pIterator_hour_ObjType != hour_Pm_object_.end())
				{
					v_pIterator_hour_Object = v_pIterator_hour_ObjType->second.begin(); 
					while(v_pIterator_hour_Object != v_pIterator_hour_ObjType->second.end())
					{
						if (v_pIterator_hour_Object->v_sObject_ID == cell_name)
						{
							v_pIterator_hour_Counter = v_pIterator_hour_Object->v_mCounter.begin();
							//v_pIterator_Counter = v_pIterator_Object->v_mCounter.begin();
							while (v_pIterator_hour_Counter != v_pIterator_hour_Object->v_mCounter.end())
							{
								v_pIterator_Counter = v_pIterator_Object->v_mCounter.find(v_pIterator_hour_Counter->first);
								if(v_pIterator_Counter == v_pIterator_Object->v_mCounter.end()){
									v_pIterator_hour_Counter ++;
									continue;
								}
								v_pIterator_type = basePm_type_.find(v_pIterator_hour_Counter->first);
								Pm_type = v_pIterator_type->second;
								switch (Pm_type)
								{
								case 0:
									value = _ttof(v_pIterator_hour_Counter->second);
									value += _ttof(v_pIterator_Counter->second);
									break;
								case 1:							
									try{
										value = _ttof(v_pIterator_hour_Counter->second);
										value += _ttof(v_pIterator_Counter->second);
										break;
									}
									catch(...){
										//Log������ʼ
										theApp.m_MyLog.WriteText(cell_name+"********************\r\n");		//Log
									}							
								case 2:
									value = _ttof(v_pIterator_hour_Counter->second);
									value1= _ttof(v_pIterator_Counter->second);
									if(value > value1)
									{
										value = value;
									}
									else
									{
										value = value1;
									}
									break;
								case 3:
									value = _ttof(v_pIterator_hour_Counter->second);
									value1= _ttof(v_pIterator_Counter->second);
									if(value < value1)
									{
										value = value;
									}
									else
									{
										value = value1;
									}
									break;
								default:
									value = _ttof(v_pIterator_hour_Counter->second);
									value += _ttof(v_pIterator_Counter->second);
									break;
								}
								v_pIterator_hour_Counter->second.Format("%f",value);
								//v_pIterator_Counter ++;
								v_pIterator_hour_Counter ++;
							}
							break;
						}
						v_pIterator_hour_Object ++;
					}
					if (v_pIterator_hour_Object == v_pIterator_hour_ObjType->second.end())
					{
						temp.v_mCounter = v_pIterator_Object->v_mCounter;
						temp.v_sObject_ID = v_pIterator_Object->v_sObject_ID;
						temp.v_sObject_DN = v_pIterator_Object->v_sObject_DN;
						v_pIterator_hour_Counter = temp.v_mCounter.begin();
						while (v_pIterator_hour_Counter != temp.v_mCounter.end())
						{
							v_pIterator_type = basePm_type_.find(v_pIterator_hour_Counter->first);
							Pm_type = v_pIterator_type->second;
							switch (Pm_type)
							{
							case 0:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							case 1:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							case 2:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							case 3:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							default:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							}
							v_pIterator_hour_Counter->second.Format("%f",value);
							v_pIterator_hour_Counter ++;
						}
						v_pIterator_hour_ObjType->second.push_back(temp);
					}
				} 
				v_pIterator_Object++;
			}
		}
		v_pIterator_ObjType++;
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�Сʱ����count��ƽ��ֵ
//------------------------------------------------------------------------------------------------------
void CalculationPmProcessing::meanHourPmCount(map_ObjType &ObjType)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjTypeָʾ��,XML��������
	list_Object::iterator	v_pIterator_Object;							//list_Object������������ȫ������Map�б�
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter����������������ÿ��Map����������
	map<CString,int>::iterator	v_pIterator_type;						//map_Counter������
	map<CString,int>::iterator	v_pIterator_cellID;						//map_Counter������
	CString cell_name;													//С������
	float value,value1;                                                 //����ֵ
	int   Pm_type;
	File_Object  temp;                                                  //��ʱ����
	v_pIterator_ObjType = ObjType.begin();		                        //��ȡ�������ݵĵ�����
	while (v_pIterator_ObjType != ObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType����Object��[����Object����Ϊ�գ�����]
		{
			v_pIterator_Object = v_pIterator_ObjType->second.begin();		    //��ã�Object��������ʼλ��
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//������Object��������ֹλ��
			{
				cell_name = v_pIterator_Object->v_sObject_ID;                   //С������
				v_pIterator_Counter = v_pIterator_Object->v_mCounter.begin();
				while(v_pIterator_Counter!=v_pIterator_Object->v_mCounter.end()){
					v_pIterator_type = basePm_type_.find(v_pIterator_Counter->first);
					Pm_type = v_pIterator_type->second;
					switch (Pm_type)
					{
					case 1:
						try{
							v_pIterator_cellID = hourCellID_cnt_.find(cell_name);
							if(v_pIterator_cellID == hourCellID_cnt_.end()){
							break;
							}
							if(v_pIterator_cellID->second == 0){
							value = 0;
							v_pIterator_Counter->second.Format("%f",value);
							}
							value = _ttof(v_pIterator_Counter->second)/v_pIterator_cellID->second;
							v_pIterator_Counter->second.Format("%f",value);
							break;
						}
						catch(...){
							//Log������ʼ
							theApp.m_MyLog.WriteText(cell_name+"********************\r\n");		//Log
						}
					default:
						break;
					}
					v_pIterator_Counter++;
				}
				v_pIterator_Object++;
			}
		}
		v_pIterator_ObjType++;
	}
}
//------------------------------------------------------------------------------------------------------
//	End
//------------------------------------------------------------------------------------------------------
