/**
*	created:		2013-4-16   5:18
*	filename: 		VirtualServer
*	author:			FreeKnight
*	Copyright (C): 	 FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "inc.h"
//------------------------------------------------------------------------
typedef struct tagSERVERSTATE
{
	tagSERVERSTATE()
	{
		memset( this, 0, sizeof( *this));
	}
	int	numConnection;
	int	numConnected;
	int	numDisconnected;
	DWORD	dwRecvBytes;
	DWORD	dwSendBytes;
	DWORD	dwRunningSeconds;
	DWORD	dwLoopTimes;
	float	fLoopTime;
	BOOL	bUpdated;
	
	char	*	pServerDescript;
	char	*	pServerState;
}SERVERSTATE;
//------------------------------------------------------------------------
#define ERRMSG_NOCONFIGFILE	"�޷���config.ini��"
#define ERRMSG_NOSERVER		"������ָ��û�г�ʼ��!"
//------------------------------------------------------------------------
class CVirtualServer
{
public:
	virtual VOID	GetServerState( SERVERSTATE & state ) = 0;
	virtual BOOL	Start( CSettingFile &) = 0;
	virtual BOOL	Stop() = 0;
};
//------------------------------------------------------------------------