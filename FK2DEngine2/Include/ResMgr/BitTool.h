/**
*	created:		2012-6-18   20:46
*	filename: 		BitTool
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// ���οɲ�����������ݻ��峤��
#define MAX_DATA_BUFFER		1024*8
//------------------------------------------------------------------------
class CBitTool
{
private:
	BYTE		m_Data[MAX_DATA_BUFFER];	// �ֽ�����
	int			m_nBitPos;					// ��ǰ������λ��λ��
public:
	CBitTool();
	~CBitTool();
public:
	BOOL Push(int nType, void* pData, int nBitsCount);
	BOOL PushFloat(float* f);
	BOOL PushString(LPCSTR szString);
	BOOL PushArray(int nType, void* pArray, int nSize);
	int	 GetBytes(int nBitsCount);
	int GetDataLength();
	void* GetDataBuffer();
	void Reset();
	int GetTypeBytes(int nType);
};
//------------------------------------------------------------------------