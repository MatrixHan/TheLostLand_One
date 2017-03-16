/**
*	created:		2013-4-23   15:18
*	filename: 		FKIResource
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKIBase.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace core
	{
		// ������Դ�����Ļ��ࡣ���� �������� ��
		class IResource : public IBase ,public IObjectInfomation,public IRefCount
		{
		public :
			virtual ~IResource(){}
			IResource(const std::wstring & resourceMane):IObjectInfomation(resourceMane){}
			virtual const std::wstring getTypeInfo() const {
				return L"IResource";
			}
		};
	}
}
//------------------------------------------------------------------------