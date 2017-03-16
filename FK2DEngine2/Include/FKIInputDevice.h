/**
*	created:		2013-4-23   20:00
*	filename: 		FKIInputDevice
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKIBase.h"
#include "FKVector3.h"
#include "FKVector4.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace input
	{
		typedef enum
		{
			E_BUTTON_DOWN,
			E_BUTTON_UP,
			E_BUTTON_PRESSED
		}E_BUTTON_STATE;

		class IInputDevice : public FK2DEngine2::core::IBase
		{
		public :
			virtual const std::wstring getTypeInfo() const {
				return L"IInputDevice";
			}
			virtual ~IInputDevice(){}
		
			virtual bool refreshDeviceState() = 0; 
			virtual void * getDeviceData() = 0;
		};

		class IInputDeviceKeyboard : public IInputDevice
		{
		public:
			virtual const std::wstring getTypeInfo() const {
				return L"IInputDeviceKeyboard";
			}
			//! ��ȡ����״̬
			//!\params keyCode : �������ţ��μ�FKKeyCode.h
			//!\params type: ����״̬
			//!\params type: E_BUTTON_DOWN			�ü��Ƿ�ոհ���
			//!\params type: E_BUTTON_UP			�ü��Ƿ�ո��ͷ�
			//!\params type: E_BUTTON_PRESSED		�ü��Ƿ��ڰ���״̬
			virtual bool getKeyState(int keyCode,E_BUTTON_STATE type) = 0;
		};

		typedef enum 
		{
			E_MOUSE_BUTTON_LEFT = 0,
			E_MOUSE_BUTTON_RIGHT = 1,
			E_MOUSE_BUTTON_MIDDLE = 2
		}E_MOUSE_BUTTONS;

		class IInputDeviceMouse : public IInputDevice
		{
		public :
			virtual const std::wstring getTypeInfo() const {
				return L"IInputDeviceMouse";
			}
			//! ��ȡ���״̬
			//\return : ������λ��
			virtual FK2DEngine2::math::Vector3 getMouseRelativePos() = 0;
			//! ��ȡ��갴ť״̬
			//!\params keyCode : ��갴������ E_MOUSE_BUTTONS
			//!\params type: ����״̬
			//!\params type: E_BUTTON_DOWN			�ü��Ƿ�ոհ���
			//!\params type: E_BUTTON_UP			�ü��Ƿ�ո��ͷ�
			//!\params type: E_BUTTON_PRESSED		�ü��Ƿ��ڰ���״̬
			virtual bool getMouseButtonState(int keyCode,E_BUTTON_STATE type) = 0;
		};
	}
}
//------------------------------------------------------------------------