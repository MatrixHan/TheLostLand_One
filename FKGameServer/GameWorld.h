/**
*	created:		2013-4-19   10:09
*	filename: 		GameWorld
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CGameWorld
{
public:
	CGameWorld(){}
	~CGameWorld(){}
public:
	// ��ʼ��
	bool	Init(){ return true; };
	// ÿ�߼�֡ˢ��
	void	Update(){};
	// ������Ϣ
	void	OnInput( const char* pString ){};
public:
	static CGameWorld * GetInstance()
	{
		if( m_pInstance == NULL )
			m_pInstance = new CGameWorld;
		return m_pInstance;
	}
private:
	static CGameWorld * m_pInstance;
};
//------------------------------------------------------------------------