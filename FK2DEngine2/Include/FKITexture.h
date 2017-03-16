/**
*	created:		2013-4-23   15:45
*	filename: 		FKITexture
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKIResource.h"
#include "FKEVideoDriverType.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace video
	{
		class ITexture : public FK2DEngine2::core::IResource
		{
		public :
			typedef unsigned int HTEXTURE;
			virtual ~ITexture(){}
			ITexture(const std::wstring & textureName):FK2DEngine2::core::IResource(textureName){}
			virtual const std::wstring getTypeInfo() const {
				return L"ITexture";
			}
			//! ��ȡ������
			virtual HTEXTURE getHandle() = 0;
			//! ��ȡ��Ⱦ������
			virtual E_VIDEODRIVERTYPE getDriverType() = 0;
			//! \return ��ȡ������
			virtual unsigned int getTextureWidth() const = 0;
			//! \return ��ȡ����߶�
			virtual unsigned int getTextureHeight() const = 0;
		};
	}
}
//------------------------------------------------------------------------