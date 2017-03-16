/**
*	created:		2012-6-18   20:00
*	filename: 		BinObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CBinObject
{
private:
	WORD				m_wSize;		// ���ݻ���Ĵ�С
	WORD				m_wOffset;		// ���������ݵ�ƫ��
	BYTE*				m_ObjectBuffer; // ���󻺳�
	vector<DWORD>		m_SubObjectList;// �Ӷ����б�
	vector<WORD>		m_UnFixSizeList;// ���������Գ����б�

public:
	CBinObject();
	~CBinObject();

	BYTE* GetBuffer()				{return m_ObjectBuffer;}
	DWORD GetSize()					{return m_wSize;}
	DWORD GetSubObjectCount()		{return m_SubObjectList.size();}
	DWORD GetUnFixPropertyCount()	{return m_UnFixSizeList.size();}

	int   GetUnFixPropertyOffset(int nIndexEx);
	int   GetUnFixPropertyLength(int nIndexEx);
	UINT  GetSubObject(int nIndex);
	BOOL  Release();

	BOOL SetSize(int nSize)	{return (m_wSize=nSize);}
	BOOL SetUnFixPropertyOffset(int nOffset) {return (m_wOffset=nOffset);}
	BOOL SetIDList(UINT* pIDList, int nSize);
	BOOL SetUnFixSizeList(WORD* pSizeList, int nSize);
	BOOL SetObjectBuffer(BYTE* pData, int nSize);
};
//------------------------------------------------------------------------