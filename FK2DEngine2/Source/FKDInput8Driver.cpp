/**
*	created:		2013-4-23   20:21
*	filename: 		FKDInput8Driver
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/FKLogger.h"
#include "../Include/FKDInput8Device.h"
#include "../Include/FKDInput8Driver.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace input
	{
		CDirectInput8Driver::~CDirectInput8Driver()
		{
		}
		CDirectInput8Driver::CDirectInput8Driver(HWND hwnd_)
		{
			lpdi8 = NULL;
			hwnd = hwnd_;
		}
		bool CDirectInput8Driver::initDriver(unsigned long deviceBits)
		{
			if (FAILED (DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void **) & lpdi8,NULL)))
			{
				LogError( L"����DInput�豸ʧ�ܣ�");
				return false;
			}
			LogInfo( L"DInput�豸��ʼ���ɹ���");
			if (deviceBits & E_INPUT_KEYBOARD)
			{
				LogInfo( L"��ʼ�������豸��");
				CDirectInput8DeviceKeyboard * keyboard_ = new CDirectInput8DeviceKeyboard();
				if ((keyboard_->initKeyboard(lpdi8,hwnd)) == true)
					keyboard = keyboard_;
				else 
				{
					delete keyboard_;
					keyboard = NULL;
				}
			}
			if (deviceBits & E_INPUT_MOUSE)
			{
				LogInfo( L"��ʼ������豸��");
				CDirectInput8DeviceMouse * mouse_ = new CDirectInput8DeviceMouse();
				if ((mouse_->initMouse(lpdi8,hwnd)) == true)
					mouse = mouse_;
				else 
				{
					delete mouse_;
					mouse = NULL;
				}
			}
			return true;
		}
		bool CDirectInput8Driver::shutDriver()
		{
			if (keyboard != NULL)
			{
				((CDirectInput8DeviceKeyboard *)keyboard)->freeKeyboard();
				keyboard = NULL;
			}
			if (mouse != NULL)
			{
				((CDirectInput8DeviceMouse *)mouse)->freeMouse();
				mouse = NULL;
			}
			if (lpdi8 != NULL)
			{
				lpdi8 ->Release();
				lpdi8 = NULL;
				LogInfo( L"DInput �豸�ͷ���ϣ�");	
			}
			return true;
		}
	}
}
//------------------------------------------------------------------------