/**
*	created:		2013-4-23   14:29
*	filename: 		FKMathMacro
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <cmath>
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace math
	{
	
		#ifndef DWORD
			typedef unsigned long       DWORD;
			typedef unsigned short      WORD;
			typedef unsigned char       BYTE;
		#endif
		
		#ifndef M_PI
		#define M_PI	3.14159265358979323846f
		#define M_PI_2	1.57079632679489661923f
		#define M_PI_4	0.785398163397448309616f
		#define M_1_PI	0.318309886183790671538f
		#define M_2_PI	0.636619772367581343076f
		#endif

		#define MIN_FLOAT_ABS 0.000001f

		//! /return ��������ֵ�Ƿ����
		inline bool equalsf(float f1,float f2){
			if (fabs(f1 - f2) < MIN_FLOAT_ABS)
				return true;
			return false;
		}

		//! /return ���������е�Сֵ
		inline float minf(float f1,float f2){
			if (f1 < f2) 
				return f1;
			return f2;
		}

		//! /return ���������еĴ�ֵ
		inline float maxf(float f1,float f2){
			if (f1 > f2) 
				return f1;
			return f2;
		}

		//! �������������е�ֵ
		inline void swapf(float & f1,float & f2){
			float temp = f1;
			f1 = f2;
			f2 = temp;
		}
	}
}
//------------------------------------------------------------------------