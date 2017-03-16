/**
*	created:		2013-5-10   8:09
*	filename: 		IDateArchive
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		������󱣴���ȡ����
	˵����			�����ڶ�ȡд�ļ���ִ��Write()��Read�����Ժ󣬻���ָ����Ӧ�����
					ʹ�ã�CArchive��Ҫ�ⲿ�ṩ������������һ���ֲ�buffer��������:
					char buf[1024];
					CDataArchive archive(buf,1024);
					archive.Write("hello",5);
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
enum
{
	sWrite/*����*/ = TRUE,
	sRead/*����*/ = FALSE
};
//------------------------------------------------------------------------
struct IArchive
{
	// ����д�뻺�塣nLen < 4K
	// �����и��ٶ���������밴˳��д����������ݣ���ΪCMapContainer�������ʱ
	virtual BOOL Write(const void* buf,int nLen) = NULL;
	
	// �������ָ������������
	virtual BOOL Read(void* buf,int nLen) = NULL;
	
	// ��ȡ��ǰ����ָ����ָ�����ݵ�ַ���ʺ�ֱ�Ӳ���������
	virtual void* GetBuffer() = NULL;
	
	// �ѵ�ǰָ������ƶ�nOffset�ֽڣ����ص�ǰָ��
	// �������ʧ�ܣ�Seek����-1
	virtual int Seek(int nOffset = 0) = NULL;
	
	// ��ָ���ƶ�ָ��λ��
	virtual int SeekTo(int nPtr = 0) = NULL;
	
	// ��õ�ǰ����ָ��ƫ��
	virtual int GetBufferOffset() = NULL;
	
	// ��ȡ����ʱ����ȡʣ�����ݳ��ȣ�д����ʱ��������Ч����������
	virtual int GetLeftBufferLen() = NULL;
};
//------------------------------------------------------------------------
class CDataArchive : public IArchive  
{
protected:
	CDataArchive(void)
	{
		m_nOffset = 0;
		m_pBindBuffer = NULL;
		m_nMaxSize = 0;
	}

	void Init(void* szBindBuffer, int nMaxSize)
	{
		if (NULL == szBindBuffer || nMaxSize <= 0)
		{
			return;
		}

		m_nOffset = 0;
		m_pBindBuffer = szBindBuffer;
		m_nMaxSize = nMaxSize;
	}
	
public:
	CDataArchive(void* szBindBuffer, int nMaxSize)
	{
		m_nOffset = 0;
		m_pBindBuffer = NULL;
		m_nMaxSize = 0;

		Init(szBindBuffer, nMaxSize);
	}

	virtual ~CDataArchive(void)
	{

	};

	// ����д�뻺�塣nLen < 4K
	virtual BOOL Write(const void* buf, int nLen)
	{
		if(buf == NULL || nLen <= 0 || m_nOffset + nLen > m_nMaxSize)
			return FALSE;

		memcpy((char*)m_pBindBuffer + m_nOffset,buf,nLen);
		m_nOffset += nLen;
		return TRUE;
	}

	// ���������������������
	virtual BOOL Read(void* buf, int nLen)
	{
		if(buf == NULL || nLen <= 0 || m_nOffset + nLen > m_nMaxSize)
			return FALSE;

		memcpy(buf,(char*)m_pBindBuffer + m_nOffset,nLen);
		m_nOffset += nLen;
		return TRUE;
	}

	// ��ȡ��ǰ����ָ����ָ�����ݵ�ַ���ʺ�ֱ�Ӳ���������
	virtual void* GetBuffer()
	{
		return (char*)m_pBindBuffer + m_nOffset;
	}
	
	// �ѵ�ǰָ������ƶ�nOffset�ֽڣ������ƶ�ǰ��ָ��
	// ����nOffSet���ɻ�õ�ǰ����ָ��ƫ��
	// �������ʧ�ܣ�Seek����-1
	virtual int Seek(int nOffset)
	{
		if(m_nOffset + nOffset > m_nMaxSize || m_nOffset + nOffset < 0)
			return -1;
		int nOld = m_nOffset;
		m_nOffset += nOffset;
		return nOld;
	}
	
	// ͬ�ϣ��ѻ���ָ���ƶ���ָ��λ�ã������ƶ�ǰ��ָ��
	virtual int SeekTo(int nPtr = 0)
	{
		if(nPtr > m_nMaxSize || nPtr < 0)
			return -1;
		int nOld = m_nOffset;
		m_nOffset = nPtr;
		return nOld;
	}
	
	// ��õ�ǰ����ָ��ƫ��
	virtual int GetBufferOffset(void)
	{
		return m_nOffset;
	}

	// ��ȡ����ʱ����ȡʣ�����ݳ��ȣ�д����ʱ��������Ч����������
	virtual int GetLeftBufferLen(void)
	{
		return m_nMaxSize - m_nOffset;
	}
	
private:
	int		m_nOffset;
	void* 	m_pBindBuffer;
	int		m_nMaxSize;
};
//------------------------------------------------------------------------