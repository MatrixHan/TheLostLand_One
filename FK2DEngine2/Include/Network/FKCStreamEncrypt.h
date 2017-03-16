/**
*	created:		2013-5-10   5:44
*	filename: 		FKCStreamEncrypt
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		������Կ��ʾ����λ�����һ������ѭ����λ������
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKStreamEncrypt.h"
//------------------------------------------------------------------------
class CStreamEncrypter :public IStreamEncrypt
{
public:
	// ������Կ
	// pKey		:	��Կ����
	// nLen		:	��Կ����
	BOOL SetKey( const unsigned char * pKey , int nLen );

	// ����һ���ַ���
	// inBuffer :   �������� /*IN*/
	// outBuffer:   ������� /*OUT*/
	// inLen	:   ���ĳ��� /*IN*/
	// outLen	:	���ĳ��� /*IN/OUT*/
	BOOL Encrypt( const unsigned char * inBuffer , 
		unsigned char * outBuffer , 
		int inLen , 
		int & outLen );

	// ����һ���ַ���
	// inBuffer :   �������� /*IN*/
	// outBuffer:   ������� /*OUT*/
	// inLen	:   ���ĳ��� /*IN*/
	// outLen	:	���ĳ��� /*IN/OUT*/
	BOOL Decrypt( const unsigned char * inBuffer , 
		unsigned char * outBuffer , 
		int inLen , 
		int & outLen );

	void Clear() {}
	void Release(){	delete this; }

	CStreamEncrypter() : ROUND_UP_4BYTES( TRUE )
	{
		m_key.lValue = 0;
	}

	// bRndup : �Ƿ�Բ����4�ֽڵı���
	CStreamEncrypter( BOOL bRndup ) : ROUND_UP_4BYTES( bRndup )
	{
		m_key.lValue = 0;
	}

	virtual ~CStreamEncrypter()
	{
	}

public:
	union KEY
	{
		unsigned char	mFlag[4];
		unsigned long   lValue;
	};

	KEY		m_key;
	// �Ƿ�Բ����4�ֽ�
	BOOL	ROUND_UP_4BYTES;
};
//------------------------------------------------------------------------