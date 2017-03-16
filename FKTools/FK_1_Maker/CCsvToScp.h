/**
*	created:		2012-7-4   20:31
*	filename: 		CCsvToScp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <vector>
#include <fstream>
#include <list>
using namespace std;
//------------------------------------------------------------------------
#define MAX_STRING_LEN		2048
#define MAX_STRING_Name		256
#define MAX_OBJECT_LEVEL	128
//------------------------------------------------------------------------
class CCSVToSCP
{
public:
	CCSVToSCP();

	~CCSVToSCP();
	//��*.csv��ʽ���ļ�ת��Ϊ*.scp��ʽ���ļ�
	BOOL	MakeCSVToSCP(LPCTSTR szCSVFileName, LPCTSTR szSCPFileName, LPTSTR szErrMsg, BOOL bPlus2Comma);

protected:
	int		SaveFirstRow(TCHAR *pwchBuf, int nBufLen, CString *pcstrFirstRow);
	BOOL	IsIDUsed(vector<int> &nVectorID, int nCurID);
	int		PrintConstStr(wofstream &outSCPFile);
	void	PrintTab(wofstream &outSCPFile, int nNum);
	void	PlusToComma(TCHAR *pwchBuf, int nBufLen);

private:
};
//------------------------------------------------------------------------