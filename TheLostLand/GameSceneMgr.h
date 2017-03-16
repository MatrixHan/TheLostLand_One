/**
*	created:		2013-5-1   21:42
*	filename: 		GameSceneMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "MainMacro.h"
//------------------------------------------------------------------------
enum EnumSceneType
{
	eScene_LoginScene		= 1,
	eScene_CharSelectScene	= 2,


	eScene_MainScene,

	eScene_Max,
};
//------------------------------------------------------------------------
class CGameSceneMgr : public FK2DEngine2::scene::S2D::FKISceneManager
{
public:
	static CGameSceneMgr& GetInstance();
	static CGameSceneMgr* GetInstancePtr();
protected:
	CGameSceneMgr();
	static CGameSceneMgr *s_GameManager;
public:
	void Render(){}
	void Update(float dt);
	void ReceiveMessage(FK2DEngine2::event::CMessage* message) {}
	bool CreateConnection( EnumNetworkLinkState e );
public:
	void SetNextSceneIndex( unsigned int index );
private:
	int	m_nNextSceneIndex;						// ��һ����������
	IFKAppNetworkLink*	m_pAppNetworkLink;		// ����ͨѶ�ӿ�
	CClientLinkHelper	m_SocketHelper;			// ����Socket��Э���ӿ�
};
//------------------------------------------------------------------------