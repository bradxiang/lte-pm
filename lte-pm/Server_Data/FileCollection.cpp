#include "stdafx.h"
#include "FileCollection.h"


FileCollection::FileCollection(void)
{
}


FileCollection::~FileCollection(void)
{
}

//------------------------------------------------------------------------------------------------------			
//函数功能：获得文件夹中的文件名称
//参   数 :sDirPath:查找文件夹的路径【保存在变量map_file_name】，map_file_name :保存待处理文件
//------------------------------------------------------------------------------------------------------
void FileCollection::SereachGZFile(CString sDirPath,map<CString,CString> &map_file_name)
{
	CFileFind   tempFind;  
	CString     sTemp;
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(sDirPath + "\\*.*")); //判断文件夹
	while(IsFinded)														   //有效文件夹？？？
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							  //寻找下一个文件
		CString sFileName= tempFind.GetFileName();				          //获取：文件名称/目录名称
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
		if(!tempFind.IsDots())											    //..或者.？？？  
		{																	//获得文件夹
			if(!tempFind.IsDirectory())
			map_file_name.insert(pair<CString,CString> (sTemp,sFileName));
		}
	} 
	tempFind.Close(); 											  //当寻找文件的事件结束就结束寻找功能
}

//-----------------------------------------------------------------------------------------------------------			
//函数功能：获得需要处理的粒度GZ文件名称
//参    数:sFilePath查找文件的路径；lastTime：最新的处理完成粒度时间；map_file_name :保存待处理文件
//-----------------------------------------------------------------------------------------------------------
void FileCollection::GetValidGZFile(CString sFilePath,COleDateTime lastTime,map<CString,CString>  &map_file_name)
{
	CFileFind   tempFind;  
	CString     sTemp,sFileTime;
	COleDateTime fileTime;
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(sFilePath+"\\*.*")); //判断文件夹
	while(IsFinded)														  //有效文件夹？？？
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							  //寻找下一个文件
		CString sFileName= tempFind.GetFileName();				          //获取：文件名称/目录名称
		if(!tempFind.IsDots())											  //..或者.？？？  
		{																  //获得文件夹
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
//函数功能：获得文件夹名称
//参    数:sDirPath:查找文件夹的路径【保存在全局变量folder_name变量】;folder_name:log目录下的文件夹名称
//------------------------------------------------------------------------------------------------------
void FileCollection::SearchDirectory(CString directory_path,list<CString> &folder_name)
{
	CFileFind   tempFind;  
	CString     sTemp;
	BOOL   IsFinded=(BOOL)tempFind.FindFile((LPCSTR)(directory_path+"\\*.*")); //判断文件夹
	COleDateTime FolderTime;
	bool flag=false;
	while(IsFinded)														//有效文件夹？？？
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile();							//寻找下一个文件
		CString sFileName= tempFind.GetFileName();				        //获取：文件名称/目录名称
		if(!tempFind.IsDots())											//..或者.？？？  
		{															    //获得文件夹
			if(tempFind.IsDirectory())
			{
				//判断文件夹是否有效
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
	tempFind.Close(); 											       //当寻找文件的事件结束就结束寻找功能
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------			
//函数功能：获得未处理粒度的名称
//参    数:directory_path：log保存路径，granularity_time：最后处理完成粒度时间;file_queue :文件队列;
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void FileCollection::GetUnprocessedLog(CString directory_path,CString granularity_time,queue<CString> &file_queue)
{
	// TODO: 在此添加控件通知处理程序代码
	//log数据全部未处理文件夹
	list<CString>         unprocessed_folder; 
	//log目录下的文件夹名称
	list<CString>         folder_name;
	//保存待处理文件
	map<CString,CString>  map_file_name;
	CString sFolderName,sFolderFile,sTemp,current_file_name="",before_file_name="",current_file_time ="",before_file_time="";
	COleDateTime FolderTime,CompareTime,SpanTime,GranularityTime;
	bool flag=false;
	int file_count(0);
	SearchDirectory(directory_path,folder_name);
	while(folder_name.begin()!=folder_name.end())               //当文档时间大于最后保存时间把文档名称保存List
	{
		sFolderName=*folder_name.begin();
		// ParseDateTime()只能判断间隔符为“-”的日期字符串
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
		if (FolderTime>CompareTime)                                //判断文件夹时间是否大于最后处理时间
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
	//改变读入文件名及路径与FTP发送来一致【D:\30_Pm_Log改为D:\】
	map_Counter::iterator v_pIterator;
	v_pIterator=map_file_name.begin();
	while(v_pIterator!=map_file_name.end())
	{
		(v_pIterator->second).Delete(3,10);
		v_pIterator++;
	}
	//将map_file_name中的数据保存到file_queue中
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
			file_queue.push("结束");
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
	if (v_pIterator == map_file_name.end() && file_count == 8) //判断结尾是不是整粒度
	{
		file_queue.push("结束");
	}
	map_file_name.clear();//清空
}

//------------------------------------------------------------------------------------------------------		
//函数功能：获得文件中粒度时间
//参    数：szFileName:压缩文件名称；sFileTime:文件中提取的时间信息
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