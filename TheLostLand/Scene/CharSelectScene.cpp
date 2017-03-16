/**
*	created:		2013-5-9   14:18
*	filename: 		CharSelectScene
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "CharSelectScene.h"
//------------------------------------------------------------------------
CCharSelectScene::CCharSelectScene()
	: IScene()
{
	m_pUIMgr		= NULL;
	m_nCurSelect	= 0;

	m_pBackgroud = new CFullScreenActor();
	m_pBackgroud->SetSize( GAME_CANVAS_WIDTH, GAME_CANVAS_HEIGHT );
	m_pBackgroud->SetSprite( CHAR_SEL_BK_FILE );
}
//------------------------------------------------------------------------
CCharSelectScene::~CCharSelectScene()
{

}
//------------------------------------------------------------------------
void CCharSelectScene::OnEnter()
{
	FKTRACE( L"���� CCharSelectScene" );

	// UI����
	if( m_pUIMgr == NULL )
	{
		m_pUIMgr = new CCharSelectUI( g_pEngine->getUICanvas(), "CharSelectScene" );
	}
	else
	{
		m_pUIMgr->Show();
	}

	m_pUIMgr->SetOwner( this );

	// �������鴦��
	theWorld.Add( m_pBackgroud, DEFAULT_BACKGROUND_LAYER_Z );

	// ���Ӵ���
	CGameSceneMgr::GetInstance().CreateConnection( eNLS_LoginSvr );
}
//------------------------------------------------------------------------
void CCharSelectScene::OnLeave()
{
	FKTRACE( L"�뿪 CCharSelectScene" );
	if( m_pUIMgr )
	{
		m_pUIMgr->Hide();
	}

	theWorld.Remove( m_pBackgroud );
}
//------------------------------------------------------------------------
void CCharSelectScene::Update( float dt )
{

}
//------------------------------------------------------------------------
void CCharSelectScene::Render()
{

}
//------------------------------------------------------------------------
void CCharSelectScene::SetNextSceneIndex( unsigned int index )
{
	CGameSceneMgr::GetInstance().SetNextSceneIndex( index );
}
//------------------------------------------------------------------------