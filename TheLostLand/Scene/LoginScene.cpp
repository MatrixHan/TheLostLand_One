/**
*	created:		2013-5-1   22:40
*	filename: 		LoginScene
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "LoginScene.h"
//------------------------------------------------------------------------
CLoginScene::CLoginScene()
	 : IScene()
{
	memset( m_szAccout, 0, sizeof(m_szAccout));
	memset( m_szPassword, 0, sizeof(m_szPassword));

	m_pUIMgr				= NULL;
	m_pBackgroud			= NULL;

	m_pBackgroud = new CFullScreenActor();
	m_pBackgroud->SetSize( GAME_CANVAS_WIDTH, GAME_CANVAS_HEIGHT );
	m_pBackgroud->SetSprite( LOGIN_SCENE_BK_FILE );

	// ������Ϣ
	theSwitchboard.SubscribeTo( this, L"EnterCharSel" );
}
//------------------------------------------------------------------------
CLoginScene::~CLoginScene()
{

}
//------------------------------------------------------------------------
void CLoginScene::OnEnter()
{	
	FKTRACE( L"���� LoginScene" );
	// ����Ping
	FK2DEngine2::Ping::Init();

	// UI����
	if( m_pUIMgr == NULL )
	{
		m_pUIMgr = new CLoginSceneUI( g_pEngine->getUICanvas(), "LoginScene" );
	}
	else
	{
		m_pUIMgr->Init();
		m_pUIMgr->Show();
	}

	m_pUIMgr->SetOwner( this );

	// �������鴦��
	theWorld.Add( m_pBackgroud, DEFAULT_BACKGROUND_LAYER_Z );

	// ���Ӵ���
	CGameSceneMgr::GetInstance().CreateConnection( eNLS_LoginSvr );
}
//------------------------------------------------------------------------
void CLoginScene::Render()
{

}
//------------------------------------------------------------------------
void CLoginScene::Update( float dt )
{

}
//------------------------------------------------------------------------
void CLoginScene::OnLeave()
{
	FKTRACE( L"�뿪 LoginScene" );
	if( m_pUIMgr )
	{
		m_pUIMgr->Hide();
	}
	
	theWorld.Remove( m_pBackgroud );

	// �ر�Ping
	FK2DEngine2::Ping::Release();
}
//------------------------------------------------------------------------
void CLoginScene::SetNextSceneIndex( unsigned int index )
{
	CGameSceneMgr::GetInstance().SetNextSceneIndex( index );
}
//------------------------------------------------------------------------
void CLoginScene::ReceiveMessage(FK2DEngine2::event::CMessage *message)
{
	FKTRACE( L"CLoginScene recv msg : %s", message->GetMessageName().c_str() );
	if ( message->GetMessageName() == L"EnterCharSel" )
	{
		// ����ǵ������ԣ���ôֱ�Ӹ�����ͼ����������
#ifdef SINGLE_TEST
		SetNextSceneIndex( eScene_CharSelectScene );
#else
		g_LoginSvrConnector.m_pClientLink->Connect( "192.168.1.118", 7000 );
#endif
	}
}
//------------------------------------------------------------------------