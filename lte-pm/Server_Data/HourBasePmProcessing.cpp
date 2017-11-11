#include "stdafx.h"
#include "HourBasePmProcessing.h"
//��ͼ��
#include "Server_DataDlg.h"
HourBasePmProcessing::HourBasePmProcessing(void)
{
}


HourBasePmProcessing::~HourBasePmProcessing(void)
{
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ��򿪴������6��TXT�ļ�
//��    ����directory_path:·����file_name���ļ�����
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::OpenPmFile(CString directory_path,CString file_name)
{
	write_file_[0].Open(directory_path + "\\Hour_Pm_LTE_MAC_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[1].Open(directory_path + "\\Hour_Pm_LTE_RRC_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[2].Open(directory_path + "\\Hour_Pm_LTE_PDCP_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[3].Open(directory_path + "\\Hour_Pm_LTE_PHY_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[4].Open(directory_path + "\\Hour_Pm_LTE_THO_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	write_file_[5].Open(directory_path + "\\Hour_Pm_LTE_TXT_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ���cell�������ݷֽ�Ϊ�����ļ�
//��    ����file_time���ļ�ʱ�䣻Pm_classification��cell���ܷ����׼
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::ProcessData(CString file_time,list<CString> *Pm_classification)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjTypeָʾ��,XML��������
	list_Object::iterator	v_pIterator_Object;							//list_Object������������ȫ������Map�б�
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter����������������ÿ��Map����������
	map_Counter::iterator	v_pIterator_File;							//map_Counter����������������ÿ��FileNameMap����������
	int	iMapCout(0),iCount(0);                                          //��¼CELL��������
	v_pIterator_ObjType = Pm_hour_object_.begin();		                //��ȡ�������ݵĵ�����
	while (v_pIterator_ObjType != Pm_hour_object_.end())
	{   		
			if(!v_pIterator_ObjType->second.empty())						        //ObjType����Object��[����Object����Ϊ�գ�����]
			{
				iMapCout += v_pIterator_ObjType->second.size();
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
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ����������������
//��    ����Pm_classification�������׼��file_object��С�����ܷ���
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::WriteBasePm(list<CString> *Pm_classification,File_Object &file_object)
{
	list<CString>::iterator Pm_classification_iterator;
	map<CString,CString>::iterator    Pm_iterator;
	for (int i = 0;i <= 5;i ++)
	{
		write_context_[i] = cell_key_;
		Pm_classification_iterator = Pm_classification[i].begin();
		while (Pm_classification_iterator != Pm_classification[i].end())
		{
			Pm_iterator = file_object.v_mCounter.find(*Pm_classification_iterator);
			if (Pm_iterator != file_object.v_mCounter.end())
			{
				write_context_[i] += "," + Pm_iterator->second;
			} 
			else
			{
				write_context_[i] += ",";
			}
			Pm_classification_iterator ++;
		}
		if (Pm_classification_iterator == Pm_classification[i].end())
		{
			write_context_[i]+="\r\n";
			write_file_[i].Write(write_context_[i],write_context_[i].GetLength());
		}
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ�����227����������	
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::WritePm227(int iCount,CString sPM)
{
	if (0==iCount)
	{							
		write_context_[1]=cell_key_;
		write_context_[1]+=","+sPM;
	}
	else if (0<iCount&&iCount<17)
	{
		write_context_[1]+=","+sPM;
	}
	else if (iCount==17)
	{
		write_context_[1]+=","+sPM;
		write_context_[1]+="\r\n";
		write_file_[1].Write(write_context_[1],write_context_[1].GetLength());
		write_context_[1]="";
	}	
	else if (iCount==18)
	{
		write_context_[2]=cell_key_;
		write_context_[2]+=","+sPM;
	}
	else if (18<iCount&&iCount<=62)
	{
		if (iCount==50)  //���ܱ���Ϊ��
		{
			write_context_[2]+=",";
		} 
		else
		{
			write_context_[2]+=","+sPM;
		}
	}
	else if (iCount==63)
	{
		write_context_[3]=cell_key_;
		write_context_[3]+=","+sPM;
	}
	else if (63<iCount&&iCount<125)
	{
		if (iCount==64||iCount==66||iCount==72||iCount==74||iCount==81||iCount==83||iCount==90||iCount==92||iCount==94||iCount==96||iCount==98||iCount==100||iCount==102) //���ܱ���Ϊ��
		{
			write_context_[3]+=",";
		} 
		else
		{
			write_context_[3]+=","+sPM;
		}
	}
	else if (iCount==125)
	{
		write_context_[3]+=","+sPM;
		write_context_[3]+="\r\n";
		write_file_[3].Write(write_context_[3],write_context_[3].GetLength());
		write_context_[3]="";
	}
	else if (iCount==126)
	{
		write_context_[4]=cell_key_;
		write_context_[4]+=","+sPM;
	}
	else if (126<iCount&&iCount<167)
	{
		if (iCount==142||iCount==144||iCount==146||iCount==149||iCount==151||iCount==153||iCount==155||iCount==156||iCount==157||iCount==164||iCount==165||iCount==167)  //���ܱ���Ϊ��
		{
			write_context_[4]+=",";
		} 
		else
		{
			write_context_[4]+=","+sPM;
		}
	}
	else if (iCount==167)
	{
		write_context_[4]+=",";
		write_context_[4]+="\r\n";
		write_file_[4].Write(write_context_[4],write_context_[4].GetLength());
		write_context_[4]="";
	}
	else if (iCount==168)
	{
		write_context_[5]=cell_key_;
		write_context_[5]+=","+sPM;
	}
	else if (168<iCount&&iCount<=169)
	{
		write_context_[5]+=","+sPM;
	}
	else if (iCount==170)
	{
		write_context_[0]=cell_key_;
		write_context_[0]+=","+sPM;
	}
	else if (170<iCount&&iCount<191)
	{
		write_context_[0]+=","+sPM;
	}
	else if (iCount==191)
	{
		write_context_[0]+=","+sPM;
		write_context_[0]+="\r\n";
		write_file_[0].Write(write_context_[0],write_context_[0].GetLength());
		write_context_[0]="";
	}
	else if (192<=iCount&&iCount<=211)
	{
		write_context_[5]+=","+sPM;
	}
	else if (212<=iCount&&iCount<231)
	{
		write_context_[2]+=","+sPM;
	}
	else if (iCount==231)
	{
		write_context_[2]+=","+sPM;
		write_context_[2]+="\r\n";
		write_file_[2].Write(write_context_[2],write_context_[2].GetLength());
		write_context_[2]="";
	}
	else if (232<=iCount&&iCount<253)
	{
		if (iCount==241)     //���ܱ���Ϊ��
		{
			write_context_[5]+=",";
		} 
		else
		{
			write_context_[5]+=","+sPM;
		}
	}
	else if (iCount==253)
	{
		write_context_[5]+=","+sPM;
		write_context_[5]+="\r\n";
		write_file_[5].Write(write_context_[5],write_context_[5].GetLength());
		write_context_[5]="";
	}	
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ�����254����������
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::WritePm254(int iCount,CString sPM)
{
	if (0==iCount)
	{							
		write_context_[1]=cell_key_;
		write_context_[1]+=","+sPM;
	}
	else if (0<iCount&&iCount<17)
	{
		write_context_[1]+=","+sPM;
	}
	else if (iCount==17)
	{
		write_context_[1]+=","+sPM;
		write_context_[1]+="\r\n";
		write_file_[1].Write(write_context_[1],write_context_[1].GetLength());
		write_context_[1]="";
	}	
	else if (iCount==18)
	{
		write_context_[2]=cell_key_;
		write_context_[2]+=","+sPM;
	}
	else if (18<iCount&&iCount<=62)
	{
		write_context_[2]+=","+sPM;
	}
	else if (iCount==63)
	{
		write_context_[3]=cell_key_;
		write_context_[3]+=","+sPM;
	}
	else if (63<iCount&&iCount<125)
	{
		write_context_[3]+=","+sPM;
	}
	else if (iCount==125)
	{
		write_context_[3]+=","+sPM;
		write_context_[3]+="\r\n";
		write_file_[3].Write(write_context_[3],write_context_[3].GetLength());
		write_context_[3]="";
	}
	else if (iCount==126)
	{
		write_context_[4]=cell_key_;
		write_context_[4]+=","+sPM;
	}
	else if (126<iCount&&iCount<167)
	{
		write_context_[4]+=","+sPM;
	}
	else if (iCount==167)
	{
		write_context_[4]+=","+sPM;
		write_context_[4]+="\r\n";
		write_file_[4].Write(write_context_[4],write_context_[4].GetLength());
		write_context_[4]="";
	}
	else if (iCount==168)
	{
		write_context_[5]=cell_key_;
		write_context_[5]+=","+sPM;
	}
	else if (168<iCount&&iCount<=169)
	{
		write_context_[5]+=","+sPM;
	}
	else if (iCount==170)
	{
		write_context_[0]=cell_key_;
		write_context_[0]+=","+sPM;
	}
	else if (170<iCount&&iCount<191)
	{
		write_context_[0]+=","+sPM;
	}
	else if (iCount==191)
	{
		write_context_[0]+=","+sPM;
		write_context_[0]+="\r\n";
		write_file_[0].Write(write_context_[0],write_context_[0].GetLength());
		write_context_[0]="";
	}
	else if (192<=iCount&&iCount<=211)
	{
		write_context_[5]+=","+sPM;
	}
	else if (212<=iCount&&iCount<231)
	{
		write_context_[2]+=","+sPM;
	}
	else if (iCount==231)
	{
		write_context_[2]+=","+sPM;
		write_context_[2]+="\r\n";
		write_file_[2].Write(write_context_[2],write_context_[2].GetLength());
		write_context_[2]="";
	}
	else if (232<=iCount&&iCount<253)
	{
		write_context_[5]+=","+sPM;
	}
	else if (iCount==253)
	{
		write_context_[5]+=","+sPM;
		write_context_[5]+="\r\n";
		write_file_[5].Write(write_context_[5],write_context_[5].GetLength());
		write_context_[5]="";
	}
}
//------------------------------------------------------------------------------------------------------			
//�������ܣ��ر�����txt�ļ�
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::ClosePmFile()
{
	for (int iNum=0;iNum<6;iNum++)//�ر�6���ı��ļ�
	{
		write_file_[iNum].Close();
	}	
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------			
//�������ܣ�6��TXT�ļ������ϴ����ݿ⡾����:sFileTime�ļ�ʱ�䡿
//��    ����sFileTime��log�ļ�ʱ�䣻save_directory_path��bulk insert�ļ�·��,CServer_DataDlg: ������ͼ����,table_suffix�����׺,control_count :����������������XML_file_name��ѹ���ļ����ƣ�file_time �ļ�ʱ��
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::UploadingHourData(CString sFileTime,CString save_directory_path,CServer_DataDlg* pDlg,CString table_suffix,int control_count,CString XML_file_name,CString file_time)
{
	CString sTemp,sMsg,sTime,count;
	COleDateTime monthTime;
	count.Format("%d",control_count);
	sFileTime.Replace("-","");
	sTime = sFileTime.Left(6);
	int iNum;
	for (iNum=0;iNum<6;iNum++)
	{
		switch (iNum)
		{
		case 0:
			sTemp="bulk insert Pm_LTE_PDCP_" + sTime + table_suffix + " FROM '"+ save_directory_path +"\\Hour_Pm_LTE_PDCP_"+ XML_file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
			if(!pDlg->My_ExecuteSQL(sTemp))
			{
				if(!pDlg->My_ExecuteSQL(sTemp))
				{
					sMsg  = sFileTime+"Pm_LTE_PDCP�ļ��ϴ�ʧ��";		
					pDlg->My_ShowList_Bug("���ݿ��쳣","���ݿ��쳣��"+sMsg);		//��ʾ���쳣
					sTemp=sFileTime;
					sTemp.Replace(":","");
					//CopyFile(save_directory_path +"\\Hour_Pm_LTE_PDCP.txt",save_directory_path +"\\"+sTemp+"Hour_Pm_LTE_PDCP.txt",false);
				}
				break;
			}			
			break;
		case 1:
			sTemp="bulk insert Pm_LTE_TXT_" + sTime + table_suffix + " FROM '"+ save_directory_path +"\\Hour_Pm_LTE_TXT_"+ XML_file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
			if(!pDlg->My_ExecuteSQL(sTemp))
			{
				if(!pDlg->My_ExecuteSQL(sTemp))
				{
					sMsg  = sFileTime+"Pm_LTE_TXT�ļ��ϴ�ʧ��";		
					pDlg->My_ShowList_Bug("���ݿ��쳣","���ݿ��쳣��"+sMsg);		//��ʾ���쳣
					sTemp=sFileTime;
					sTemp.Replace(":","");
					//CopyFile(save_directory_path +"\\Hour_Pm_LTE_TXT.txt",save_directory_path +"\\"+sTemp+"Hour_Pm_LTE_TXT.txt",false);
				}
				break;
			}
			break;
		case 2:
			sTemp="bulk insert Pm_LTE_RRC_" + sTime + table_suffix + " FROM '"+ save_directory_path +"\\Hour_Pm_LTE_RRC_"+ XML_file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
			if(!pDlg->My_ExecuteSQL(sTemp))
			{
				if(!pDlg->My_ExecuteSQL(sTemp))
				{
					sMsg  = sFileTime+"Pm_LTE_RRC�ļ��ϴ�ʧ��";		
					pDlg->My_ShowList_Bug("���ݿ��쳣","���ݿ��쳣��"+sMsg);		//��ʾ���쳣
					sTemp=sFileTime;
					sTemp.Replace(":","");
					//CopyFile(save_directory_path +"\\Hour_Pm_LTE_RRC.txt",save_directory_path +"\\"+sTemp+"Hour_Pm_LTE_RRC.txt",false);
				}
				break;
			}
			break;
		case 3:
			sTemp="bulk insert Pm_LTE_THO_" + sTime + table_suffix + " FROM '"+ save_directory_path +"\\Hour_Pm_LTE_THO_"+ XML_file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
			if(!pDlg->My_ExecuteSQL(sTemp))
			{
				if(!pDlg->My_ExecuteSQL(sTemp))
				{
					sMsg  = sFileTime+"Pm_LTE_THO�ļ��ϴ�ʧ��";		
					pDlg->My_ShowList_Bug("���ݿ��쳣","���ݿ��쳣��"+sMsg);		//��ʾ���쳣
					sTemp=sFileTime;
					sTemp.Replace(":","");
					//CopyFile(save_directory_path +"\\Hour_Pm_LTE_THO.txt",save_directory_path +"\\"+sTemp+"Hour_Pm_LTE_THO.txt",false);
				}
				break;
			}
			break;
		case 4:
			sTemp="bulk insert Pm_LTE_MAC_" + sTime + table_suffix + " FROM '"+ save_directory_path +"\\Hour_Pm_LTE_MAC_"+ XML_file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
			if(!pDlg->My_ExecuteSQL(sTemp))
			{
				if(!pDlg->My_ExecuteSQL(sTemp))
				{
					sMsg  = sFileTime+"Pm_LTE_MAC�ļ��ϴ�ʧ��";		
					pDlg->My_ShowList_Bug("���ݿ��쳣","���ݿ��쳣��"+sMsg);		//��ʾ���쳣
					sTemp=sFileTime;
					sTemp.Replace(":","");
					//CopyFile(save_directory_path +"\\Hour_Pm_LTE_MAC.txt",save_directory_path +"\\"+sTemp+"Hour_Pm_LTE_MAC.txt",false);
				}
				break;
			}
			break;
		case 5:
			sTemp="bulk insert Pm_LTE_PHY_" + sTime + table_suffix + " FROM '"+ save_directory_path +"\\Hour_Pm_LTE_PHY_"+ XML_file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 , BATCHSIZE = " + count + ")";
			if(!pDlg->My_ExecuteSQL(sTemp))
			{
				if(!pDlg->My_ExecuteSQL(sTemp))
				{
					sMsg  = sFileTime+"Pm_LTE_PHY�ļ��ϴ�ʧ��";		
					pDlg->My_ShowList_Bug("���ݿ��쳣","���ݿ��쳣��"+sMsg);		//��ʾ���쳣
					sTemp=sFileTime;
					sTemp.Replace(":","");
					//CopyFile(save_directory_path +"\\Hour_Pm_LTE_PHY.txt",save_directory_path +"\\"+sTemp+"Hour_Pm_LTE_PHY.txt",false);
				}
				break;
			}
			break;
		}		
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ�ʵ�����ܹ�ʽ�ֽ�
//  ��    ��:v_sFormula��������㹫ʽ v_mFormula���ֽ⹫ʽ
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula)
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
void HourBasePmProcessing::CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula)
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
//�������ܣ�����һСʱ���ȹ�ʽ���ܵ�TXT
//��    ����directory_path:·����XML_file_name���ļ����ƣ�file_time �ļ�ʱ��
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::SaveFormulaPmToTXT(CString derictory_path,CString file_name,CString file_time)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //�����ļ�������
	list_Object::iterator	iterator_performance_object;				//����С��������
	map_Counter::iterator	iterator_map_formula;						//���ܹ�ʽ������
	CString formula_performace="",cell_name;
	//�򿪹�ʽ�����ļ�
	formula_Pm_file_.Open(derictory_path+"\\Pm_LTE_Cal_Hour_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
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
//  �������ܣ�ʵ�����ܼ��㣻����ֵ������ȫ�ֱ���performance_object_�У���С��Ԥ��
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::CalculatePerformance(CString time)
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
	bool performance_flag = false;//����ֵƥ���ǩ
	float number_result = 0;//����ֵ
	mPM_Formula  decompose_formula;//�ֽ⹫ʽ
	//ѭ����������滻����ֵ������
	//�ļ�ѭ��
	Pm_object_ = Pm_hour_object_;
	iterator_objtype = Pm_hour_object_.begin();
	iterator_performance_objtype =Pm_object_.begin();
	while (iterator_objtype != Pm_hour_object_.end())
	{
		//С��ѭ��
		iterator_Object = iterator_objtype->second.begin();
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while (iterator_Object != iterator_objtype->second.end())
		{
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
						iterator_counter = iterator_Object->v_mCounter.begin();
						performance_flag = false;
						while (iterator_counter != iterator_Object->v_mCounter.end() && (!performance_flag))
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
	Pm_warning_.file_time_ = time;
	Pm_warning_.CompareToFormulaPerformance(Pm_object_);
	//Reset����
	decompose_formula.clear();
}

//-----------------------------------------------------------------------------------------------------------------
//  �������ܣ������ݿ��ж�ȡ���ܴ���ʽ
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::GetFomula()
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
//  �������ܣ���ʼ��
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::Init()
{
	Pm_warning_.Init();
	GetFomula();
	GetBase254PmType();
	Pm_hour_object_.clear();
	list_Object temp;
	Pm_hour_object_.insert(pair<CString,list_Object>("1",temp));
	Pm_hour_object_.insert(pair<CString,list_Object>("2",temp));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------			
//�������ܣ����ܹ�ʽ�ļ������ϴ�Сʱ�������ݿ�
//��   ����sFileTime���ļ�ʱ�䣻save_directory_path��bulk insert�ļ�·����CServer_DataDlg: ������ͼ����,table_suffix�����׺,control_count :����������������XML_file_name��ѹ���ļ����ƣ�file_time �ļ�ʱ��
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::UploadingHourFormulaPm(CString sFileTime,CString save_directory_path,CServer_DataDlg* pDlg,CString table_suffix,int control_count,CString XML_file_name,CString file_time)
{
	CString sTemp,sMsg,sTime,count;
	COleDateTime monthTime;
	count.Format("%d",control_count);
	sFileTime.Replace("-","");
	sTime = sFileTime.Left(6);
	sTemp="bulk insert Pm_LTE_Cal_" + sTime + table_suffix + " FROM '"+ save_directory_path +"\\Pm_LTE_Cal_Hour_"+ XML_file_name + ".txt' WITH ( FIELDTERMINATOR =',', FIRSTROW = 1 ,BATCHSIZE = " + count + ")";
	if(!pDlg->My_ExecuteSQL(sTemp))
	{
		if(!pDlg->My_ExecuteSQL(sTemp))
		{
			sMsg  = sFileTime+"Pm_LTE_Cal�ļ��ϴ�ʧ��";		
			pDlg->My_ShowList_Bug("���ݿ��쳣","���ݿ��쳣��"+sMsg);		//��ʾ���쳣
			sTemp=sFileTime;
			sTemp.Replace(":","");
			//CopyFile(save_directory_path+"\\Pm_LTE_Cal_Hour.txt",save_directory_path+"\\"+sTemp+"Pm_LTE_Cal_Hour.txt",false);
		}
	}			

}
//------------------------------------------------------------------------------------------------------			
//�������ܣ�����4�����Ȼ�������
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::SaveHourPmData(CMyFile_XML &v_cFile_XML)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjTypeָʾ��,XML��������
	map_ObjType::iterator	v_pIterator_hour_ObjType;				    //list_ObjTypeָʾ��,����Сʱ����
	list_Object::iterator	v_pIterator_Object;							//list_Object������������ȫ������Map�б�
	list_Object::iterator	v_pIterator_hour_Object;				    //list_Object������������С��
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter����������������ÿ��Map����������
	map_Counter::iterator	v_pIterator_hour_Counter;				    //map_Counter��������������������������
	map<CString,int>::iterator	v_pIterator_type;							//map_Counter��������254��������
	CString cell_name;      //С������
	object_type_ = false;                                               //�ж�254����227����������
	float value,value1;                                                        //����ֵ
	int   Pm_type;
	File_Object  temp;                                                  //��ʱ����
	v_pIterator_ObjType = v_cFile_XML.v_mObjType.begin();		        //��ȡ�������ݵĵ�����
	while (v_pIterator_ObjType != v_cFile_XML.v_mObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType����Object��[����Object����Ϊ�գ�����]
		{
			v_pIterator_Object = v_pIterator_ObjType->second.begin();		    //��ã�Object��������ʼλ��
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//������Object��������ֹλ��
			{
				cell_name = v_pIterator_Object->v_sObject_ID;                   //С������
				v_pIterator_hour_ObjType = Pm_hour_object_.find("1");
				if (v_pIterator_hour_ObjType != Pm_hour_object_.end())
				{
					v_pIterator_hour_Object = v_pIterator_hour_ObjType->second.begin(); 
					while(v_pIterator_hour_Object != v_pIterator_hour_ObjType->second.end())
					{
						if (v_pIterator_hour_Object->v_sObject_ID == cell_name)
						{
							v_pIterator_hour_Counter = v_pIterator_hour_Object->v_mCounter.begin();
							v_pIterator_Counter = v_pIterator_Object->v_mCounter.begin();
							while (v_pIterator_hour_Counter != v_pIterator_hour_Object->v_mCounter.end())
							{
								v_pIterator_type = base254Pm_type_.find(v_pIterator_hour_Counter->first);
								Pm_type = v_pIterator_type->second;
								switch (Pm_type)
								{
								case 0:
									value = _ttof(v_pIterator_hour_Counter->second);
									value += _ttof(v_pIterator_Counter->second);
									break;
								case 1:
									value = _ttof(v_pIterator_hour_Counter->second);
									value += _ttof(v_pIterator_Counter->second)/4;
									break;
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
								v_pIterator_Counter ++;
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
							v_pIterator_type = base254Pm_type_.find(v_pIterator_hour_Counter->first);
							Pm_type = v_pIterator_type->second;
							switch (Pm_type)
							{
							case 0:
								value = _ttof(v_pIterator_hour_Counter->second);
								break;
							case 1:
								value = _ttof(v_pIterator_hour_Counter->second)/4;
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
//�������ܣ����254��������������
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::GetBase254PmType()
{
	int				v_iItem,v_iID=0,value;									//KPI����
	CString			v_sTemp,v_sSql,v_sLog_Key;							//��ʱ����
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//��ʱ����
	_RecordsetPtr v_pRecordset;											//��¼��
	CMyADOConn v_cAdoConn;												//���ݿ�ʵ��
	base254Pm_type_.clear();
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
            base254Pm_type_.insert(pair<CString,int>(v_sPm,value));
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


