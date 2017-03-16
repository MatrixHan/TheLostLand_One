/**
*	created:		2013-5-3   17:42
*	filename: 		ServerList
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "MainMacro.h"
//------------------------------------------------------------------------
enum ENUM_ServerState
{
	eServerState_Unknown		= 0,	// δ֪
	eServerState_UnConnected	= 1,	// �޷�����
	eServerState_Normal			= 2,	// ����
	// TODO: ping �£� ������ֱ�Ӽ�����������Ƽ�����
};
//------------------------------------------------------------------------
struct SServerInfo
{
	int					m_nServerIndex;
	std::wstring		m_szServerName;
	ENUM_ServerState	m_nServerState;
	std::string			m_szServerIP;

	SServerInfo() : m_nServerIndex(0) , m_nServerState(eServerState_Unknown)
	{
		m_szServerName = L"δ֪������";
		m_szServerIP = "0.0.0.0";
	}
};
//------------------------------------------------------------------------
class CServerList : public TSingletonClass< CServerList >
{
public:
	typedef std::vector< SServerInfo >					SERVER_INFO_VEC;
	typedef std::map< std::wstring, SERVER_INFO_VEC >	SERVER_INFO_MAP;
public:
	CServerList(){};
	~CServerList();
public:
	SERVER_INFO_MAP					m_ServerInfoMap;
public:
	// ��ȡָ����Ϸ�ķ���������
	int								GetServerNum( std::wstring& szGameName );
	// ��ȡ����Ϸ���͵ĸ���
	int								GetGameNum();
	// ���ط������б�
	bool							LoadFromFile( const char* filename );
private:
	bool							Parser( IResObject *pResObject, int nParentID, std::string szGameName );
};
//------------------------------------------------------------------------