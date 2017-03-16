/**
*	created:		2013-5-10   5:55
*	filename: 		FKMEncrypter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		�����ݼ�����
*	Dese:			ʵ��ÿ�η�������һ���µ���Կ
*					�������ĸ�ʽ��:
*					0  ���� 31 bits  ��Կ
*					32 ���� 47 bits  �����ʶ����CRCУ��
*					48 ���� 63 bits  ���ݳ��� ���������ǰ�����Կ����
*/
//------------------------------------------------------------------------
#include "../../Include/Network/FKMEncrypter.h"
//------------------------------------------------------------------------
CStreamEncrypter MEncrypter::m_StreamEncrypter;
unsigned long	 MEncrypter::m_DesKey[256];
BOOL MEncrypter::ENABLE_CRC_CHECK = TRUE;
//------------------------------------------------------------------------
MEncrypter::MEncrypter() : m_byLastSendFlag( 0 )
{
	memset( &m_LastStreamKey , 0 , sizeof( m_LastStreamKey ) );
}
//------------------------------------------------------------------------
MEncrypter::~MEncrypter()
{
}
//------------------------------------------------------------------------
// ������Կ
// pKey		:	��Կ����
// nLen		:	��Կ����
BOOL MEncrypter::SetKey( const unsigned char * pKey , int nLen )
{
	des_ky( (void *)pKey , (void *)&m_DesKey[0] );
	return TRUE;
}
//------------------------------------------------------------------------
// ����һ���ַ���
// inBuffer :   �������� /*IN*/
// outBuffer:   ������� /*OUT*/
// inLen	:   ���ĳ��� /*IN*/
// outLen	:	���ĳ��� /*IN/OUT*/
BOOL MEncrypter::Encrypt( const unsigned char * inBuffer , 
						 unsigned char * outBuffer , 
						 int inLen , 
						 int & outLen )
{
	// �����������
	STREAM_KEY	tempKey;
	srand( GetTickCount() );
	tempKey.key.mFlag[0] = rand()%256;
	tempKey.key.mFlag[1] = rand()%256;
	tempKey.key.mFlag[2] = rand()%256;
	tempKey.key.mFlag[3] = rand()%256;

	tempKey.wLen = inLen;

	if( m_byLastSendFlag == 255 )
		m_byLastSendFlag = 0;
	else
		m_byLastSendFlag ++;

	tempKey.byFlag = m_byLastSendFlag;

	// ѭ��У��
	if( ENABLE_CRC_CHECK )
	{
		tempKey.byCrc = CalculateCRC( inBuffer , inLen );
	}

	STREAM_KEY	Key;
	des_ec( (const void *)&tempKey, (void *)&Key,  (void *)&m_DesKey[0] );

	if( outLen < sizeof( STREAM_KEY ) )
		return FALSE;

	memcpy( outBuffer , &Key , sizeof( STREAM_KEY ) );
	outBuffer += sizeof( STREAM_KEY );
	int tempLen = outLen - sizeof( STREAM_KEY );
	m_StreamEncrypter.SetKey( (const unsigned char *)&tempKey.key , sizeof(tempKey.key) ); 
	if( !m_StreamEncrypter.Encrypt( inBuffer , outBuffer , inLen , tempLen ) )
		return FALSE;

	outLen = tempLen + sizeof( STREAM_KEY );
	return TRUE;
}
//------------------------------------------------------------------------
// ����һ���ַ���
// inBuffer :   �������� /*IN*/
// outBuffer:   ������� /*OUT*/
// inLen	:   ���ĳ��� /*IN*/
// outLen	:	���ĳ��� /*IN/OUT*/
BOOL MEncrypter::Decrypt( const unsigned char * inBuffer , 
						 unsigned char * outBuffer , 
						 int inLen , 
						 int & outLen )
{
	if( inLen < sizeof( m_LastStreamKey ) )
		return FALSE;

	STREAM_KEY	tempKey;
	des_dc( (const void *)inBuffer , (void *)&tempKey , (void *)&m_DesKey[0] );

	// ����У��
	int nDataLen = tempKey.wLen + sizeof( STREAM_KEY );
	if( inLen < nDataLen || nDataLen > nDataLen + 4 )
		return FALSE;

	// �����������
	if( tempKey.byFlag == 0 )
	{
		if( m_LastStreamKey.byFlag != 255 )
			return FALSE;
	}else
	{
		if( tempKey.byFlag - m_LastStreamKey.byFlag != 1 )
			return FALSE;
	}

	// ����
	inBuffer += sizeof( STREAM_KEY );
	inLen -= sizeof( STREAM_KEY );
	m_StreamEncrypter.SetKey( (const unsigned char * )&tempKey.key , sizeof(tempKey.key) ); 
	if( !m_StreamEncrypter.Decrypt( inBuffer , outBuffer , inLen , outLen ) )
		return FALSE;

	// ѭ��У��
	if( ENABLE_CRC_CHECK )
	{
		if( CalculateCRC( outBuffer , tempKey.wLen ) != tempKey.byCrc )
			return FALSE;
	}

	outLen = tempKey.wLen;
	m_LastStreamKey = tempKey;
	return TRUE;
}
//------------------------------------------------------------------------
// ����ĳ�����ݵ�CRC��
BYTE MEncrypter::CalculateCRC( const unsigned char * buf , int nLen )
{
	unsigned long ltemp = 0;
	for( int i = 0 ; i <= nLen - 4 ; i += 4 )
	{
		ltemp += *((unsigned long *)&buf[i]);
	}

	return (BYTE)ltemp;
}
//------------------------------------------------------------------------