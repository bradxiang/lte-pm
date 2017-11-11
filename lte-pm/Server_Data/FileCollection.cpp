#include "stdafx.h"
#include "FileCollection.h"


FileCollection::FileCollection(void)
{
}


FileCollection::~FileCollection(void)
{
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�����ļ����е��ļ�����
//��   �� :sDirPath:�����ļ��е�·���������ڱ���map_file_name����map_file_name :����������ļ�
//------------------------------------------------------------------------------------------------------
void FileCollection::SereachGZFile(CString sDirPath,map<CString,CString> &map_file_name)
{
	CFileFind   tempFind;  
	CString     sTemp;
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(sDirPath + "\\*.*")); //�ж��ļ���
	while(IsFinded)														   //��Ч�ļ��У�����
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							  //Ѱ����һ���ļ�
		CString sFileName= tempFind.GetFileName();				          //��ȡ���ļ�����/Ŀ¼����
		sFileName=sDirPath+"\\"+sFileName;
		sTemp=sFileName;
		sTemp.Delete(0,24);
		if (sTemp.Right(2) != "gz")
		{
			continue;
		}
		if (sTemp.Left(1)=="E")                     //HW PM
		{
			sTemp=sTemp.Mid(28,16);
			sTemp.Replace("-"," ");
			sTemp.Insert(4,"-");
			sTemp.Insert(7,"-");
			sTemp.Insert(13,":");
			sTemp.Insert(16,":00");
			sTemp+="_HW";

		} 
		else
		{
			sTemp=sTemp.Mid(33,16);                     //ZTE PM
			sTemp.Replace("-"," ");
			sTemp.Replace("-"," ");
			sTemp.Insert(4,"-");
			sTemp.Insert(7,"-");
			sTemp.Insert(13,":");
			sTemp.Insert(16,":00");
			sTemp+="_ZTE";
		}
		if(!tempFind.IsDots())											    //..����.������  
		{																	//����ļ���
			if(!tempFind.IsDirectory())
			map_file_name.insert(pair<CString,CString> (sTemp,sFileName));
		}
	} 
	tempFind.Close(); 											  //��Ѱ���ļ����¼������ͽ���Ѱ�ҹ���
}

//-----------------------------------------------------------------------------------------------------------			
//�������ܣ������Ҫ���������GZ�ļ�����
//��    ��:sFilePath�����ļ���·����lastTime�����µĴ����������ʱ�䣻map_file_name :����������ļ�
//-----------------------------------------------------------------------------------------------------------
void FileCollection::GetValidGZFile(CString sFilePath,COleDateTime lastTime,map<CString,CString>  &map_file_name)
{
	CFileFind   tempFind;  
	CString     sTemp,sFileTime;
	COleDateTime fileTime;
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(sFilePath+"\\*.*")); //�ж��ļ���
	while(IsFinded)														  //��Ч�ļ��У�����
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							  //Ѱ����һ���ļ�
		CString sFileName= tempFind.GetFileName();				          //��ȡ���ļ�����/Ŀ¼����
		if(!tempFind.IsDots())											  //..����.������  
		{																  //����ļ���
			if(!tempFind.IsDirectory())
			{
				GetFileTime(sFileName,sFileTime);
				fileTime.ParseDateTime(sFileTime);
				sFileName =sFilePath+"\\"+sFileName;
				sTemp=sFileName;
				sTemp.Delete(0,24);
				if (sTemp.Right(2) != "gz")
				{
					continue;
				}
				if (sTemp.Left(1)=="E")                     //HW PM
				{
					sTemp=sTemp.Mid(28,16);
					sTemp.Replace("-"," ");
					sTemp.Insert(4,"-");
					sTemp.Insert(7,"-");
					sTemp.Insert(13,":");
					sTemp.Insert(16,":00");
					sTemp+="_HW";

				} 
				else
				{
					sTemp=sTemp.Mid(33,16);            //ZTE PM
					sTemp.Replace("-"," ");
					sTemp.Replace("-"," ");
					sTemp.Insert(4,"-");
					sTemp.Insert(7,"-");
					sTemp.Insert(13,":");
					sTemp.Insert(16,":00");
					sTemp+="_ZTE";
				}
				if (lastTime<fileTime)	
				map_file_name.insert(pair<CString,CString> (sTemp,sFileName));
			}
		}
	} 
	tempFind.Close(); 
}

//------------------------------------------------------------------------------------------------------			
//�������ܣ�����ļ�������
//��    ��:sDirPath:�����ļ��е�·����������ȫ�ֱ���folder_name������;folder_name:logĿ¼�µ��ļ�������
//------------------------------------------------------------------------------------------------------
void FileCollection::SearchDirectory(CString directory_path,list<CString> &folder_name)
{
	CFileFind   tempFind;  
	CString     sTemp;
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(directory_path+"\\*.*")); //�ж��ļ���
	COleDateTime FolderTime;
	bool flag=false;
	while(IsFinded)														//��Ч�ļ��У�����
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							//Ѱ����һ���ļ�
		CString sFileName= tempFind.GetFileName();				        //��ȡ���ļ�����/Ŀ¼����
		if(!tempFind.IsDots())											//..����.������  
		{															    //����ļ���
			if(tempFind.IsDirectory())
			{
				//�ж��ļ����Ƿ���Ч
				flag = FolderTime.ParseDateTime(sFileName, VAR_DATEVALUEONLY);   
				if (flag)
				{
					folder_name.push_back(sFileName);
				} 
				else
				{
					continue;
				}	
			}
				
		}
	} 
	tempFind.Close(); 											       //��Ѱ���ļ����¼������ͽ���Ѱ�ҹ���
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------			
//�������ܣ����δ�������ȵ�����
//��    ��:directory_path��log����·����granularity_time��������������ʱ��;file_queue :�ļ�����;
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FileCollection::GetUnprocessedLog(CString directory_path,CString granularity_time,queue<CString> &file_queue)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//log����ȫ��δ�����ļ���
	list<CString>         unprocessed_folder; 
	//logĿ¼�µ��ļ�������
	list<CString>         folder_name;
	//����������ļ�
	map<CString,CString>  map_file_name;
	CString sFolderName,sFolderFile,sTemp,current_file_name="",before_file_name="",current_file_time ="",before_file_time="";
	COleDateTime FolderTime,CompareTime,SpanTime,GranularityTime;
	bool flag=false;
	int file_count(0);
	SearchDirectory(directory_path,folder_name);
	while(folder_name.begin()!=folder_name.end())               //���ĵ�ʱ�������󱣴�ʱ����ĵ����Ʊ���List
	{
		sFolderName=*folder_name.begin();
		// ParseDateTime()ֻ���жϼ����Ϊ��-���������ַ���
		flag = FolderTime.ParseDateTime(sFolderName, VAR_DATEVALUEONLY);   
		if (flag)
		{
			FolderTime.ParseDateTime(sFolderName);	
		} 
		else
		{
			folder_name.pop_front(); 
			continue;
		}		
		sTemp = granularity_time;
		sTemp.Delete(11,8);
		sTemp += "00:00:00";
		CompareTime.ParseDateTime(sTemp);
		if (FolderTime>CompareTime)                                //�ж��ļ���ʱ���Ƿ���������ʱ��
		{
			unprocessed_folder.push_back(sFolderName);
		} 
		else if(FolderTime==CompareTime)
		{
			GranularityTime.ParseDateTime(granularity_time);
			GetValidGZFile(directory_path + "\\" + sFolderName,GranularityTime,map_file_name);
		}
		folder_name.pop_front();      
	}
	while(unprocessed_folder.begin()!=unprocessed_folder.end())
	{
		sFolderFile=directory_path +"\\" + *unprocessed_folder.begin();
		SereachGZFile(sFolderFile,map_file_name);
		unprocessed_folder.pop_front();
	}	
	//�ı�����ļ�����·����FTP������һ�¡�D:\30_Pm_Log��ΪD:\��
	map_Counter::iterator v_pIterator;
	v_pIterator=map_file_name.begin();
	while(v_pIterator!=map_file_name.end())
	{
		(v_pIterator->second).Delete(3,10);
		v_pIterator++;
	}
	//��map_file_name�е����ݱ��浽file_queue��
	v_pIterator=map_file_name.begin();
	while(v_pIterator!=map_file_name.end())
	{
		if (v_pIterator->first.Right(3) == "ZTE")//zte
		{
			current_file_time = v_pIterator->second.Mid(47,13);
			current_file_time.Replace("-"," ");
			current_file_time.Insert(4,"-");
			current_file_time.Insert(7,"-");
			current_file_time.Insert(13,":");
			current_file_time.Insert(16,":00");
		} 
		else//hw
		{
			current_file_time = v_pIterator->second.Mid(42,13);
			current_file_time.Replace("-"," ");
			current_file_time.Insert(4,"-");
			current_file_time.Insert(7,"-");
			current_file_time.Insert(13,":");
			current_file_time.Insert(16,":00");
		}
		current_file_name = v_pIterator->second;
		if (current_file_time != before_file_time && before_file_time != "")
		{  
			file_queue.push("����");
			file_queue.push(current_file_name);
			file_count = 1;
		} 
		else
		{
			file_queue.push(current_file_name);
			file_count ++;
		}

		before_file_name = v_pIterator->second;
		before_file_time = current_file_time; 

		v_pIterator++;
	}
	if (v_pIterator == map_file_name.end() && file_count == 8) //�жϽ�β�ǲ���������
	{
		file_queue.push("����");
	}
	map_file_name.clear();//���
}

//------------------------------------------------------------------------------------------------------		
//�������ܣ�����ļ�������ʱ��
//��    ����szFileName:ѹ���ļ����ƣ�sFileTime:�ļ�����ȡ��ʱ����Ϣ
//------------------------------------------------------------------------------------------------------
void FileCollection::GetFileTime(const CString& szFileName,CString &sFileTime)
{
	if (szFileName.Left(1)=="E")                     //HW PM
	{
		sFileTime=szFileName.Mid(28,13);
		sFileTime.Replace("-"," ");
		sFileTime.Insert(4,"-");
		sFileTime.Insert(7,"-");
		sFileTime.Insert(13,":");
		sFileTime.Insert(16,":00");

	} 
	else
	{
		sFileTime=szFileName.Mid(33,13);            //ZTE PM
		sFileTime.Replace("-"," ");
		sFileTime.Replace("-"," ");
		sFileTime.Insert(4,"-");
		sFileTime.Insert(7,"-");
		sFileTime.Insert(13,":");
		sFileTime.Insert(16,":00");
	}
}