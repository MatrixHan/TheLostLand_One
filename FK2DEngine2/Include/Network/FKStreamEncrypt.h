/**
*	created:		2013-5-10   2:18
*	filename: 		FKStreamEncrypt
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		��ʽ���ݵļ���
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <winsock2.h>
//------------------------------------------------------------------------
struct IStreamEncrypt
{
	// ������Կ
	// pKey		:	��Կ����
	// nLen		:	��Կ����
	virtual BOOL SetKey( const unsigned char * pKey , int nLen ) = 0;

	// ����һ���ַ���
	// inBuffer :   �������� /*IN*/
	// outBuffer:   ������� /*OUT*/
	// inLen	:   ���ĳ��� /*IN*/
	// outLen	:	���ĳ��� /*IN/OUT*/
	virtual BOOL Encrypt( const unsigned char * inBuffer , 
		unsigned char * outBuffer , 
		int inLen , 
		int & outLen ) = 0;

	// ����һ���ַ���
	// inBuffer :   �������� /*IN*/
	// outBuffer:   ������� /*OUT*/
	// inLen	:   ���ĳ��� /*IN*/
	// outLen	:	���ĳ��� /*IN/OUT*/
	virtual BOOL Decrypt( const unsigned char * inBuffer , 
		unsigned char * outBuffer , 
		int inLen , 
		int & outLen ) = 0;

	// ���
	virtual void Clear() = 0;

	// ����
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------