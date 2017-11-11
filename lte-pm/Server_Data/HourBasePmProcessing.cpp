#include "stdafx.h"
#include "HourBasePmProcessing.h"
//视图类
#include "Server_DataDlg.h"
HourBasePmProcessing::HourBasePmProcessing(void)
{
}


HourBasePmProcessing::~HourBasePmProcessing(void)
{
}

//------------------------------------------------------------------------------------------------------			
//函数功能：打开待保存的6个TXT文件
//参    数：directory_path:路径；file_name：文件名称
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
//函数功能：将cell性能数据分解为六个文件
//参    数：file_time：文件时间；Pm_classification：cell性能分类标准
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::ProcessData(CString file_time,list<CString> *Pm_classification)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjType指示器,XML总体数据
	list_Object::iterator	v_pIterator_Object;							//list_Object迭代器，保存全体性能Map列表
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter迭代器，操作总体每个Map容器迭代器
	map_Counter::iterator	v_pIterator_File;							//map_Counter迭代器，操作总体每个FileNameMap容器迭代器
	int	iMapCout(0),iCount(0);                                          //记录CELL数据条数
	v_pIterator_ObjType = Pm_hour_object_.begin();		                //获取总体数据的迭代器
	while (v_pIterator_ObjType != Pm_hour_object_.end())
	{   		
			if(!v_pIterator_ObjType->second.empty())						        //ObjType中有Object？[即：Object数量为空？？？]
			{
				iMapCout += v_pIterator_ObjType->second.size();
				v_pIterator_Object=v_pIterator_ObjType->second.begin();		        //获得：Object迭代器起始位置
				while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//条件：Object迭代器终止位置
				{
					cell_key_ = file_time+","+v_pIterator_Object->v_sObject_ID;     //保存头两列数据,txt六个文件添加每行头信息
				    WriteBasePm(Pm_classification,*v_pIterator_Object);
					v_pIterator_Object++;
				}
			}
			v_pIterator_ObjType++;	  
	}
}
//------------------------------------------------------------------------------------------------------			
//函数功能：处理基本性能数据
//参    数：Pm_classification：分类标准；file_object：小区性能分类
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
//函数功能：处理227个性能数据	
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
		if (iCount==50)  //性能表中为空
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
		if (iCount==64||iCount==66||iCount==72||iCount==74||iCount==81||iCount==83||iCount==90||iCount==92||iCount==94||iCount==96||iCount==98||iCount==100||iCount==102) //性能表中为空
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
		if (iCount==142||iCount==144||iCount==146||iCount==149||iCount==151||iCount==153||iCount==155||iCount==156||iCount==157||iCount==164||iCount==165||iCount==167)  //性能表中为空
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
		if (iCount==241)     //性能表中为空
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
//函数功能：处理254个性能数据
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
//函数功能：关闭六个txt文件
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::ClosePmFile()
{
	for (int iNum=0;iNum<6;iNum++)//关闭6个文本文件
	{
		write_file_[iNum].Close();
	}	
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------			
//函数功能：6个TXT文件数据上传数据库【参数:sFileTime文件时间】
//参    数：sFileTime：log文件时间；save_directory_path：bulk insert文件路径,CServer_DataDlg: 调用视图函数,table_suffix：表后缀,control_count :控制批处理数量；XML_file_name：压缩文件名称；file_time 文件时间
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
					sMsg  = sFileTime+"Pm_LTE_PDCP文件上传失败";		
					pDlg->My_ShowList_Bug("数据库异常","数据库异常："+sMsg);		//显示：异常
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
					sMsg  = sFileTime+"Pm_LTE_TXT文件上传失败";		
					pDlg->My_ShowList_Bug("数据库异常","数据库异常："+sMsg);		//显示：异常
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
					sMsg  = sFileTime+"Pm_LTE_RRC文件上传失败";		
					pDlg->My_ShowList_Bug("数据库异常","数据库异常："+sMsg);		//显示：异常
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
					sMsg  = sFileTime+"Pm_LTE_THO文件上传失败";		
					pDlg->My_ShowList_Bug("数据库异常","数据库异常："+sMsg);		//显示：异常
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
					sMsg  = sFileTime+"Pm_LTE_MAC文件上传失败";		
					pDlg->My_ShowList_Bug("数据库异常","数据库异常："+sMsg);		//显示：异常
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
					sMsg  = sFileTime+"Pm_LTE_PHY文件上传失败";		
					pDlg->My_ShowList_Bug("数据库异常","数据库异常："+sMsg);		//显示：异常
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
//  函数功能：实现性能公式分解
//  参    数:v_sFormula：具体计算公式 v_mFormula：分解公式
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::DecomposeFormula(CString v_sFormula, mPM_Formula* v_mFormula)
{
	CString		v_sStr = "";												//临时变量
	bool		v_bFlag = false;											//标记：运算符
	char*		v_pFormula;												    //临时变量
	PM_Formula_Struct	v_cPM_Formula;									    //结构
	v_mFormula->clear();
	v_pFormula = v_sFormula.GetBuffer(0);
	for (int vi = 0; vi<v_sFormula.GetLength(); vi++)						//遍历计算公式
	{
		if ((unsigned char)v_pFormula[vi]<0x30)							    //运算符？？？
		{
			if (v_bFlag)
			{
				if (!v_sStr.IsEmpty())
				{
					v_cPM_Formula.v_sPM_Formula = v_sStr;
					v_cPM_Formula.v_bFormula = v_bFlag;
					if (v_sStr.Left(1) <= "9")							    //若预算值的第一位必须为字符，否则为常数，如：100等，按运算符处理【即不用获取Counter值】
						v_cPM_Formula.v_bFormula = false;					//运算符
					v_mFormula->push_back(v_cPM_Formula);				    //增加：运算值
					v_sStr = "";											//复位
				}
				v_bFlag = false;											//标记：运算符
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
					v_mFormula->push_back(v_cPM_Formula);				    //增加：运算符
					v_sStr = "";											//复位
				}
				v_bFlag = true;											    //标记：运算值
			}
		}
		v_sStr += v_sFormula.Mid(vi, 1);									//获取：字符串
	}
	if (!v_sStr.IsEmpty())
	{
		v_cPM_Formula.v_sPM_Formula = v_sStr;
		v_cPM_Formula.v_bFormula = v_bFlag;
		if (v_cPM_Formula.v_bFormula)									    //运算值？？？
		{
			if (v_sStr.Left(1) <= "9")									    //若预算值的第一位必须为字符，否则为常数，如：100等，按运算符处理【即不用获取Counter值】
				v_cPM_Formula.v_bFormula = false;							//运算符
		}
		v_mFormula->push_back(v_cPM_Formula);							    //增加：运算值/运算符
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  函数功能：实现性能公式合成
//  参    数:CSstringformula:合成公式 StructFormula：分解公式
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::CompositionFormula(CString *CSstringformula,mPM_Formula StructFormula)
{
	// TODO: 在此添加控件通知处理程序代码
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
//函数功能：保存一小时粒度公式性能到TXT
//参    数：directory_path:路径；XML_file_name：文件名称；file_time 文件时间
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::SaveFormulaPmToTXT(CString derictory_path,CString file_name,CString file_time)
{
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	CString formula_performace="",cell_name;
	//打开公式性能文件
	formula_Pm_file_.Open(derictory_path+"\\Pm_LTE_Cal_Hour_"+ file_name + ".txt",CFile::modeWrite|CFile::modeRead|CFile::modeCreate);
	iterator_performance_objtype = Pm_object_.begin();
	//将Cell公式性能写入文件
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
	//关闭公式性能文件
	formula_Pm_file_.Close();
}
//-----------------------------------------------------------------------------------------------------------------
//  函数功能：实现性能计算；计算值保存在全局变量performance_object_中；加小区预警
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::CalculatePerformance(CString time)
{
	//以下为实际过程
	CString formula_result = "", formula = "", performace_name = "", performace_cstring_value = "";//计算公式；公式原型;性能名称;性能值
	mPM_Formula::iterator   iterator_decompose_formula;                 //分解公式迭代器
	map_ObjType::iterator	iterator_objtype;						    //count文件迭代器
	list_Object::iterator	iterator_Object;							//count小区迭代器
	map_Counter::iterator	iterator_counter;						    //count性能迭代器
	map_ObjType::iterator	iterator_performance_objtype;			    //性能文件迭代器
	list_Object::iterator	iterator_performance_object;				//性能小区迭代器
	map_Counter::iterator	iterator_map_formula;						//性能公式迭代器
	CMyFormula calculate_formula;//计算性能指标类
	char * Pcharformula = NULL;//用于性能指标输入
	bool performance_flag = false;//性能值匹配标签
	float number_result = 0;//性能值
	mPM_Formula  decompose_formula;//分解公式
	//循环处理变量替换性能值并计算
	//文件循环
	Pm_object_ = Pm_hour_object_;
	iterator_objtype = Pm_hour_object_.begin();
	iterator_performance_objtype =Pm_object_.begin();
	while (iterator_objtype != Pm_hour_object_.end())
	{
		//小区循环
		iterator_Object = iterator_objtype->second.begin();
		iterator_performance_object = iterator_performance_objtype->second.begin();
		while (iterator_Object != iterator_objtype->second.end())
		{
			//清空原map中的count数据
			iterator_performance_object->v_mCounter.clear();
			//性能公式循环
			iterator_map_formula = map_formula_.begin();
			while(iterator_map_formula != map_formula_.end())
			{
				//性能名称
				performace_name = iterator_map_formula->first;
				//保存性能数据结构
				formula = iterator_map_formula->second;
				//替换"."为aaa
				formula.Replace(".", "aaa");
				//将公式分解;成变量和运算符
				DecomposeFormula(formula, &decompose_formula);
				//还原性能名称中的"."
				iterator_decompose_formula = decompose_formula.begin();
				while (iterator_decompose_formula != decompose_formula.end())
				{
					iterator_decompose_formula->v_sPM_Formula.Replace("aaa", ".");
					iterator_decompose_formula++;
				}
				//公式变量替换为性能值
				iterator_decompose_formula = decompose_formula.begin();
				while (iterator_decompose_formula != decompose_formula.end())
				{
					//替代公式中的变量
					if (iterator_decompose_formula->v_bFormula == true)
					{
						iterator_counter = iterator_Object->v_mCounter.begin();
						performance_flag = false;
						while (iterator_counter != iterator_Object->v_mCounter.end() && (!performance_flag))
						{
							//找到匹配的性能值
							if (iterator_counter->first == iterator_decompose_formula->v_sPM_Formula)
							{
								iterator_decompose_formula->v_sPM_Formula = iterator_counter->second;
								performance_flag = true;
							}
							iterator_counter++;
						}
						if (!performance_flag)
						{
							//应该改为异常显示
							//MessageBoxA("性能未匹配");
						}
						iterator_decompose_formula++;
					}
					else
					{
						iterator_decompose_formula++;
						continue;
					}

				}
				//还原计算公式
				CompositionFormula(&formula_result, decompose_formula);
				//性能计算
				Pcharformula = formula_result.GetBuffer(0);
				number_result = calculate_formula.simcalc(Pcharformula);
				performace_cstring_value.Format("%f", number_result);
				//插入性能
				iterator_performance_object->v_mCounter.insert(pair<CString, CString>(performace_name, performace_cstring_value));
				//Reset变量
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
	//小区预警
	Pm_warning_.file_time_ = time;
	Pm_warning_.CompareToFormulaPerformance(Pm_object_);
	//Reset变量
	decompose_formula.clear();
}

//-----------------------------------------------------------------------------------------------------------------
//  函数功能：从数据库中读取性能处理公式
//-----------------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::GetFomula()
{
	// TODO: 在此添加控件通知处理程序代码
	CString v_sSql,formula,formula_name;
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	map_formula_.clear();                                               //清空数据
	v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
	v_cAdoConn.SetCommandTimeout(100);
	v_sSql="SELECT zPm,zPm_Formula FROM CON_Pm_LTE_Cal_This_Month";
	v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句	
	while (!v_pRecordset->adoEOF)
	{
		formula_name = v_cAdoConn.GetField_String("zPm");			//公式名称
		formula = v_cAdoConn.GetField_String("zPm_Formula");			       //公式计算值
		map_formula_.insert(pair<CString,CString> (formula_name,formula));
		v_pRecordset->MoveNext();
	}
}
//-----------------------------------------------------------------------------------------------------------------
//  函数功能：初始化
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
//函数功能：性能公式文件数据上传小时粒度数据库
//参   数：sFileTime：文件时间；save_directory_path：bulk insert文件路径；CServer_DataDlg: 调用视图函数,table_suffix：表后缀,control_count :控制批处理数量；XML_file_name：压缩文件名称；file_time 文件时间
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
			sMsg  = sFileTime+"Pm_LTE_Cal文件上传失败";		
			pDlg->My_ShowList_Bug("数据库异常","数据库异常："+sMsg);		//显示：异常
			sTemp=sFileTime;
			sTemp.Replace(":","");
			//CopyFile(save_directory_path+"\\Pm_LTE_Cal_Hour.txt",save_directory_path+"\\"+sTemp+"Pm_LTE_Cal_Hour.txt",false);
		}
	}			

}
//------------------------------------------------------------------------------------------------------			
//函数功能：保存4个粒度基本数据
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::SaveHourPmData(CMyFile_XML &v_cFile_XML)
{
	map_ObjType::iterator	v_pIterator_ObjType;						//list_ObjType指示器,XML总体数据
	map_ObjType::iterator	v_pIterator_hour_ObjType;				    //list_ObjType指示器,保存小时数据
	list_Object::iterator	v_pIterator_Object;							//list_Object迭代器，保存全体性能Map列表
	list_Object::iterator	v_pIterator_hour_Object;				    //list_Object迭代器，保存小区
	map_Counter::iterator	v_pIterator_Counter;						//map_Counter迭代器，操作总体每个Map容器迭代器
	map_Counter::iterator	v_pIterator_hour_Counter;				    //map_Counter迭代器，操作性能容器迭代器
	map<CString,int>::iterator	v_pIterator_type;							//map_Counter迭代器，254性能类型
	CString cell_name;      //小区名称
	object_type_ = false;                                               //判断254还是227个性能数据
	float value,value1;                                                        //数据值
	int   Pm_type;
	File_Object  temp;                                                  //临时变量
	v_pIterator_ObjType = v_cFile_XML.v_mObjType.begin();		        //获取总体数据的迭代器
	while (v_pIterator_ObjType != v_cFile_XML.v_mObjType.end())
	{   		
		if(!v_pIterator_ObjType->second.empty())						        //ObjType中有Object？[即：Object数量为空？？？]
		{
			v_pIterator_Object = v_pIterator_ObjType->second.begin();		    //获得：Object迭代器起始位置
			while(v_pIterator_Object != v_pIterator_ObjType->second.end())		//条件：Object迭代器终止位置
			{
				cell_name = v_pIterator_Object->v_sObject_ID;                   //小区名称
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
//函数功能：获得254个基本性能类型
//------------------------------------------------------------------------------------------------------
void HourBasePmProcessing::GetBase254PmType()
{
	int				v_iItem,v_iID=0,value;									//KPI数量
	CString			v_sTemp,v_sSql,v_sLog_Key;							//临时变量
	CString			v_sType,v_sType_1,v_sPm,v_sPm_Name,v_sTable_Key;	//临时变量
	_RecordsetPtr v_pRecordset;											//记录集
	CMyADOConn v_cAdoConn;												//数据库实例
	base254Pm_type_.clear();
	try
	{											//条目选择无效

		v_cAdoConn.OnInitADOConn(theApp.a_iDatabase_Type,theApp.a_sDatabase_Ip,theApp.a_sDatabase_Name,theApp.a_sDatabase_User,theApp.a_sDatabase_Password);
		v_cAdoConn.SetCommandTimeout(100);
		v_sSql="SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED ";
		v_sSql += "SELECT zPm,zHour FROM CON_Pm_LTE ";
		v_sSql+=" order by ztype,ztype_1,zPm";
		v_pRecordset = v_cAdoConn.GetRecordSet((_bstr_t)v_sSql);		//执行SELETE语句										
		while(!v_pRecordset->adoEOF)
		{
			v_sPm = v_cAdoConn.GetField_String("zPm");					//zPm
			value = v_cAdoConn.GetField_Int("zHour");                   //zHour
            base254Pm_type_.insert(pair<CString,int>(v_sPm,value));
			v_pRecordset->MoveNext();									//
		}
		v_cAdoConn.ExitConnect();										//断开连接
	}
	catch(_com_error e)
	{
		v_cAdoConn.ExitConnect();										//断开连接
		v_sTemp = (char *)e.Description();								//错误信息
	}
}


