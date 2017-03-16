/**
*	created:		2013-4-16   5:50
*	filename: 		BaseMsgDefine
*	author:			FreeKnight
*	Copyright (C): 	 FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// DB��������Ϣ
enum EnumDBSvrMsg
{
	DM_START,

	// add
	DM_CHECKFREE,

	DM_END
};
//------------------------------------------------------------------------
// ���ķ�������Ϣ
enum EnumServerCenterMsg
{
	SCM_MSG_START		= 0,
	//	ע�������
	//	send...
	//	dwFlag = id( server = 0 other id != 0 )
	//	data = REGISTER_SERVER_INFO
	//	recv...
	//	dwFlag = id
	//	w1 = success?
	//	w2 = reason
	//	data = REGISTER_SERVER_RESULT
	SCM_REGISTERSERVER,
	//	ȡ����Ϸ�����������ַ
	//	send...
	//	
	//	mapname/x/y/servername
	//	recv
	//	w1 = success?
	//	w2 = reason
	//	data = ip/port
	SCM_GETGAMESERVERADDR,
	//	���·�������Ϣ
	//	send...
	//	w1 = connections
	//	dwFlag = float loop time
	//	every one second
	SCM_UPDATESERVERINFO,
	//	ȡ�÷�������ַ
	//	send...
	//	w1 = type
	//	data = name
	//	recv
	//	w1 = success?reason
	//	data = SERVERADDR
	SCM_FINDSERVER,
	//	���ͷ���������Ϣ
	//	send...
	//	dwflag = 0
	//	w1 = cmd
	//	w2 = sendtype
	//	w3 = sendparam
	//	data = data
	//	recv...
	//	dwflag = 0
	//	w1 = cmd
	//	w2 = sendservertype
	//	w3 = sendserverindex
	//	data = data
	SCM_MSGACROSSSERVER,
	//	����Կ��ŵ���Ϸ�����������б�
	//	send...
	//	recv
	//	data = gslist
	SCM_MSGGSNAMELIST,
};
//------------------------------------------------------------------------
// ���������Ϣ
enum MSG_ACROSS_SERVER
{
	//	�ߵ�ĳ��
	//	�����ݿ���ı�����óɲ�����
	//	data = account
	MAS_KICKCONNECTION,
	//	�������ӵ�½�������������ѡ�˷�����
	//	send...
	//	data = loginid/account
	//	recv...
	//	data = selcharid
	MAS_ENTERSELCHARSERVER,
	//	������Ϸ���������
	//	send...
	//	data = ENTERGAMESERVER
	//	recv...
	//	data = fail or success
	MAS_ENTERGAMESERVER,
	//	send...
	//	data = LID/ACCOUNT/SID
	//	recv
	//	data = 
	MAS_RESTARTGAME,
};
//------------------------------------------------------------------------
// ������Ϣ
#define	FILLSELF( fill )	memset( this, (BYTE)(fill), sizeof( *this ))
// ��Ϣͷ��С
#define MSGHEADERSIZE	12
//------------------------------------------------------------------------
typedef struct tagMIRMSG
{
	tagMIRMSG()
	{
		FILLSELF(0);
	}
	DWORD		dwFlag;
	WORD		wCmd;
	WORD		wParam[3];
	CHAR		data[4];
}MIRMSG,*PMIRMSG;
//------------------------------------------------------------------------
typedef struct tagMIRMSGHEADER
{
	tagMIRMSGHEADER()
	{
		FILLSELF(0);
	}
	tagMIRMSGHEADER( DWORD _dwFlag, WORD _wCmd, WORD _w1, WORD _w2, WORD _w3 )
	{
		dwFlag = _dwFlag;
		wCmd = _wCmd;
		w1 = _w1;
		w2 = _w2;
		w3 = _w3;
	}
	DWORD	dwFlag;
	WORD	wCmd;
	WORD	w1;
	WORD	w2;
	WORD	w3;
}MIRMSGHEADER;
//------------------------------------------------------------------------
typedef struct tagLMIRMSG
{
	tagLMIRMSG()
	{
		FILLSELF(0);
	}
	BOOL	bUnCodedMsg;
	int		size;
	MIRMSG	msg;
}LMIRMSG,*PLMIRMSG;
//------------------------------------------------------------------------
// ��Ϣ���ӽ���
inline int	_UnGameCode( CHAR * pIn, BYTE * pOut )
{
	int ilen = (int)strlen( pIn );
	int	iptr = 0;
	int optr = 0;
	BYTE	b1,b2,b3,b4;
	int i = 0;
	for( i = 0;i < ilen / 4;i ++ )
	{
		b1 = pIn[iptr++]-0x3b;
		b2 = pIn[iptr++]-0x3b;
		b3 = pIn[iptr++]-0x3b;
		b4 = pIn[iptr++]-0x3b;
		pOut[optr++] = ((b1 & 3) | ((b1 & 0x3c)<<2) | (b4 & 0x0c))^0xeb;
		pOut[optr++] = ((b2 & 3) | ((b2 & 0x3c)<<2) | ((b4 & 0x03)<<2))^0xeb;
		pOut[optr++] = ((b3 & 0x3f) | ((b4 & 0x30)<<2)) ^ 0xeb;

	}
	ilen -= i*4;

	if( ilen == 2 )
	{
		b1 = pIn[iptr++]-0x3b;
		b2 = pIn[iptr++]-0x3b;
		pOut[optr++] = ((b1 & 3) | ((b1 & 0x3c)<<2) | ((b2 & 0x03)<<2) )^0xeb;
	}
	else if( ilen == 3 )
	{
		b1 = pIn[iptr++]-0x3b;
		b2 = pIn[iptr++]-0x3b;
		b3 = pIn[iptr++]-0x3b;
		pOut[optr++] = ((b1 & 3) | ((b1 & 0x3c)<<2) | (b3 & 0x0c))^0xeb;
		pOut[optr++] = ((b2 & 3) | ((b2 & 0x3c)<<2) | ((b3 & 0x03)<<2))^0xeb;
	}

	pOut[optr] = 0;
	return optr;
}
inline int	_CodeGameCode( BYTE * pIn, int size, BYTE * pOut)
{
	BYTE	b1 = 0,bcal = 0;
	BYTE	bflag1 = 0,bflag2 = 0;
	int i = 0;
	int iptr = 0;
	int optr = 0;
	while( iptr < size )
	{
		b1 = pIn[iptr++] ^ 0xeb;//0xeb;
		if( i < 2 )
		{
			bcal = b1;
			bcal>>=2;
			bflag1 =bcal;
			bcal &= 0x3c;
			b1 &=3;
			bcal |= b1;
			bcal += 0x3b;//0x3b;
			pOut[optr++] = (char)bcal;
			bflag2 =  (bflag1 & 3 ) | (bflag2 << 2 );		
		}
		else
		{
			bcal = b1;
			bcal &= 0x3f;
			bcal += 0x3b;//0x3b;
			pOut[optr++] = (char)bcal;
			b1>>= 2;
			b1 &=0x30;
			b1 |= bflag2;
			b1 += 0x3b;//0x3b;
			pOut[optr++] = (char)b1;
			bflag2 = 0;
		}
		i ++;
		i %=3;
	}
	pOut[optr] = 0;
	if( i == 0 )
		return optr;
	pOut[optr++] = bflag2 + 0x3b;//0x3b;
	pOut[optr] = 0;
	//	����??
	return optr;
}
//------------------------------------------------------------------------
inline int DBEncode( LPVOID lpData, int iDataSize, char * pszBuffer )
{
	int length = _CodeGameCode( (BYTE*)lpData, iDataSize, (BYTE*)pszBuffer );
	for( int i = 0;i < length;i ++ )
	{
		if( pszBuffer[i] == '\'' )pszBuffer[i] = '*';
	}
	return length;
}
//------------------------------------------------------------------------
inline int DBDecode( char * pszBuffer, int iCodeSize, LPVOID lpData )
{
	for( int i = 0;i < iCodeSize;i ++ )
	{
		if( pszBuffer[i] == '*' )pszBuffer[i] = '\'';
	}

	return _UnGameCode( (char*)pszBuffer, (BYTE*)lpData );
}
//------------------------------------------------------------------------
// ������Ϣ
// �ȼ�'#'���ټ�Header�����ģ��ټ�DataBuf�����ģ��ټ�'!'������'/0'Ϊβ
inline int encodeMsg( char * buffer, DWORD dwFlag, WORD wCmd, WORD w1, WORD w2, WORD w3, LPVOID lpdata=NULL, int datasize=-1 )
{
	MIRMSGHEADER	header( dwFlag, wCmd, w1, w2, w3 );
	int	codedsize = 1;
	buffer[0] = '#';
	codedsize += _CodeGameCode( (BYTE*)&header, sizeof( header ), (BYTE*)buffer+codedsize );
	if( lpdata != NULL )
	{
		if( datasize < 0 )
			datasize = (int)strlen( (char*)lpdata );
		codedsize += _CodeGameCode( (BYTE*)lpdata, datasize, (BYTE*)buffer+codedsize );
	}
	buffer[codedsize++] = '!';
	buffer[codedsize] = 0;
	return codedsize;
}
//------------------------------------------------------------------------