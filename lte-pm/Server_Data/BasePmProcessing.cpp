#include "stdafx.h"
#include "BasePmProcessing.h"
//��ͼ��
#include "Server_DataDlg.h"

BasePmProcessing::BasePmProcessing(void)
{
	for (int i = 0 ; i < 6 ; i++)
	{
		CFile *a =new CFile();
		write_file_.push_back(a);
	}
}


BasePmProcessing::~BasePmProcessing(void)
{
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ��򿪴�����TXT�ļ�
//��    ����directory_path:·����file_name��ѹ���ļ�����
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::OpenPmFile(CString directory_path,CString file_name)
{
	CString temp = directory_path + "\\Pm_LTE_MAC_"+ file_name + ".txt";
	write_file_[0]->Open(directory_path + "\\Pm_LTE_MAC_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[1]->Open(directory_path + "\\Pm_LTE_RRC_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[2]->Open(directory_path + "\\Pm_LTE_PDCP_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[3]->Open(directory_path + "\\Pm_LTE_PHY_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[4]->Open(directory_path + "\\Pm_LTE_THO_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[5]->Open(directory_path + "\\Pm_LTE_TXT_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���cell�������ݷֽ�Ϊ�ļ�
//��    ����ObjType���ļ����ݣ�file_time���ļ�ʱ�䣻Pm_classification��cell���ܷ����׼
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::ProcessData(map_ObjType &ObjType,CString file_time,list<CString> *Pm_classification)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjTypeָʾ��,XML��������
	list_Object::iterator	v_pIterator_Object;							//list_Object������������ȫ������Map�б�
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter����������������ÿ��Map����������
	map_Counter::iterator	v_pIterator_File;							//map_Counter����������������ÿ��FileNameMap����������
	v_pIterator_ObjType = ObjType.begin();							    //��ȡ�������ݵĵ�����
	while (v_pIterator_ObjType != ObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType����Object��[����Object����Ϊ�գ�����]
		{
			v_pIterator_Object=v_pIterator_ObjType->second.begin();		        //��ã�Object��������ʼλ��
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//������Object��������ֹλ��
			{
				cell_key_ = file_time+","+v_pIterator_Object->v_sObject_ID;     //����ͷ��������,txt�����ļ����ÿ��ͷ��Ϣ
				WriteBasePm(Pm_classification,*v_pIterator_Object);
				v_pIterator_Object++;
			}
		}
		v_pIterator_ObjType++;	   
	}

	//��������Ԥ��
	Pm_warning_.file_time_ = file_time;
	Pm_warning_.CompareToBasePerformance(ObjType);
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���cell�������ݷֽ�Ϊ�ļ�
//��    ����ObjType���ļ����ݣ�file_time���ļ�ʱ�䣻Pm_classification��cell���ܷ����׼
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::ProcessHourData(map_ObjType &ObjType,CString file_time,list<CString> *Pm_classification)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjTypeָʾ��,XML��������
	list_Object::iterator	v_pIterator_Object;							//list_Object������������ȫ������Map�б�
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter����������������ÿ��Map����������
	map_Counter::iterator	v_pIterator_File;							//map_Counter����������������ÿ��FileNameMap����������
	v_pIterator_ObjType = ObjType.begin();							    //��ȡ�������ݵĵ�����
	while (v_pIterator_ObjType != ObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType����Object��[����Object����Ϊ�գ�����]
		{
			v_pIterator_Object=v_pIterator_ObjType->second.begin();		        //��ã�Object��������ʼλ��
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//������Object��������ֹλ��
			{
				cell_key_ = file_time+","+v_pIterator_Object->v_sObject_ID;     //����ͷ��������,txt�����ļ����ÿ��ͷ��Ϣ
				WriteBasePm(Pm_classification,*v_pIterator_Object);
				v_pIterator_Object++;
			}
		}
		v_pIterator_ObjType++;	   
	}

	//��������Ԥ��
	//Pm_warning_.file_time_ = file_time;
	//Pm_warning_.CompareToBasePerformance(ObjType);
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ����������������
//��    ����Pm_classification�������׼��file_object��С�����ܷ���
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::WriteBasePm(list<CString> *Pm_classification,File_Object &file_object)
{
	list<CString>::iterator Pm_classification_iterator;
	map<CString,CString>::iterator    Pm_iterator;
	CString write_context;
	for (int i = 0;i < write_file_.size();i ++)
	{
		write_context = cell_key_;
		Pm_classification_iterator = Pm_classification[i].begin();
		while (Pm_classification_iterator != Pm_classification[i].end())
		{
			Pm_iterator = file_object.v_mCounter.find(*Pm_classification_iterator);
			if (Pm_iterator != file_object.v_mCounter.end())
			{
				write_context += "," + Pm_iterator->second;
			} 
			else
			{
				write_context += ",";
			}
			Pm_classification_iterator ++;
		}
		if (Pm_classification_iterator == Pm_classification[i].end())
		{
			write_context+="\r\n";
			write_file_[i]->Write(write_context,write_context.GetLength());
		}
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ��ر�txt�ļ�
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::ClosePmFile()
{
	for (int iNum=0 ; iNum < write_file_.size() ; iNum++)//�ر�6���ı��ļ�
	{
		write_file_[iNum]->Close();
	}	
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------		
//�������ܣ�bulk insert�����ջ
//��    ����save_directory_path��bulk insert�ļ�·��,table_suffix�����׺,control_count :��������������;file_name���ļ�����;sql_stack:����bulkinsert���ջ
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BasePmProcessing::PushSqlIntoStack(CString save_directory_path,CString table_suffix,int control_count,CString file_name,stack<CString> &sql_stack)
{
	CString sTemp,sMsg,count;
	COleDateTime monthTime;
	count.Format("%d",control_count);
	sTemp="bulk insert Pm_LTE_PDCP" + table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_PDCP_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
	sTemp="bulk insert Pm_LTE_TXT" +  table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_TXT_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";	
	sql_stack.push(sTemp);	
	sTemp="bulk insert Pm_LTE_RRC" +  table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_RRC_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";	
	sql_stack.push(sTemp);	
	sTemp="bulk insert Pm_LTE_THO" +  table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_THO_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
	sTemp="bulk insert Pm_LTE_MAC" +  table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_MAC_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
	sTemp="bulk insert Pm_LTE_PHY" + table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_PHY_"+ file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
	sql_stack.push(sTemp);
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ���ʼ��
//-----------------------------------------------------------------------------------------------------------------
void BasePmProcessing::Init()
{
	Pm_warning_.Init();
	GetPmClassification(Pm_classification);
	GetBasePmType();
	GetCellID(hourCellID_cnt_);
	hour_Pm_object_.clear();
	list_Object temp;
	hour_Pm_object_.insert(pair<CString,list_Object>("1",temp));
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ���������
//-----------------------------------------------------------------------------------------------------------------
void BasePmProcessing::Updata()
{
	Pm_warning_.Init();
	GetCellID(hourCellID_cnt_);
}

//-----------------------------------------------------------------------------------------------------------------
//  ��������:���С����Ψһ����
//  ��    ��:hourCellID_cnt_ : С������뼰һ��Сʱ���ȳ��ִ���
//-----------------------------------------------------------------------------------------------------------------
void BasePmProcessing:: GetCellID(map<CString,int> &hourCellID_cnt_)
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

//------------------------------------------------------------------------------------------------------			
//�������ܣ�������ܷ���
//��    ��:Pm_classification:��ͬ���˳�򱣴��ڲ�ͬ��list��
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::GetPmClassification(list<CString> *Pm_classification)
{
	CString			     v_sTemp,v_sSql,temp;							    //��ʱ������SQLִ�У�
	CString			     v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//��Ӧ�ڱ�CON_System�еı�������
	map<CString,CString> map_temp;
	COleDateTime         tableTime;                                         //��ʱ��
	_RecordsetPtr v_pRecordset;											    //��¼��
	CMyADOConn v_cAdoConn;												    //���ݿ�ʵ��
	try
	{   
		for (int i = 0;i<=5;i++)
		{
			Pm_classification[i].clear();
		}
		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(1000);
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";     //��Pmԭʼ��������
		v_sSql += "SELECT * FROM CON_Pm_LTE ";
		v_sSql+=" order by ztype,ztype_1,zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//ִ��SELETE���										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			v_sTable_Key = v_cAdoConn.GetField_String("zTable_Key");	//zTable_Key
			//��������
			if (v_sTable_Key=="MAC")
			{
				Pm_classification[0].push_back(v_sPm);
			}
			else if(v_sTable_Key=="RRC")
			{
				Pm_classification[1].push_back(v_sPm);
			}
			else if(v_sTable_Key=="PDCP")
			{
				Pm_classification[2].push_back(v_sPm);
			}
			else if(v_sTable_Key=="PHY")
			{
				Pm_classification[3].push_back(v_sPm);
			}
			else if(v_sTable_Key=="THO")
			{
				Pm_classification[4].push_back(v_sPm);
			}
			else if(v_sTable_Key=="TXT")
			{
				Pm_classification[5].push_back(v_sPm);
			}
			else
			{
				v_sTemp= "��ȡPM���ƴ���"+v_sPm;
				theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	  //log
			}	
			v_pRecordset->MoveNext();									      //
		}
		v_cAdoConn.ExitConnect();										//�Ͽ�����
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//�Ͽ�����
		v_sTemp = (char *)e.Description();								//������Ϣ
		theApp.m_MyLog.WriteText("�쳣��"+v_sTemp+"  "+v_sSql+"\r\n");	//log
	}
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���û�����������
//------------------------------------------------------------------------------------------------------
void BasePmProcessing::GetBasePmType()
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
void BasePmProcessing::SaveHourPmData(map_ObjType &ObjType)
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
void BasePmProcessing::meanHourPmCount(map_ObjType &ObjType)
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

