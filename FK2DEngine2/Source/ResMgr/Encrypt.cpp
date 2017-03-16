/**
*	created:		2012-6-18   20:24
*	filename: 		Encrypt
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../Include/ResMgr/ResMgrCommon.h"
#include "../../Include/ResMgr/Encrypt.h"
//------------------------------------------------------------------------
CEncrypt::CEncrypt()
{

}
//------------------------------------------------------------------------
CEncrypt::~CEncrypt()
{

}
//------------------------------------------------------------------------
BOOL CEncrypt::Encrypt(BYTE* pBuffer, int nSize, BYTE* pKey)
{
	return MakeSimpleMap(pBuffer, nSize);
}
//------------------------------------------------------------------------
BOOL CEncrypt::Decrypt(BYTE* pBuffer, int nSize, BYTE* pKey)
{
	return MakeSimpleMap(pBuffer, nSize);
}
//------------------------------------------------------------------------
BOOL CEncrypt::Encrypt(BYTE* pInput, BYTE* pOutput, int nSize, BYTE* pKey)
{
	memcpy(pOutput, pInput, nSize);
	return MakeSimpleMap(pOutput, nSize);
}
//------------------------------------------------------------------------
BOOL CEncrypt::Decrypt(BYTE* pInput, BYTE* pOutput, int nSize, BYTE* pKey)
{
	memcpy(pOutput, pInput, nSize);
	return MakeSimpleMap(pOutput, nSize);
}
//------------------------------------------------------------------------
// �����������
// �ٶȲ��Խ��:һ��130K��buffer����10000��MakeSimpleMap��������ʱ1000ms
BOOL CEncrypt::MakeSimpleMap(BYTE* buf, int nLen)
{
	__asm{
		// eax:(nLen/8)
		// ebx:β������8�ֽڶ����ʣ���С��8�ֽڵ��ֽ�����ռ��λ��
		// ecx:64-ebx
		// mm7:key
		mov esi, dword ptr buf;
		mov eax, nLen;
		pcmpeqd mm7, mm7;
		movd mm6, eax;
		mov ebx, eax;
		psllq mm6, 32;
		movd mm5, ebx;
		and ebx, 7;
		paddd mm6, mm5;
		shr eax, 3;
		pxor mm7, mm6;
		align 4;
_loop:	
		cmp eax, 0;
		jz _tail;
		movq mm0, [esi];
		pxor mm0, mm7;
		movq [esi], mm0;
		add esi,8;
		dec eax;
		jmp _loop;
_tail:
		cmp ebx, 0;
		jz _done;

		// ����β��������ֽ�
		pcmpeqd mm1, mm1;
		pcmpeqd mm2, mm2;
		shl ebx, 3;
		mov ecx, 64;
		movq mm0, [esi];
		sub ecx, ebx;
		movd mm3, ebx;
		movd mm4, ecx;
		psllq mm1, mm3;
		psrlq mm2,mm4;
		pand mm1, mm0;
		por mm1, mm2;
		pxor mm0, mm7;
		pand mm0, mm1;
		movq [esi], mm0;
_done:
		emms;
	}
	return TRUE;
}
//------------------------------------------------------------------------