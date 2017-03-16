/**
*	created:		2013-4-23   15:27
*	filename: 		FKResourceMap
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#	if (_MSC_VER <= 1200)
#		pragma warning(disable:4786)
#	endif
//------------------------------------------------------------------------
#include <map>
#include <string>

#include "FKIBase.h"
#include "FKIResource.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace core
	{
		class CResourceMap : public IBase
		{
		public :
			virtual const std::wstring getTypeInfo() const {
				return L"CResourceMap";
			}
			virtual ~CResourceMap();
			CResourceMap();
			//! ����Դ��������һ����Դ ���ڲ������һ�� addRef��
			virtual bool addResource(IResource * resource);
			//! ������Դ����ȡ��Դ
			virtual IResource * getResourceByName(const std::wstring & resourceName);
			//! �ͷ�һ����Դ ���ڲ������һ�� delRef ��
			virtual void freeResource(IResource * resource);	
			virtual void freeResource(const std::wstring & resourceName);
			//! �ͷ�ȫ����Դ
			virtual void freeAllResource();
		private:
			typedef std::map<std::wstring,IResource *> TResourceMap;
			typedef TResourceMap::iterator TResourceMapIterator;
			TResourceMap resourceMap;
		};
	}
}
//------------------------------------------------------------------------