// MyExcel.cpp: implementation of the CMyExcel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comdef.h" 
#include "MyExcel.h"
#include "Server_Data.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// ���캯��
//-------------------------------------------------------------------------------------------------
CMyExcel::CMyExcel()													//���캯��
{

}

//-------------------------------------------------------------------------------------------------
// ��������
//-------------------------------------------------------------------------------------------------
CMyExcel::~CMyExcel()													//��������
{

}

//-------------------------------------------------------------------------------------------------
// ����Excel����д��CSV�ļ�������Excel�򿪣�
//-------------------------------------------------------------------------------------------------
void CMyExcel::Show(CListCtrl *v_cList) 
{
	Show_Name(v_cList,"",1);											//д��CSV���ļ����ƣ�Ĭ�ϣ���Excel���ļ�
}
//-------------------------------------------------------------------------------------------------
// ����Excel����д��CSV�ļ���
// ������v_cList���б�ָ��
//		 v_sFile_Name��������ļ�����
//		 v_bFile_Open���ļ��򿪱�־��[1���򿪣�0�����򿪣�]
//-------------------------------------------------------------------------------------------------
void CMyExcel::Show_Name(CListCtrl *v_cList,CString v_sFile_Name,bool v_bFile_Open) 
{
	int		v_iNum1,v_iNum2;											//��ʱ����
	int		v_iColumn,v_iRow;											//�б���������������
	CString	v_sStr,v_sCSV,v_sPath_Data,v_sCSV_File;						//��ʱ����
	CFile	v_fWriter;													//ʵ����
	CFileFind	finder;

	//Ŀ¼����
	v_sPath_Data=theApp.m_sPath+"\\DATA";								//�ļ�Ŀ¼
	if (!finder.FindFile(v_sPath_Data))									//���ڣ�
	{
		if (!::CreateDirectory(v_sPath_Data, NULL))						//�����ڣ�����Ŀ¼
		{
			MessageBox(NULL,"�쳣��"+v_sPath_Data+" Ŀ¼����ʧ�ܡ�", "��ʾ", MB_OK);		//ʧ��
			return;
		}
	}

	try
	{
		//����CSV�ļ����ļ�����v_sCSV_File
		if (v_sFile_Name.IsEmpty())
			v_sCSV_File = theApp.m_sPath+"\\DATA\\Excel.csv";			//���ã�CSVĬ���ļ�����Excel.csv
		else
			v_sCSV_File = theApp.m_sPath+"\\DATA\\"+v_sFile_Name+".csv";//���ã�CSVָ�����ļ�����
		if(v_fWriter.Open(v_sCSV_File, CFile::modeCreate | CFile::modeWrite))		//���ļ����粻���������ɣ������������ļ���
		{
			v_iRow = v_cList->GetItemCount();							//ȡ�б������
			CHeaderCtrl*  pHeaderCtrl = v_cList->GetHeaderCtrl();		//�б���ͷ
			v_iColumn=pHeaderCtrl->GetItemCount();						//ȡ�б������

			HDITEM hdi;													//�нṹ
			TCHAR lpBuffer[256]; 
			hdi.mask = HDI_TEXT; 
			hdi.pszText = lpBuffer; 
			hdi.cchTextMax = 256; 	
			//��ͷ����ȡ�б�Ŀ������ƣ�����д��CSV
			for(v_iNum1=0;v_iNum1<v_iColumn;v_iNum1++)					//��
			{
				pHeaderCtrl->GetItem(v_iNum1,&hdi);						//ȡList������
				v_sStr=hdi.pszText;										//������
				v_sStr.Replace(" ","");									//�޳��ո�
				if(v_iNum1==0)
					v_sCSV=v_sStr;										//��һ��
				else
					v_sCSV += "," + v_sStr;								//�ڶ��С������С�...����n��
			}
			v_sCSV +="\r\n";											//�س�����
			v_fWriter.Write(v_sCSV,v_sCSV.GetLength());					//д���ַ���

			//�����ݴ���ȡ�б����ݣ�д��CSV
			for(v_iNum1=0;v_iNum1<v_iRow;v_iNum1++)						//��
			{
				for(v_iNum2=0;v_iNum2<v_iColumn;v_iNum2++)				//��
				{
					v_sStr=v_cList->GetItemText(v_iNum1,v_iNum2);		//ȡ�б�����
					v_sStr.Replace(",",";");
					if(v_iNum2==0)
						v_sCSV=v_sStr;									//��һ��
					else
						v_sCSV += "," + v_sStr;							//�ڶ��С������С�...����n��
				}
				v_sCSV +="\r\n";										//�س�����
				v_fWriter.Write(v_sCSV,v_sCSV.GetLength());				//д���ַ���
			}
			v_fWriter.Close();											//�ļ��ر�

			//���ļ�
			if (v_bFile_Open)
				ShellExecute(NULL,"open", v_sCSV_File, NULL, NULL, SW_SHOWNORMAL);		//���ļ���Excel.csv
		}
		else
			MessageBox(NULL,"��ر��Ѿ��򿪵ĵ����ļ��������µ���������", "��ʾ", MB_OK);	//��ʾ��Ϣ
	}	
	catch(_com_error e)
	{
		CString v_sError = (char *)e.Description();						//��ã�������Ϣ
		MessageBox(NULL,"�쳣"+v_sError, "��ʾ", MB_OK);				//��ʾ��Ϣ
	}
}

//----------------------------------------------------------------------------------------------------
//	�������ܣ�����CSV�ļ���list�ؼ���
//	������CSV�ļ�·����list�ؼ�����		v_bHead���ļ�����ͷ��ǣ�
//----------------------------------------------------------------------------------------------------
bool CMyExcel::Get_CSV(CString v_sFile,CListCtrl *v_cList,bool v_bHead)
{
	CStdioFile	v_fRead;												//ʵ����	
	CString		v_sStr,v_sTemp;											//��ʱ����
	bool		v_Flag_Head=true;										//��ʱ����
	int			v_iLen,v_iNum,v_iItem;									//��ʱ����

	if (!v_fRead.Open(_T(v_sFile),CFile::modeRead))						//���ļ�
	{
		MessageBox(NULL,"�쳣���ļ���ʧ�ܣ�����", "��ʾ", MB_OK);		//��ʾ��Ϣ
		return false;													//���أ�ʧ��
	}
	
	while(v_fRead.ReadString(v_sStr))									//���ݶ�ȡ��һ��
	{
		if (v_bHead && v_Flag_Head)										//����ͷ��
		{
			v_Flag_Head=false;											//����ͷ�����Ե�
			continue;													//
		}
		//һ�����ݴ���
		v_iNum=0;														//�к�
		while (true)
		{
			if (v_sStr.IsEmpty())										//Ϊ�գ�����
				break;													//�˳�
			v_iLen=v_sStr.Find(",",0);									//����","
			if (v_iLen<0)												//�ҵ�������
			{
				v_sTemp=v_sStr;											//��ȡ����Ч��Ϣ
				v_sStr="";												//���㣺��һ����Ϣλ��
			}
			else
			{
				v_sTemp=v_sStr.Left(v_iLen);							//��ȡ����Ч��Ϣ
				v_sStr=v_sStr.Mid(v_iLen+1);							//���㣺��һ����Ϣλ��
			}
			if (v_iNum==0)
				v_iItem=v_cList->InsertItem(0xffff,v_sTemp);			//�У�0
			else
				v_cList->SetItemText(v_iItem,v_iNum,(_bstr_t)v_sTemp);	//�У�1��2��3...
			v_iNum++;													//�кţ�++
		}
	}
	v_fRead.Close();													//�ر��ļ�
	return true;														//���أ��ɹ�
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
bool CMyExcel::do_events()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) > 0)
	{
		
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		GetMessage(&msg, NULL, 0, 0);
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
// ����
//-------------------------------------------------------------------------------------------------
