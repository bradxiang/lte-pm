#pragma once
class FileCollection
{
public:
	FileCollection(void);
	~FileCollection(void);
	//获得未处理的Log名称【与socket传入数据格式相同】
	void GetUnprocessedLog(CString directory_path,CString granularity_time,queue<CString> &file_queue);
	//获得根目录下的文件夹
	void SearchDirectory(CString directory_path,list<CString> &folder_name);   
	//获得需要处理的粒度GZ文件名称
	void GetValidGZFile(CString sFilePath,COleDateTime lastTime,map<CString,CString>  &map_file_name);
	//获得目录下有效gz文件
	void SereachGZFile(CString sDirPath,map<CString,CString>  &map_file_name);
	//获得文件时间
	void GetFileTime(const CString& szFileName,CString &sFileTime);
};

