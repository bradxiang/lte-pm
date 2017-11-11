// COM_About.cpp : 实现文件
//

#include "stdafx.h"
#include "Server_Data.h"
#include "COM_About.h"


// CCOM_About 对话框

IMPLEMENT_DYNAMIC(CCOM_About, CDialog)

CCOM_About::CCOM_About(CWnd* pParent /*=NULL*/)
	: CDialog(CCOM_About::IDD, pParent)
{

}

CCOM_About::~CCOM_About()
{
}

void CCOM_About::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCOM_About, CDialog)
	ON_STN_CLICKED(IDB_ABOUT, &CCOM_About::OnStnClickedAbout)
END_MESSAGE_MAP()


// CCOM_About 消息处理程序

void CCOM_About::OnStnClickedAbout()
{
	// TODO: 在此添加控件通知处理程序代码
}
