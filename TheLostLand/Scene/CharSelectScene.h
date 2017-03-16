/**
*	created:		2013-5-9   13:56
*	filename: 		CharSelectScene
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../MainMacro.h"
#include "CharSelectUI.h"
//------------------------------------------------------------------------
class CCharSelectScene : public FK2DEngine2::scene::IScene
{
public:
	CCharSelectScene();
	~CCharSelectScene();
public:
	void				OnEnter();
	void				OnLeave();
	void				Update( float dt );
	void				Render();
	void				ReceiveMessage(FK2DEngine2::event::CMessage *message) {}
	void				SetNextSceneIndex( unsigned int index );
private:
	CCharSelectUI*		m_pUIMgr;							// ѡ���ɫUI������
	int					m_nCurSelect;						// ��ǰѡ��Ľ�ɫ���
	stMainRoleLoginInfo m_tagRoleList[MAX_MAIN_ROLE_NUM];	// ��ɫ��Ϣ
	CFullScreenActor*	m_pBackgroud;						// ����ͼƬ
};
//------------------------------------------------------------------------