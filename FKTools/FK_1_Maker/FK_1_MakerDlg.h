
// FK_1_MakerDlg.h : ͷ�ļ�
//

#pragma once


// CFK_1_MakerDlg �Ի���
class CFK_1_MakerDlg : public CDialog
{
// ����
public:
	CFK_1_MakerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FK_1_Maker_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
private:
	CString	m_CStrFileName;													// CSV�ļ�·��
	void ChangeToSCPName(LPTSTR szSCPName, LPCTSTR szCSVName);				// ת���ļ���
	BOOL MySaveFile(PTSTR szDefSaveFileName, LPTSTR szRealSaveFileName);	// �򿪱����ļ��Ĵ���
	BOOL m_bPlus2Comma;														// �Ƿ� + ת��Ϊ ,
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
};
