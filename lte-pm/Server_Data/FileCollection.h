#pragma once
class FileCollection
{
public:
	FileCollection(void);
	~FileCollection(void);
	//���δ�����Log���ơ���socket�������ݸ�ʽ��ͬ��
	void GetUnprocessedLog(CString directory_path,CString granularity_time,queue<CString> &file_queue);
	//��ø�Ŀ¼�µ��ļ���
	void SearchDirectory(CString directory_path,list<CString> &folder_name);   
	//�����Ҫ���������GZ�ļ�����
	void GetValidGZFile(CString sFilePath,COleDateTime lastTime,map<CString,CString>  &map_file_name);
	//���Ŀ¼����Чgz�ļ�
	void SereachGZFile(CString sDirPath,map<CString,CString>  &map_file_name);
	//����ļ�ʱ��
	void GetFileTime(const CString& szFileName,CString &sFileTime);
};

