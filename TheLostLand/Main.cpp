/**
*	created:		2013-5-1   20:58
*	filename: 		Main
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "MainMacro.h"
#include "MainImpl.h"
#include "../depend/vld2.3/include/vld.h"
//------------------------------------------------------------------------
//#ifdef _DEBUG  
//#include <crtdbg.h>  
//#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)  
//#else  
//#define DEBUG_NEW new  
//#endif 
////------------------------------------------------------------------------
//#ifdef _DEBUG   
//#define new DEBUG_NEW  
//#endif  
//------------------------------------------------------------------------
IEngine *						g_pEngine = NULL;
IVideoDriver *					g_pVideoDriver = NULL;
I2DSceneManager *				g_p2DSceneMgr = NULL;
IInputDriver *					g_pInputDriver = NULL;
CFKTrace *						g_pTrace = NULL;
IResMgr*						g_pResMgr = NULL;

bool							g_bReadyExit = false;
//------------------------------------------------------------------------
// �Լ��ĳ�ʼ��
bool MyInit();
// ��ѭ��
bool MyUpdate();
// �Լ����ͷ�
bool MyShut();
//------------------------------------------------------------------------
int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	// ����DEBUG�ڴ�й¶���
	_CrtDumpMemoryLeaks();
	// ������������������ͬ�Ľ���
	if(!FKProcessLimitGuard<1>::Instance()->IsOwned())
	{
		MessageBox(0, L"�Բ��𣬱���Ϸ��֧��˫���Լ��࿪��", L"FreeKnightGame��ʾ", 0);
		return 0;
	}

	g_pEngine = GetEngine();
	if( !g_pEngine )
	{
		return -1;
	}
	g_pEngine->setEngineStateStr( E_ENGINE_STATE_WINDOWNAME, L"��ʧ֮�� Version 1.0 by FK2DEngine2");
	if( !g_pEngine->initEngine())
	{
		return -2;
	}
	if( !MyInit() )
	{
		return -3;
	}

	// ��ѭ��
	while (g_pEngine->runEngine())
	{
		// �Լ���ÿ֡ѭ��
		if (!MyUpdate())
		{
			break;
		}
	}

	if( !MyShut() )
	{
		return -4;
	}
	if( !g_pEngine->shutEngine())
	{
		return -5;
	}
	return 0;
}
//------------------------------------------------------------------------
// �Լ��ĳ�ʼ��
bool MyInit()
{
	// ��ʼ�������豸��Դ
	g_pVideoDriver = g_pEngine->getVideoDriver();
	g_pInputDriver = g_pEngine->getInputDriver();
	g_p2DSceneMgr = g_pEngine->get2DSceneManager();
	g_pTrace = g_pEngine->getTraceWin();
	g_pResMgr = g_pEngine->getResMgr();

	// ��ȡ����
	if( !InitConfig() )
	{
		FKTRACE( L"���������ļ�ʧ��..." );
		return false;
	}

	// ����
	g_p2DSceneMgr->SetSceneMgr( CGameSceneMgr::GetInstancePtr() );
	g_p2DSceneMgr->GetSceneMgr()->SetCurScene( eScene_LoginScene );

	return true;
}
//------------------------------------------------------------------------
// ��ѭ��
bool MyUpdate()
{
	g_pVideoDriver->beginScene(SColor::orange(),E_CLEAR_COLOR|E_CLEAR_ZBUFFER);
	{
		// ÿ֡ˢ��Input�豸״̬��ʹ��DInputֻ�ܻ�ȡ��֡��ƫ�ơ�
		g_pInputDriver->refreshDriverState();

		// ��Ⱦ2D����
		g_p2DSceneMgr->begin2DScene();
		g_p2DSceneMgr->drawAll( g_pEngine->getUICanvas() );
		g_p2DSceneMgr->end2DScene();

		// ��ȾUI
		g_p2DSceneMgr->renderUI( g_pEngine->getUICanvas() );
	}
	// �ر���Ⱦ
	g_pVideoDriver->endScene();

	if( g_bReadyExit )
	{
		return false;
	}

	return true;
}
//------------------------------------------------------------------------
// �Լ����ͷ�
bool MyShut()
{
	return true;
}
//------------------------------------------------------------------------