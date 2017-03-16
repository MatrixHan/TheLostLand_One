/**
*	created:		2013-4-23   21:14
*	filename: 		FKIChannel
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
	namespace audio
	{
		class IChannel : public FK2DEngine2::core::IBase
		{
		public:
			typedef unsigned long HCHANNEL;
			virtual const std::wstring getTypeInfo() const {
				return L"IChannel";
			}

			//! �ͷű�Channel
			void release(){
				// Channel�ѱ�Audio��������Կ���ֱ��Delete
				delete this;
			}

			//! \return Channel������
			virtual float getVolume() = 0;

			//! \return Channel��ƽ��λ�� -1.0f - 1.0f ��-1.0f: ������, 0.0f ����, 1.0f ��������
			virtual float getPan() = 0;

			//! ��ȡѭ������
			virtual int getLoopCount() = 0;

			//! �������� 0.0f -1.0f
			virtual void setVolume(float volume) = 0;

			//! ����ƽ��λ�� -1.0f - 1.0f ��-1.0f: ������, 0.0f ����, 1.0f ��������
			virtual void setPan(float pan) = 0;

			//! ����ѭ������
			virtual void setLoopCount(int loopCount) = 0;

			//! \return ���ڲ�����᷵��true
			virtual bool isPlaying() = 0;

			//! ��ͣ
			virtual void setPaused(bool pause) = 0;

			//! ֹͣ
			virtual void stop() = 0;

			//! ��Channel�Ƿ���Ч
			virtual bool isValid() = 0;

			//! ��ȡHandle���
			virtual HCHANNEL getHandle() = 0;
		};
	}
}
//------------------------------------------------------------------------