
// FK_1_MakerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FK_1_Maker.h"
#include "FK_1_MakerDlg.h"
#include "CCsvToScp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFK_1_MakerDlg �Ի���




CFK_1_MakerDlg::CFK_1_MakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFK_1_MakerDlg::IDD, pParent)
{
	m_CStrFileName = _T("");
	m_bPlus2Comma = TRUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFK_1_MakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_CStrFileName);
	DDX_Check(pDX, IDC_CHECK1, m_bPlus2Comma);
}

BEGIN_MESSAGE_MAP(CFK_1_MakerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CFK_1_MakerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFK_1_MakerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &CFK_1_MakerDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CFK_1_MakerDlg ��Ϣ�������

BOOL CFK_1_MakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFK_1_MakerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFK_1_MakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------
// ѡ���ļ���ť
void CFK_1_MakerDlg::OnBnClickedButton1()
{
	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Excel���ɵ�CSV��ʽ (*.csv)|*.csv|", this);
	if ( IDOK != FileDialog.DoModal())
	{
		return;
	}
	m_CStrFileName = FileDialog.GetPathName();
	UpdateData(FALSE);

	return;
}
//------------------------------------------------------------------------
// ת���ļ���ť
void CFK_1_MakerDlg::OnBnClickedButton2()
{
	if ( m_CStrFileName.GetLength() <= 0)
	{
		MessageBox("������CSV�ļ���!");
		return;
	}	
	TCHAR *szCSVFileName = m_CStrFileName.GetBuffer(m_CStrFileName.GetLength());
	m_CStrFileName.ReleaseBuffer();

	TCHAR szDefSaveFileName[MAX_STRING_Name] = {0};
	ChangeToSCPName(szDefSaveFileName, szCSVFileName);

	TCHAR szSaveFileName[MAX_STRING_Name] = {0};
	if ( !MySaveFile(szDefSaveFileName, szSaveFileName) )
	{
		return;
	}

	TCHAR szErrmsg[MAX_STRING_Name] = {0};
	CCSVToSCP MyCSVToSCP;
	if ( !MyCSVToSCP.MakeCSVToSCP(szCSVFileName, szSaveFileName, szErrmsg, m_bPlus2Comma) )
	{
		MessageBox(szErrmsg);
	}
	else
	{
		MessageBox("CSV -> FK1 �ļ�ת���ɹ�...");
	}
}
//------------------------------------------------------------------------
// ת���ļ���
void CFK_1_MakerDlg::ChangeToSCPName(LPTSTR szSCPName, LPCTSTR szCSVName)
{	
	strcpy(szSCPName, szCSVName);
	char *pPoint = strrchr(szSCPName, '.');
	strcpy( (pPoint+ 1), "FK1" );
}
//------------------------------------------------------------------------
//�򿪱����ļ��Ĵ���
BOOL CFK_1_MakerDlg::MySaveFile(PTSTR szDefSaveFileName, LPTSTR szRealSaveFileName)
{
	CFileDialog FileDialog(FALSE, NULL, szDefSaveFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"FK1��Դ�ļ� (*.FK1)|*.FK1|", this);
	if ( IDOK != FileDialog.DoModal())
	{
		return FALSE;
	}
	CString cstrFileName = FileDialog.GetPathName();
	UpdateData(FALSE);

	strcpy(szRealSaveFileName, cstrFileName.GetBuffer(cstrFileName.GetLength()) );
	cstrFileName.ReleaseBuffer();

	return TRUE;
}
//------------------------------------------------------------------------
void CFK_1_MakerDlg::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bPlus2Comma = !m_bPlus2Comma;
}
//------------------------------------------------------------------------