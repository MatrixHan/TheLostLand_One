
// FK_1_Maker.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FK_1_Maker.h"
#include "FK_1_MakerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFK_1_MakerApp

BEGIN_MESSAGE_MAP(CFK_1_MakerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFK_1_MakerApp ����

CFK_1_MakerApp::CFK_1_MakerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CFK_1_MakerApp ����

CFK_1_MakerApp theApp;


// CFK_1_MakerApp ��ʼ��

BOOL CFK_1_MakerApp::InitInstance()
{
	CWinAppEx::InitInstance();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CFK_1_MakerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
