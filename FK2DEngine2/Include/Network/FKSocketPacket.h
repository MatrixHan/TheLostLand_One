/**
*	created:		2013-5-10   2:10
*	filename: 		FKSocketPacket
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKClientSocket.h"
//------------------------------------------------------------------------
class CClientSocketPacket
{
public:
	enum 
	{
		MaxDataLen = MAX_SOCKET_RAW_DATA_LEN,		// �û����ݵ���󳤶�
		MaxDataHeaderLen = 256,						// ��������װ�Ժ����Է�װͷ���ܵĿ��ܳ���
		MaxBufferLen = (MaxDataLen + MaxDataHeaderLen) 
	};

	CClientSocketPacket()
	{
		m_pClientSocket = NULL;
		m_nBegin = 0;
		m_nEnd = 0;
	}
	void Close()
	{
		m_nBegin = 0;
		m_nEnd = 0;
	}
	BOOL Create(IClientSocket* pClientSocket)
	{
		m_pClientSocket = pClientSocket;
		return TRUE;
	}

	/*
	 ���ܣ���ȡ���������������һ�����ݰ�
	
	 ��ʽ��wDataLen + data[wDataLen]
	
	 ����һ�������ݳ��ȣ�*ppBufBeginΪ��������ʼ��ַ,ָ��m_buf�е�ĳ��λ�� 
		����0��ʾ��δ���룬�ȴ��´�OnDataArrived()�¼�����
		���ظ�����ʾ����ʧ��
	*/
	int OnDataArrived(char** ppBufBegin)
	{
		if(m_pClientSocket == NULL)
			return 0;
		// ��ǰ���������Ƿ������ݣ� CClientLink���Ƿ�����������Ǩ�Ƶ��µĴ���
		if(m_nEnd - m_nBegin > 1)
		{ 
beginrecvdata:
			// ���ʣ��������Ѿ�װ�����壬������δ���룬��Ҫ�������Ƶ����忪ͷ
			int nNextPacketLen = (int) *(WORD*)(m_buf + m_nBegin);
			int nLeft = m_nEnd - m_nBegin;
			int nExpectLen = 2 + nNextPacketLen;		// ���ܳ�
			if(nNextPacketLen <= 0 || nExpectLen > MaxBufferLen)
			{ // �������������������0
#ifdef _DEBUG
				OutputDebugString(L"Error : CClientSocketPacket::OnDataArrived() : nPacketLen error!!\r\n");
				__asm int 3;
#endif
				m_nBegin = m_nEnd = 0;					// ���س�����0
				return -1;
			}
			else if(nExpectLen <= nLeft)
			{ // ������һ����
				*ppBufBegin = m_buf + m_nBegin + 2;
				if(nExpectLen < nLeft)
					m_nBegin += nExpectLen;		// �����������ڻ�����
				else
				{ // ==
					m_nBegin = 0; m_nEnd = 0;			// ��ǰ�պ���һ����
				}
				
				// �������ݰ���
				//.....

				return nNextPacketLen;
			}
			else if(m_nBegin  + nExpectLen > MaxBufferLen)
			{ // �������Ѿ��޷��������һ����
				memmove(m_buf,m_buf + m_nBegin,m_nEnd - m_nBegin);
				m_nEnd = m_nEnd - m_nBegin; m_nBegin = 0;
			}
		}
		else if(m_nEnd - m_nBegin == 1)
		{ // ��˵����:ֻ�յ�����wDataLen��һ�����ֽڣ�����޷������nNextPacketLen
			m_buf[0] = m_buf[m_nBegin];
			m_nBegin = 0; m_nEnd = 1;
		}
		int nLen;
		if(!m_pClientSocket->Recv(MaxBufferLen - m_nEnd,m_buf + m_nEnd,nLen))
			return 0;
		if(nLen == 0)
			return 0;
		m_nEnd += nLen;
		goto beginrecvdata;	// ������ݰ�
	}

	// �����ݰ�����
	int Send(LPSTR buf,int nLen)
	{
		if(m_pClientSocket)
			return FALSE;
		// ���ܣ�
		//.......

		if(nLen > 32)
		{
			m_pClientSocket->Send((char*)&nLen,2);	// ���ͳ���
			return m_pClientSocket->Send(buf,nLen);
		}
		else
		{ // ������������٣���������m_pClientSocket->Send()һ�ε��õĳɱ�
			char data[1024];
			*(WORD*)data = (WORD)nLen;
			memcpy(data + 2,buf,nLen);
			return m_pClientSocket->Send(data,2 + nLen);
		}
	}

	// ��һ��С���������һ���Է��͸�ͨѶ��
	// ��������һ������������������㹻��ʱ���������ͳ�ȥ
	int SendGroupPacket(LPCSTR buf,int nLen)
	{
		return m_pClientSocket->Send(buf,nLen);			// ��������
	}
	// ���һ����ķ���
	// �������ѻ���ʣ��δ���͵����ݷ��ͳ�ȥ
	int SendGroupPacketEnd()								// ����Ǵ�����ͣ��ڴ������������ݣ������һ�����ݰ�
	{
		return TRUE;
	}

protected:
	IClientSocket* m_pClientSocket;
	char m_buf[MaxBufferLen]; 
	int  m_nBegin;		// ��Ч����ʼλ�ã���Ϊ�п��ܵ�һ�����͵ڶ�����һ��������ڶ���������ʼλ�þͲ���0��
	int  m_nEnd;		// ��һ�������ܻ�δ�������
/*
	m_bufʾ��ͼ���£�d��ʾ������
	+-+-+-+-+-+-+-+-+
	|d|d|d|d|d|-+-+-+
	+-+-+-+-+-+-+-+-+
 	 ^		   ^
	m_Begin	  m_nEnd
*/
};
//------------------------------------------------------------------------
