/**
*	created:		2013-5-10   5:48
*	filename: 		FKMEncrypter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKStreamEncrypt.h"
#include "FKCStreamEncrypt.h"
#include "Des.h"
//------------------------------------------------------------------------
// ѭ��У�鿪��
// #define  ENABLE_CRC_CHECK	TRUE
//------------------------------------------------------------------------
class MEncrypter : public IStreamEncrypt
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

	void Clear(){ m_byLastSendFlag = 0; }
	void Release(){	delete this; }

	static void EnableCRC( BOOL bFlag )
	{
		ENABLE_CRC_CHECK = bFlag;
	}

	MEncrypter();
	virtual ~MEncrypter();

private:
	// ����ĳ�����ݵ�CRC��
	BYTE CalculateCRC( const unsigned char * buf , int nLen );

public:
	struct STREAM_KEY
	{
		CStreamEncrypter::KEY	key;		// ��Կ
		BYTE					byCrc;		// ѭ��У����
		BYTE					byFlag;		// ������ʶ
		WORD					wLen;		// �����ݳ���
	};

	typedef  unsigned __int64	DES_KEY;

private:
	static CStreamEncrypter  m_StreamEncrypter;
	static unsigned long	 m_DesKey[256];
	static BOOL				 ENABLE_CRC_CHECK;

	STREAM_KEY		 m_LastStreamKey;
	BYTE			 m_byLastSendFlag;
};
//------------------------------------------------------------------------