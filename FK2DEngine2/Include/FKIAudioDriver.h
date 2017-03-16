/**
*	created:		2013-4-23   21:19
*	filename: 		FKIAudioDriver
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once

#include "FKIBase.h"
#include "FKResourceMap.h"

#include "FKIChannel.h"
#include "FKISound.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace audio
	{
		typedef enum
		{
			E_SOUND_TYPE_SAMPLE,
			E_SOUND_TYPE_STREAM
		}E_SOUND_TYPE;

		class IAudioDriver : public FK2DEngine2::core::IBase
		{
		public :
			virtual const std::wstring getTypeInfo() const {
				return L"IAudioDriver";
			}
			virtual ~IAudioDriver(){}
			IAudioDriver(){}

			//! ��ʼ����Ƶ�豸
			virtual bool initDriver() = 0;

			//! ������Ƶ�豸
			virtual bool updateDriver() = 0;

			//! �ر���Ƶ�豸
			virtual bool shutDriver() = 0;

			//! ��������
			//! \return �������������Channel
			//! \params volume : 0.0f - 1.0f �����Ĵ�С
			//! \params pan : -1.0f - 1.0f -1.0f: ������, 0.0f ����, 1.0f ������
			//! \params loopCount : ѭ�����Ŵ�������-1 ��Զ��0 һ�Ρ�
			//! \params getChannel : �Ƿ�Ҫ���ȡ��Channel�ľ����������ֶ���ȡ�˸þ����������Чʱ����Ҫ�ֶ����� release() �����ͷ�����
			virtual IChannel * playSound(ISound * sound,float volume = 1.0f,float pan = 0.0f,int loopCount = 0,bool getChannel = false) = 0;

			//! ������Ч
			//! \params type: E_SOUND_TYPE_SAMPLE ��Ϊһ���ļ���������
			//! \params type: E_SOUND_TYPE_STREAM ��Ϊһ�����ļ���������
			virtual ISound * loadSound(const std::wstring & soundName,const std::wstring & fileName,E_SOUND_TYPE type) = 0;

			//! �ͷ�����
			//! \return ����ҵ��˸����������ͷŵ����򷵻�true
			virtual bool freeSound(const std::wstring & soundName) = 0;

			//! �ͷ�����
			//! \return ����ҵ��˸����������ͷŵ����򷵻�true
			virtual bool freeSound(ISound * sound) = 0;

		public:
			FK2DEngine2::core::CResourceMap * getResourceMap(){
				return resourceMap;
			}
			FK2DEngine2::core::CResourceMap * setResourceMap(FK2DEngine2::core::CResourceMap * resourceMap_){
				FK2DEngine2::core::CResourceMap * ret = resourceMap;
				resourceMap = resourceMap_;
				return ret;
			}
		protected: 
			FK2DEngine2::core::CResourceMap * resourceMap;
		};
	}
}
//------------------------------------------------------------------------