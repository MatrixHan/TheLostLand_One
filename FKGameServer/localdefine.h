/**
*	created:		2013-4-18   23:00
*	filename: 		localdefine
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Scriptdef.h"
#include "StringDef.h"
#include "MapDefine.h"
//------------------------------------------------------------------------
typedef struct tagStringCacheNode
{
	char	szString[256];
}StringCacheNode;
//------------------------------------------------------------------------
// ����ȡ��
#define ROUND(f)				(int)(f+0.5)
// ����ȡ��
#define ROUNDDOWN(f)			(int)(f)
// ���ֵ
#define MAX(i,j)				((i)>(j)?(i):(j))
// MakeWord
#define MAKEDWORD2W( wh, wl )	((((DWORD)wh)<<16)|wl)
// �����߾���
#define	DISTANCE( x1, y1, x2, y2 ) (UINT)(max( abs( (int)(x1) - (x2) ) , abs( (int)(y1) - (y2) ) ))
//------------------------------------------------------------------------