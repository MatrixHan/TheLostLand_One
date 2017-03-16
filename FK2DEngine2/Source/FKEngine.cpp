/**
*	created:		2013-4-23   23:16
*	filename: 		FKEngine
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/FKEngine.h"
//------------------------------------------------------------------------
using namespace FK2DEngine2::core;
using namespace FK2DEngine2::video;
using namespace FK2DEngine2::system;
using namespace FK2DEngine2::input;
using namespace FK2DEngine2::audio;
using namespace FK2DEngine2::scene::S2D;
using namespace FK2DEngine2::scene::S3D;
//------------------------------------------------------------------------
#pragma warning( disable:4244 )
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	//------------------------------------------------------------------------
	CEngine::CEngine()
	{
		// window :
		windowWidth = 1024;
		windowHeight = 768;
		windowStyle  = WS_OVERLAPPEDWINDOW;
		windowName = L"FK_Window";

		// video :
		zdepth = 255;
		fullscreen = false;
		zbuffer = true;
		alphatesting = false;
		alphablend = true;
		lighting = false;
		
		// input :
		initKeyboard = true;
		initMouse = true;

		// timer :
		fps = 60.0f;
	}
	
	CEngine::~CEngine()
	{
	}

	bool CEngine::initEngine()
	{
		bool result = false;

		// Log init
		FKLogger::SFileLoggerInitParams tagParams;
		tagParams.m_pFileName = "FKLog.txt";
		if( InitFKLogger( FKLogger::ELOM_File, (void*)&tagParams ) < 0 )
		{
			return false;
		}

		// Timer init
		timer = new CWin32Timer();
		if( timer == NULL )
		{
			LogError( L"����ʱ��ϵͳʧ��..." );
			return false;
		}
		timer->initFrameRate(fps);

		// Window init
		HINSTANCE hInstance = GetModuleHandle(NULL);
		window = new CWin32Window(windowName,hInstance,SW_SHOW,windowWidth,windowHeight,windowStyle);
		if( window == NULL )
		{
			LogError( L"����Windows����ʧ��..." );
			return false;
		}
		
		// Trace init
		traceWin = new CFKTrace();
		BOOL bRet = FALSE;
		bRet = traceWin->Create( hInstance, OS_WINDOW | OS_FILE, (HWND)window->getHandle(), NULL,
			L"FKGameLog.txt", false );
		if( !bRet )
		{
			LogError( L"����Trace����ʧ��..." );
			return false;
		}

		// Res System init
		resMgr = CIResMgrHelper::GetInstance()->Create( (ITrace*)traceWin );
		if( resMgr == NULL )
		{
			LogError( L"������Դ����ϵͳʧ��..." );
			return false;
		}

		// Video Driver init
		HWND hwnd = (HWND)window->getHandle();
		videoDriver = new CDirect3D9VideoDriver();
		result = videoDriver->initDriver(windowWidth,windowHeight,zdepth,hwnd,fullscreen,zbuffer,alphatesting,alphablend,lighting);
		if ( !result )
		{
			LogError( L"������Ⱦ�豸ʧ��..." );
			return false;
		}
		
		// 2D Scene Manager init
		i2DSceneManager = (FK2DEngine2::scene::S2D::I2DSceneManager *)theWorldPtr;
		result = i2DSceneManager->init2DSceneManager(videoDriver,6000,windowWidth,windowHeight,zdepth);
		if ( !result )
		{
			LogError( L"��ʼ��2D�����������ʧ��..." );
			return false;
		}

		// 3D Scene Manger init
		i3DSceneManager = new C3DSceneManager(videoDriver);
		if( i3DSceneManager == NULL )
		{
			LogError( L"����3D�����������ʧ��..." );
			return false;
		}

		// Input Driver init
		inputDriver = new CDirectInput8Driver(hwnd);
		if( inputDriver == NULL )
		{
			LogError( L"����������ƹ���������ʧ��..." );
			return false;
		}
		unsigned long inputInitState = 0;
		if (initKeyboard == true) inputInitState |= E_INPUT_KEYBOARD;
		if (initMouse == true) inputInitState |= E_INPUT_MOUSE;
		result = inputDriver->initDriver(inputInitState);
		if ( !result )
		{
			LogError( L"��ʼ��������ƹ���������ʧ��..." );
			return false;
		}
		inputDriver->refreshDriverState();

		// Audio Driver init 
		audioDriver = new CFmodAudioDriver();
		if( audioDriver == NULL )
		{
			LogError( L"������Ƶ�豸����������ʧ��..." );
			return false;
		}
		result = audioDriver->initDriver();
		if ( !result )
		{
			LogError( L"��ʼ����Ƶ�豸����������ʧ��..." );
			return false;
		}

		// Resource Map init
		resourceMap = new CResourceMap();
		if( resourceMap == NULL )
		{
			LogError( L"������Դ�����ʧ��..." );
			return false;
		}
		else
		{
			LogInfo( L"������Դ�����ɹ�..." );
		}
		videoDriver->setResourceMap(resourceMap);
		audioDriver->setResourceMap(resourceMap);
		LogInfo( L"������Դ�����ɹ�..." );

		// UI init
		uiRenderDriver = new FK2DEngine2::video::DirectX9( videoDriver );
		if( uiRenderDriver == NULL )
		{
			LogError( L"����UI�豸����������ʧ��..." );
			return false;
		}
		else
		{
			LogInfo( L"����UI�豸����������ɹ�..." );
		}
		uiSkin = new FK2DEngine2::Skin::TexturedBase( uiRenderDriver );
		if( uiSkin == NULL )
		{
			LogError( L"����Ĭ��UIƤ������ʧ��..." );
			return false;
		}
		else
		{
			LogInfo( L"����Ĭ��UIƤ������ɹ�..." );
		}
		uiSkin->Init( L"Res\\DefaultSkin.png" );
		LogInfo( L"����Ĭ��UIƤ������ɹ�..." );
		RECT FrameBounds;
		GetClientRect( hwnd, &FrameBounds );
		uiCanvas = new FK2DEngine2::Controls::Canvas( uiSkin );
		if( uiCanvas == NULL )
		{
			LogError( L"����UICanvas����ʧ��..." );
			return false;
		}
		else
		{
			LogInfo( L"����UICanvas����ɹ�..." );
		}
		uiCanvas->SetSize( FrameBounds.right, FrameBounds.bottom );

		// UIInput init
		uiInput = new FK2DEngine2::Input::Windows();
		if( uiInput == NULL )
		{
			LogError( L"����Ĭ��UI�������������ʧ��..." );
			return false;
		}
		else
		{
			LogInfo( L"����Ĭ��UI�������������ɹ�..." );
		}
		uiInput->Initialize( uiCanvas );
		LogInfo( L"��ʼ��UI�������������ɹ�..." );
		LogInfo( L"--------------------------------");
		LogInfo( L"��Notice����ʼ�������ں˶���ȫ���ɹ�");

		return true;
	}
	bool CEngine::runEngine()
	{
		timer->frameDelay();
		if (!window->peekMessage( uiInput ))
			return false;
		audioDriver->updateDriver();
		return true;
	}

	bool CEngine::shutEngine()
	{
		// UIInput
		if( uiInput != NULL )
		{
			delete uiInput;
			uiInput = NULL;
		}

		// UICanvas
		if( uiCanvas != NULL )
		{
			delete uiCanvas;
			uiCanvas = NULL;
		}

		// UISkin
		if( uiSkin != NULL )
		{
			// delete uiSkin;
			uiSkin = NULL;
		}

		// UIRender
		if( uiRenderDriver != NULL )
		{
			delete uiRenderDriver;
			uiRenderDriver = NULL;
		}

		// resourceMap 
		if (resourceMap != NULL)
		{
			resourceMap->freeAllResource();
			delete resourceMap;
			resourceMap = NULL;
		}
		// audio
		if (audioDriver != NULL)
		{
			audioDriver->shutDriver();
			delete audioDriver;
			audioDriver = NULL;
		}
		// input 
		if (inputDriver != NULL)
		{
			inputDriver->shutDriver();
			delete inputDriver;
			inputDriver = NULL;
		}
		// scene
		if (i3DSceneManager != NULL)
		{
			delete i3DSceneManager;
			i3DSceneManager = NULL;
		}
		if (i2DSceneManager != NULL)
		{
			i2DSceneManager->shut2DSceneManager();
			delete i2DSceneManager;
			i2DSceneManager = NULL;
		}
		// video 
		if (videoDriver != NULL)
		{
			videoDriver->shutDriver();
			delete videoDriver;
			videoDriver = NULL;
		}

		// trace
		if( traceWin != NULL )
		{
			traceWin->Close();
			 delete traceWin;
			traceWin = NULL;
		}

		// window
		if (window != NULL)
		{
			delete window;
			window = NULL;
		}
		// timer
		if (timer != NULL)
		{
			delete timer;
		}

		// log
		DeInitFKLogger();

		// engine
		delete this;
		return true;
	}

	void CEngine::setEngineStateB(E_ENGINE_STATE state,bool value)
	{
		switch (state)
		{
		case E_ENGINE_STATE_FULLSCREEN :
			fullscreen = value;
			break;
		case E_ENGINE_STATE_ZBUFFER:
			zbuffer = value;
			break;
		case E_ENGINE_STATE_ALPHATESTING:
			alphatesting = value;
			break;
		case E_ENGINE_STATE_ALPHABLEND:
			alphablend = value;
			break;
		case E_ENGINE_STATE_LIGHTING:
			lighting = value;
			break;
		case E_ENGINE_STATE_INITKEYBOARD:
			initKeyboard = value;
			break;
		case E_ENGINE_STATE_INITMOUSE:
			initMouse = value;
			break;
		}
	}
	void CEngine::setEngineStateI(E_ENGINE_STATE state,unsigned long value)
	{
		switch (state)
		{
		case E_ENGINE_STATE_WINDOWWIDTH:
			windowWidth = value;
			break;
		case E_ENGINE_STATE_WINDOWHEIGHT:
			windowHeight = value;
			break;
		case E_ENGINE_STATE_WINDOWSTYLE:
			windowStyle = value;
			break;
		case E_ENGINE_STATE_WINDOWFUNC:
			CWin32Window::winFunc = (FK2DEngine2::system::TMessageFunc)(value);
			break;
		case E_ENGINE_STATE_ZDEPTH:
			zdepth = value;
			break;
		}
	}
	void CEngine::setEngineStateF(E_ENGINE_STATE state,float value)
	{
		switch (state)
		{
		case E_ENGINE_STATE_FPS :
			fps = value;
			break;
		}
	}
	void CEngine::setEngineStateStr(E_ENGINE_STATE state,const std::wstring & str)
	{
		switch(state)
		{
		case E_ENGINE_STATE_WINDOWNAME:
			windowName = str.c_str();
			break;
		}
	}

	bool CEngine::getEngineStateB(E_ENGINE_STATE state)
	{
		switch (state)
		{
		case E_ENGINE_STATE_FULLSCREEN:
			return fullscreen;
		case E_ENGINE_STATE_ZBUFFER:
			return zbuffer;
		case E_ENGINE_STATE_ALPHATESTING:
			return alphatesting;
		case E_ENGINE_STATE_ALPHABLEND:
			return alphablend;
		case E_ENGINE_STATE_LIGHTING:
			return lighting;
		case E_ENGINE_STATE_INITKEYBOARD:
			return initKeyboard;
		case E_ENGINE_STATE_INITMOUSE:
			return initMouse;
		}
		return false;
	}
	unsigned long CEngine::getEngineStateI(E_ENGINE_STATE state)
	{
		switch (state)
		{
		case E_ENGINE_STATE_WINDOWWIDTH:
			return windowWidth;
		case E_ENGINE_STATE_WINDOWHEIGHT:
			return windowHeight;
		case E_ENGINE_STATE_WINDOWSTYLE:
			return windowStyle;
		case E_ENGINE_STATE_WINDOWFUNC:
			return (unsigned long)(CWin32Window::winFunc);
		case E_ENGINE_STATE_ZDEPTH:
			return zdepth;
		}
		return 0;
	}
	float CEngine::getEngineStateF(E_ENGINE_STATE state)
	{
		switch(state)
		{
		case E_ENGINE_STATE_FPS:
			return fps;
		}
		return 0.0f;
	}
	const std::wstring CEngine::getEngineStateStr(E_ENGINE_STATE state)
	{
		switch (state)
		{
		case E_ENGINE_STATE_WINDOWNAME:
			return windowName;
		}
		return L"";
	}
}
//------------------------------------------------------------------------