/**
*	created:		2012-6-16   0:44
*	filename: 		FKTempl
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
/*
 ˵����1����ģ���������vc6.0��afxtempl.h��
       2��afxtempl.h������ܱ�stl���㣻��򵥵��������ջ���Ҳ��ʵ��
	   3��������Ŀ�Ľӿڸ�afxtempl.h��ͬ�����Է������MFC�ͷ�MFC���л�
	   4�������ԭ���ǲ��ı�Դ���룬Դ��������Ҫ�ĺ��Ѿ��������¶���
	      ������в�ͬ�ĵط��� //> ע��
	   5���¼���CCriticalSectionLock��
	   6��������RemoveHeadOnly()����RemoveHead()��֮ͬ�����ڲ�����node
	   7��2008.5.27,����CFIFOCache��ʵ���Ƚ��ȳ��Ļ�����ԣ����ڽϴ����ͬ��С�Ķ���Ļ��壬��IBitmap
	   8��2008.7.6�������������࣬CMultLenArray����ɱ䳤���飬CMultLenArrayCache�������ʹ�õĿɱ䳤�������
	   9��2008.8.7��CStaticStack����̬ջ������һ��FILO����ʵ�֣�����������ʱ���¼���Ķ�������
 */
//------------------------------------------------------------------------
// ����Ѿ�ʹ����MFCģ��⣬��������¸����CList��CArray��ģ�壬���䴲������Ч
#if !defined(__AFXTEMPL_H__)
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
#if !defined(ASSERT)
	#if defined(_DEBUG)
		#define ASSERT(f) {if (!(f))__asm int 3}
	#else
		#define ASSERT(f)	 ((void*)0)
	#endif
#endif
//------------------------------------------------------------------------
#define ASSERT_VALID(x)		((void)0)		//>> ʹ�˺겻������
//------------------------------------------------------------------------
#ifndef __PLACEMENT_NEW_INLINE
	#define __PLACEMENT_NEW_INLINE
	inline void *__cdecl operator new(size_t, void *_P)
			{return (_P); }
	#if _MSC_VER >= 1200
		inline void __cdecl operator delete(void *, void *)
			{return; }
	#endif
#endif
//------------------------------------------------------------------------
template<class TYPE>
inline void __stdcall ConstructElements(TYPE* pElements, int nCount)
{
	ASSERT(nCount == 0 || IsValidAddress(pElements, nCount * sizeof(TYPE)));
	memset((void*)pElements, 0, nCount * sizeof(TYPE));
	// ���ù���
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
BOOL __stdcall CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	return *pElement1 == *pElement2;
}
//------------------------------------------------------------------------
class CDumpContext
{
public:
};
//------------------------------------------------------------------------
class CObject
{
public:
	virtual ~CObject() = 0{};
protected:
	CObject(){}
private:
	CObject(const CObject& objectSrc){}
public:
#if defined(_DEBUG) 
	// Diagnostic Support
	virtual void AssertValid() const{}
	virtual void Dump(CDumpContext& dc) const{}
#endif
};
//------------------------------------------------------------------------
struct CPlex     // warning variable length structure
{
	CPlex* pNext;
	DWORD dwReserved[1];    // align on 8 byte boundary
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CPlex* PASCAL Create(CPlex*& pHead, UINT nMax, UINT cbElement)
			// like 'calloc' but no zero fill
			// may throw memory exceptions
	{
		ASSERT(nMax > 0 && cbElement > 0);
		CPlex* p = (CPlex*) new BYTE[sizeof(CPlex) + nMax * cbElement];
				// may throw exception
		p->pNext = pHead;
		pHead = p;  // change head (adds in reverse order for simplicity)
		return p;
	}

	void FreeDataChain()       // free this one and links
	{
		CPlex* p = this;
		while (p != NULL)
		{
			BYTE* bytes = (BYTE*) p;
			CPlex* pNext = p->pNext;
			delete[] bytes;
			p = pNext;
		}
	}
};

struct __POSITION { };
typedef __POSITION* POSITION;
//------------------------------------------------------------------------
// ���϶������ԭ����ʹ�õ�һЩ����
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
class CList : public CObject
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		TYPE data;
	};
public:
	CList(int nBlockSize = 10);

	int GetCount() const;
	BOOL IsEmpty() const;

	TYPE& GetHead();
	TYPE GetHead() const;
	TYPE& GetTail();
	TYPE GetTail() const;

	TYPE RemoveHead();
	void RemoveHeadOnly();
	TYPE RemoveTail();
	void RemoveAll();

	POSITION AddHead(ARG_TYPE newElement);
	POSITION AddTail(ARG_TYPE newElement);
	void AddHead(CList* pNewList);
	void AddTail(CList* pNewList);

	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	TYPE& GetNext(POSITION& rPosition);			// return *Position++
	TYPE GetNext(POSITION& rPosition) const;	// return *Position++
	TYPE& GetPrev(POSITION& rPosition);			// return *Position--
	TYPE GetPrev(POSITION& rPosition) const;	// return *Position--

	TYPE& GetAt(POSITION position);
	TYPE GetAt(POSITION position) const;
	void SetAt(POSITION pos, ARG_TYPE newElement);
	void RemoveAt(POSITION position);

	POSITION InsertBefore(POSITION position, ARG_TYPE newElement);
	POSITION InsertAfter(POSITION position, ARG_TYPE newElement);

	POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL) const;
	POSITION FindIndex(int nIndex) const;

protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
	CNode* m_pNodeFree;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;
protected:
	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);
public:
	~CList();
#ifdef _DEBUG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};
//------------------------------------------------------------------------
// CList<TYPE, ARG_TYPE> ��������
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
inline int CList<TYPE, ARG_TYPE>::GetCount() const
	{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
inline BOOL CList<TYPE, ARG_TYPE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetHead()
	{ ASSERT(m_pNodeHead != NULL);
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetHead() const
	{ ASSERT(m_pNodeHead != NULL);
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetTail()
	{ ASSERT(m_pNodeTail != NULL);
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetTail() const
	{ ASSERT(m_pNodeTail != NULL);
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline POSITION CList<TYPE, ARG_TYPE>::GetHeadPosition() const
	{ return (POSITION) m_pNodeHead; }
template<class TYPE, class ARG_TYPE>
inline POSITION CList<TYPE, ARG_TYPE>::GetTailPosition() const
	{ return (POSITION) m_pNodeTail; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) const // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetAt(POSITION position)
	{ CNode* pNode = (CNode*) position;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetAt(POSITION position) const
	{ CNode* pNode = (CNode*) position;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline void CList<TYPE, ARG_TYPE>::SetAt(POSITION pos, ARG_TYPE newElement)
	{ CNode* pNode = (CNode*) pos;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		pNode->data = newElement; }
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
CList<TYPE, ARG_TYPE>::CList(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveAll()
{
	ASSERT_VALID(this);

	CNode* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		// �������� <TYPE>(&pNode->data, 1);
		pNode->data.~TYPE();		//> ѧϰvc7.0������

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
CList<TYPE, ARG_TYPE>::~CList()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}
//------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Node helpers
//
// Implementation note: CNode's are stored in CPlex blocks and
//  chained together. Free blocks are maintained in a singly linked list
//  using the 'pNext' member of CNode with 'm_pNodeFree' as the head.
//  Used blocks are maintained in a doubly linked list using both 'pNext'
//  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
//   as the head/tail.
//
// We never free a CPlex block unless the List is destroyed or RemoveAll()
//  is used - so the total number of CPlex blocks may grow large depending
//  on the maximum past size of the list.
//

template<class TYPE, class ARG_TYPE>
typename CList<TYPE, ARG_TYPE>::CNode*
CList<TYPE, ARG_TYPE>::NewNode( CNode* pPrev, CNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		CPlex* pNewBlock = CPlex::Create(m_pBlocks, m_nBlockSize,
				 sizeof(CNode));

		// chain them into free list
		CNode* pNode = (CNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
	ASSERT(m_pNodeFree != NULL);  // we must have something

	CList::CNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow

	ConstructElements<TYPE>(&pNode->data, 1);

	return pNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::FreeNode( CNode* pNode)
{
	// ���� <TYPE>(&pNode->data, 1); //>ѧϰvc7.0
	pNode->data.~TYPE();

	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	CNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	CNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::AddHead(CList* pNewList)
{
	ASSERT_VALID(this);

	ASSERT(pNewList != NULL);
	ASSERT_VALID(pNewList);

	// add a list of same elements to head (maintain order)
	POSITION pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::AddTail(CList* pNewList)
{
	ASSERT_VALID(this);
	ASSERT(pNewList != NULL);
	ASSERT_VALID(pNewList);

	// add a list of same elements
	POSITION pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

template<class TYPE, class ARG_TYPE>
TYPE CList<TYPE, ARG_TYPE>::RemoveHead()
{
	ASSERT_VALID(this);
	ASSERT(m_pNodeHead != NULL);  // don't call on empty list !!!
	ASSERT(IsValidAddress(m_pNodeHead, sizeof(CNode)));

	CNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveHeadOnly()
{
	ASSERT_VALID(this);
	ASSERT(m_pNodeHead != NULL);  // don't call on empty list !!!
	ASSERT(IsValidAddress(m_pNodeHead, sizeof(CNode)));

	CNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
TYPE CList<TYPE, ARG_TYPE>::RemoveTail()
{
	ASSERT_VALID(this);
	ASSERT(m_pNodeTail != NULL);  // don't call on empty list !!!
	ASSERT(IsValidAddress(m_pNodeTail, sizeof(CNode)));

	CNode* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::InsertBefore(POSITION position, ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
		ASSERT(IsValidAddress(pOldNode->pPrev, sizeof(CNode)));
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::InsertAfter(POSITION position, ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	ASSERT(IsValidAddress(pOldNode, sizeof(CNode)));
	CNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
		ASSERT(IsValidAddress(pOldNode->pNext, sizeof(CNode)));
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveAt(POSITION position)
{
	ASSERT_VALID(this);

	CNode* pOldNode = (CNode*) position;
	ASSERT(IsValidAddress(pOldNode, sizeof(CNode)));

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		ASSERT(IsValidAddress(pOldNode->pPrev, sizeof(CNode)));
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		ASSERT(IsValidAddress(pOldNode->pNext, sizeof(CNode)));
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::FindIndex(int nIndex) const
{
	ASSERT_VALID(this);

	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	CNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		pNode = pNode->pNext;
	}
	return (POSITION) pNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, POSITION startAfter) const
{
	ASSERT_VALID(this);

	CNode* pNode = (CNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; pNode != NULL; pNode = pNode->pNext)
		if (CompareElements<TYPE>(&pNode->data, &searchValue))
			return (POSITION)pNode;
	return NULL;
}

inline bool IsValidAddress(const void* p, size_t nBytes,bool bReadWrite = true)
{
	return ((p != NULL) && !IsBadReadPtr(p, nBytes) &&
		(!bReadWrite || !IsBadWritePtr(const_cast<void*>(p), nBytes)));
}

#ifdef _DEBUG
	template<class TYPE, class ARG_TYPE>
	void CList<TYPE, ARG_TYPE>::Dump(CDumpContext& dc) const
	{
		CObject::Dump(dc);
	}

	template<class TYPE, class ARG_TYPE>
	void CList<TYPE, ARG_TYPE>::AssertValid() const
	{
		if (m_nCount == 0)
		{
			// empty list
			ASSERT(m_pNodeHead == NULL);
			ASSERT(m_pNodeTail == NULL);
		}
		else
		{
			// non-empty list
			ASSERT(IsValidAddress(m_pNodeHead, sizeof(CNode)));  //> ԭΪIsValidAddress() 
			ASSERT(IsValidAddress(m_pNodeTail, sizeof(CNode)));
		}
	}
#endif //_DEBUG
#endif // #if !defined(__AFXTEMPL_H__)
//------------------------------------------------------------------------
/*
	���ܣ���̬ջ������һ��FILO����ʵ�֣�����������ʱ���¼���Ķ�������
	˵����CList��Ȼû�г������ƣ�����̬�������ٶ�̫����������е�����Ҳ̫��
		  TYPEһ��Ϊ����ָ��
	��������ͼ�ϻ�����������ɾ��ɾ������ʱ������ָ��ŵ���ջ���Ա��´�����
*/
//------------------------------------------------------------------------
template<class TYPE,int nStackSize = 256>
class CStaticStack
{
public:
	CStaticStack()
	{
		m_nCount = 0;
	}
	~CStaticStack()
	{
		Close();
	};

	BOOL IsEmpty() const{return m_nCount == 0;}
	BOOL IsFull() const{return m_nCount == nStackSize;}

	int  GetCount() const{return m_nCount;}

	// ȡ��һ����㣬Ҫȷ��ջ��Ϊ��
	TYPE Pop()
	{
		m_nCount--;
		return m_stack[m_nCount];
	}

	// ���ջ����������FALSE���˽�㲻�ӵ�ջ�У�Ӧ�ò����Ҫɾ���˶���
	BOOL Push(TYPE node)
	{
		if(m_nCount >= nStackSize)
			return FALSE;
		m_stack[m_nCount++] = node;
		return TRUE;
	}

	// ֱ�ӻ�ȡջ�еĶ���һ�������ɾ���ر�ջʱɾ��ջ�б����Ķ���
	TYPE GetData(int nIndex){return m_stack[nIndex];}

	void Close()
	{
		m_nCount = 0;
	}
private:
	int		m_nCount;
	TYPE	m_stack[nStackSize];
};
//------------------------------------------------------------------------
class CCriticalSectionLock
{
// Constructor
public:
	CCriticalSectionLock()
	{
		InitializeCriticalSection(&m_sect);
	}

// Attributes
public:
	CRITICAL_SECTION m_sect;
// Operations
public:
	inline void Lock()
	{
		EnterCriticalSection(&m_sect);
	}
	inline void Unlock()
	{
		LeaveCriticalSection(&m_sect);
	}
	
// Implementation
public:
	virtual ~CCriticalSectionLock(){}
};
//------------------------------------------------------------------------
/*
	���ܣ�
		���Ƚ��ȳ�����ģ���࣬ʵ�ּ򵥵��Ƚ��ȳ�����
	˵����
		һ��KEY��һ������ͨ��KEY���Ҷ���
		������Ķ��󻺳��������´����õ�ʱ�������ڶ�������ң�����Ҳ����Ŵ�Ӳ���϶�ȡ
		�����С�ڴ���ʱ����ģ����󲻿ɸı�
	ʵ�֣�
		���õ�һ���Ǹռӵ�����δβ�ģ�����ʱ�Ӻ���ǰ����
	������
		TYPE : ����������ͣ���Ҫ֧��TYPE->Release()��������IBitmap*������Ϊָ�룬��Ϊ��Щ����ֵΪNULL
		KEY  : ��ʶ����һ��Ϊ����
		nCacheSize : ������������ر���ͼƬΪ48����ͼƬ
 */
//------------------------------------------------------------------------
template<class TYPE,class KEY,int nCacheSize = 48>
class CFIFOCache
{
public:
	CFIFOCache()
	{
	}
	~CFIFOCache()
	{
		Close();
	}
	// ȡ�û���Ķ��󣬲��ӻ��������ɾ��
	TYPE GetObj(KEY key);

	// ���Ҷ����Ƿ���ڣ�����Ҳ���������NULL
	TYPE Find(KEY key);

	// ����һ�������൱�ڷŵ�����վ��������ֻ���վ���Ѿ�����ͬ�Ķ�����ɾ���ɶ���
	void AddTail(KEY key,TYPE obj);

	// ��Щ�����Ѿ�����key�󶨣�����һ�����õ��ڴ���ѣ���һ���Ѿ���key�޹ص�IBitmap�ڴ�
	// �����ֶ���ŵ����п�ͷ��keyȡһ����Чֵ���ɡ��˺�����ʵ�ָ�GetTail()��ͬ������ɾ���ظ�ID�Ķ���
	// ��Ϊ�ǿն������Ե�������ʱ�����뵽������
	void AddGarbageToHead(KEY key,TYPE obj);

	// ��������ʱ��ȡ��ͷ��㣬�������Ϊ�գ�����NULL
	TYPE PopHead()
	{
		if(m_CacheList.IsEmpty())
			return NULL;
		return m_CacheList.RemoveHead().obj;
	}

	// �����Ƿ�Ϊ��
	BOOL IsEmpty()
	{
		return m_CacheList.IsEmpty();
	}

	BOOL IsFull()
	{
		return m_CacheList.GetCount() >= nCacheSize;
	}

	// ���ղ���ʹ�õ����ж��󣬲�����ɾ��CNode.obj
	void Close();

#ifndef _DEBUG
	private:
#endif

	struct CNode
	{
		KEY			key;
		TYPE		obj;
	};

	CList<CNode,CNode&> m_CacheList;
};
//------------------------------------------------------------------------
template<class TYPE,class KEY,int nCacheSize>
TYPE CFIFOCache<TYPE,KEY,nCacheSize>::Find(KEY key)
{
	POSITION pos = m_CacheList.GetTailPosition();
	while(pos)
	{
		CNode& node = m_CacheList.GetPrev(pos);
		if(node.key == key)
			return node.obj;
	}
	return NULL;
}

template<class TYPE,class KEY,int nCacheSize>
TYPE CFIFOCache<TYPE,KEY,nCacheSize>::GetObj(KEY key)
{
	POSITION pos = m_CacheList.GetTailPosition();
	while(pos)
	{
		POSITION posLast = pos;
		CNode node = m_CacheList.GetPrev(pos);
		if(node.key == key)
		{
			m_CacheList.RemoveAt(posLast);
			return node.obj;
		}
	}
	return NULL;
}

template<class TYPE,class KEY,int nCacheSize>
void CFIFOCache<TYPE,KEY,nCacheSize>::AddTail(KEY key,TYPE obj)
{
	TYPE oldobj = GetObj(key);
	if(oldobj)
		oldobj->Release();

	CNode node;
	if(m_CacheList.GetCount() > nCacheSize)
	{
		node = m_CacheList.RemoveHead();
		node.obj->Release();
	}
	node.key = key;
	node.obj = obj;
	m_CacheList.AddTail(node);
}

template<class TYPE,class KEY,int nCacheSize>
void CFIFOCache<TYPE,KEY,nCacheSize>::AddGarbageToHead(KEY key,TYPE obj)
{
	if(m_CacheList.GetCount() >= nCacheSize)
		return;
	CNode node;
	node.key = key;
	node.obj = obj;
	m_CacheList.AddHead(node);
}

template<class TYPE,class KEY,int nCacheSize>
void CFIFOCache<TYPE,KEY,nCacheSize>::Close()
{
	POSITION pos = m_CacheList.GetHeadPosition();
	while(pos)
	{
		CNode node = m_CacheList.GetNext(pos);
		node.obj->Release();
	}
}
//------------------------------------------------------------------------
/*
	���ܣ��Դ���ʹ�õĿɱ䳤����Ļ�������ͷŶ���ʱ�������̶ֹ���С����ŵ����ԵĶ����У�
		  �´�ʹ��ʱ����������Щ����
		  �磺�����䳤��Ϊ2��3��4������ʱ��ʵ�ʷ���Ķ��ǳ���Ϊ4������
		  CMultLenArray����ɱ䳤���飬CMultLenArrayCache����䳤�������
	��;��
		�磺��ͼ��ÿ����ܰ�����������������������Խ������ɸ����岻ͬ��������Ķ��У�
		��֧��1��4��8��32�ȳ��ȵĿɱ䳤����
		һ��ͨ��CMultLenArray::Create()��̬��������CMultLenArray����ΪCMultLenArray���Ȳ��̶�

	���������£�
		struct CNode
		{
			TYPE pre;		// һЩ�Զ�����Ϣ
			DATA data[];	// �������飬���Ȳ�һ
			// ��������ͬ���ȵ�����
		}
	ʹ�ã�
		  һ���÷�1
			  1��ͨ��CMultLenArrayCache.AddCacheList()������ͬ�������͵�������У���1,4,8,16,32��
			  2��CMultLenArrayCache.AddData()������������е����ݣ�AddData�����Ƿ���Ҫ��һ������������
			  3�����䳤���鲻����Ҫʱ��ͨ��2��CMultLenArrayCache.FreeArray()��������
		  �����÷�2������
				char buf[4096];
				CMultLenArray<DWORD,DWORD> *p = (CMultLenArray<DWORD,DWORD>*)(buf);
				p->Init(100); // 100���鵥Ԫ��������
				p->Add(3);
				p->Add(4);
				int nBufLen = p->GetMemSize();
				......
*/
//------------------------------------------------------------------------
#pragma warning(disable : 4200)
//------------------------------------------------------------------------
template<class TYPE,class DATA>
class CMultLenArray
{
public:
	CMultLenArray(int nArraySize)
	{
		wCount = 0;
		this->wArraySize = nArraySize;
	}
	~CMultLenArray()
	{
		for(int i = 0; i < wArraySize; i++)
			data[i].~DATA();
		wCount = 0;
		wArraySize = 0;
	}
	void Init(int nArraySize = 32)
	{
		wCount = 0;
		this->wArraySize = nArraySize;
	}
	static CMultLenArray<TYPE,DATA>* Create(int nArraySize)
	{
		if(nArraySize < 0)
			return NULL;
		CMultLenArray<TYPE,DATA>* p = (CMultLenArray<TYPE,DATA>*)new char[sizeof(CMultLenArray<TYPE,DATA>) + sizeof(DATA) * nArraySize];
		if(p == NULL)
			return NULL;
		p->wCount = 0;
		p->wArraySize = nArraySize;
		//for(int i = 0; i < nArraySize; i++)
		//	p->data[i].DATA();		// ���������һ�¹��캯��������DATAΪint��ʱ���벻ͨ��
		return p;
	}

	// ��������Ƿ���������������Աʱ��CMultLenArrayCache��ɾ�������飬����һ�������������
	inline BOOL IsFull()const{return wCount >= wArraySize;}
	inline BOOL IsEmpty()const{return wCount == 0;}
	inline BOOL IsValid()const {return wArraySize >= wCount;}

	// ����һ��data��Ԫ
	inline BOOL Add(const DATA& data)
	{
		if(wCount >= wArraySize)
			return FALSE;
		this->data[wCount++] = data;
		return TRUE;
	}

	// ͬAdd()��������Ч�Լ��
	inline void _Add(const DATA& data)
	{
		this->data[wCount++] = data;
	}

	//ɾ����Ч��㣬�������н����ǰ��
	BOOL RemoveAt(int index)
	{
		ASSERT(index < (int)wCount);
		memmove(&data[index],&data[index + 1],(wCount - index - 1) * sizeof(DATA));
		wCount--;
		return TRUE;
	}

	// ɾ��������������ͬ�Ķ���
	BOOL Remove(const DATA& data)
	{
		for(int index = 0; index < (int)wCount; index++)
			if(this->data[index] == data)
			{
				memmove(&this->data[index],&this->data[index + 1],(wCount - index - 1) * sizeof(DATA));
				wCount--;
				return TRUE;
			}
		return FALSE;
	}

	// ��item��0����ʾ�˶����Ѿ���ɾ��
	// ˵����Ϊ���ѭ������ʱ��������Ѿ��������ṩ�˺���
	//       Ҫ��DATA�ṹ�ĵ�һ������Ϊ�����־��һ���ʱDATA�Ľṹ��һ��ָ��
	BOOL ZeroItem(const DATA& data)
	{
		for(int index = 0; index < (int)wCount; index++)
			if(this->data[index] == data)
			{
				// ����һ��DWORD��Ϊ0
				(DWORD&)this->data[index] = 0;
				return TRUE;
			}
		return FALSE;
	}

	// ����Ƿ����������ж����Ѿ���Ч
	BOOL IsFullZero()
	{
		// ����ǲ�������
		for(int index = 0; index < (int)wCount; index++)
		{
			if((DWORD&)this->data[index] != 0)
				return FALSE;
		}
		return index == (int)wCount;
	}

	// ע�⣺�����p�����ָ�룬�����������ʣ�(*p)[0] = 1;
	// �Ҷ�������������ˣ���, p[0] = 1;(X)
	inline DATA& operator[](int i )
	{
		ASSERT(i < (int)wCount);
		return data[i];
	}
	
	// ȡ���û����ݵ�ַ���û����ݶ���ΪPreData + data[]
	inline void* GetBuffer(){return &PreData;}

	// ȡ��data����ĵ�ַ
	inline DATA* GetDataBuffer(){return data;}
	inline TYPE& GetPreData(){return PreData;}

	inline int GetCount()const{return wCount;}
	inline int GetArraySize()const{return wArraySize;}

	// ��ȡ��Ч�����ڴ�ռ���ֽ���������ͨѶʱ��������Ԫ�غ�����
	int GetMemSize()const{return sizeof(CMultLenArray<TYPE,DATA>) + wCount * sizeof(DATA);}

	//friend CMultLenArrayCache<TYPE,DATA>;	// ģ�����޷���������Ԫ?
protected:
	WORD wCount;		// ��ǰ�����
	WORD wArraySize;	// �������
	TYPE PreData;
	DATA data[];		// ��������ͬ���ȵ�����
};
//------------------------------------------------------------------------
template<class TYPE,class DATA>
class CMultLenArrayCache
{
public:
	enum{maxArrayKind = 16};
	typedef CMultLenArray<TYPE,DATA>		ArrayType;
	typedef CStaticStack<ArrayType*,512>	ArrayList;

	CMultLenArrayCache()
	{
		memset(&m_list[0],0,sizeof(m_list));
		memset(&m_SizeList[0],0,sizeof(m_SizeList));
		m_nCount = 0;
	}
	~CMultLenArrayCache(){Close();}
	void Close();
	
	// ����һ��������У���ֱ����֧��1��4��8��32�ȳ��ȶ���Ļ������
	// ע�⣺����С�����˳����룬�Ҳ�д���������
	BOOL AddCacheList(int nArraySize);

	// ����һ���µĲ��������飻����pOldArray�оɵ����ݣ�pOldArray�����յ�����վ
	// ˵�������ȼ��cache�������Ƿ�����ý�㣻�����������Ľ��ʧ�ܣ�pOldArray
	ArrayType* AllocArray(int nItemCount,ArrayType* pOldArray = NULL);

	// ɾ��һ������������
	// ˵����ɾ��ʱ���˽��ŵ���������
	void FreeArray(ArrayType* pArray);

	// ���һ��data�����*ppArrayΪ�գ������һ����С��Array
	// *ppArray�����µ�pArray����Ϊ����������Ѿ�����һ��������������
	// ������ʧ�ܣ�����FALSE�������鲻��
	BOOL AddData( ArrayType** ppArray,DATA& data);

	// ɾ��һ��data������������Ѿ����µ����ݣ����Զ�ɾ�������飬���������뵽��������վ��
	BOOL RemoveData(ArrayType** ppArray,DATA& data);
	
	// �������еĶ�������Ϊ0����ʾ�Ѿ������Ѿ���ɾ��
	BOOL ZeroItem(ArrayType** ppArray,DATA& data);

protected:
	int			m_nCount;
	ArrayList*	m_list[maxArrayKind];
	int			m_SizeList[maxArrayKind];	// m_list����ֱ��Ӧ�ĳ���
};
//------------------------------------------------------------------------
template<class TYPE,class DATA>
void CMultLenArrayCache<TYPE,DATA>::Close()
{
	for(int i = 0; i < m_nCount; i++)
	{
		if(m_list[i])
		{
			int nCount = m_list[i]->GetCount();
			for(int j = 0; j < nCount; j++)
				delete m_list[i]->GetData(j);
			delete m_list[i];
			m_list[i] = NULL;
		}
		m_nCount = 0;
	}
}

template<class TYPE,class DATA>
BOOL CMultLenArrayCache<TYPE,DATA>::AddCacheList(int nSize)
{
	if(m_nCount >= maxArrayKind)
		return FALSE;
	ArrayList* pList = new ArrayList();
	if(pList == NULL)
		return FALSE;
	m_SizeList[m_nCount] = nSize;
	m_list[m_nCount++] = pList;
	return TRUE;	
};

template<class TYPE,class DATA>
BOOL CMultLenArrayCache<TYPE,DATA>::AddData(ArrayType** ppArray,DATA& data)
{
	ASSERT(ppArray != NULL);
	ArrayType* pArr;
	if(*ppArray != NULL)
	{
		if((*ppArray)->Add(data))
			return TRUE;
		if(NULL == (pArr = AllocArray((*ppArray)->GetCount() + 1,*ppArray)))
			return FALSE;
	}
	else
	{
		if(NULL == (pArr = AllocArray(m_SizeList[0])))
			return FALSE;
	}
	pArr->_Add(data);	// �϶��ɹ�
	*ppArray = pArr;
	return TRUE;
}

template<class TYPE,class DATA>
BOOL CMultLenArrayCache<TYPE,DATA>::RemoveData(ArrayType** ppArray,DATA& data)
{
	ASSERT(ppArray != NULL);
	ArrayType* pArr = *ppArray;
	if(pArr == NULL)
		return NULL;
	BOOL retv = pArr->Remove(data);
	if(pArr->IsEmpty())
	{
		FreeArray(pArr);
		*ppArray = NULL;
	}
	return retv;
}

template<class TYPE,class DATA>
BOOL CMultLenArrayCache<TYPE,DATA>::ZeroItem(ArrayType** ppArray,DATA& data)
{
	ASSERT(ppArray != NULL);
	ArrayType* pArr = *ppArray;
	if(pArr == NULL)
		return NULL;
	BOOL retv = pArr->ZeroItem(data);
	if(pArr->IsEmpty())
	{
		FreeArray(pArr);
		*ppArray = NULL;
	}
	return retv;
}


template<class TYPE,class DATA>
CMultLenArray<TYPE,DATA>* CMultLenArrayCache<TYPE,DATA>::AllocArray(int nItemCount,ArrayType* pOldArray)
{
	int i = 0;
	for(; m_SizeList[i] < nItemCount && i < m_nCount;i++);
	if(i >= m_nCount)	// �Ҳ���һ������������ô����������ж�
		return NULL;
	ArrayType* pArray;
	if(!m_list[i]->IsEmpty())
	{
		pArray = m_list[i]->Pop();
		pArray->Init(m_SizeList[i]);
	}
	else if(NULL == (pArray = ArrayType::Create(m_SizeList[i])))
		return NULL;

	// �������ݸ��Ƶ���������
	if(pOldArray)
	{
		int nCount = pOldArray->GetCount();
		for(int j = 0; j < nCount; j++)
			pArray->_Add((*pOldArray)[j]);
		FreeArray(pOldArray);
	}
	return pArray;
}

template<class TYPE,class DATA>
void CMultLenArrayCache<TYPE,DATA>::FreeArray(ArrayType* pArray)
{
	if(pArray == NULL)
		return;
	int nSize = pArray->GetArraySize();
	int i = 0;
	for(; m_SizeList[i] != nSize && i < m_nCount;i++);
	ASSERT(i < m_nCount);

	if(!m_list[i]->Push(pArray))	// ����Ѿ�����ɾ���˱䳤���顣ЩʱpArray�Ѿ���������Ч���ݣ����Կ�ֱ��ɾ��
		delete pArray;
}
//------------------------------------------------------------------------
typedef CMultLenArray<DWORD,void*>		PointerArray;
typedef CMultLenArray<DWORD,DWORD>		DwordArray;
typedef CMultLenArrayCache<DWORD,void*> PointerArrayCache;
typedef CMultLenArrayCache<DWORD,DWORD> DwordArrayCache;
//------------------------------------------------------------------------
#if !defined(__AFXTEMPL_H__)

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMap : public CObject
{
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		KEY key;
		VALUE value;
	};
public:
// Construction
	CMap(int nBlockSize = 10);

// Attributes
	// number of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(ARG_KEY key, VALUE& rValue) const;

// Operations
	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key);

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(ARG_KEY key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(ARG_KEY, UINT&) const;

public:
	~CMap();
#ifdef _DEBUG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};
//------------------------------------------------------------------------
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline int CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
	{ return m_nCount; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
	{ (*this)[key] = newValue; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline POSITION CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline UINT CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
	{ return m_nHashTableSize; }
//------------------------------------------------------------------------
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMap(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
	ASSERT_VALID(this);

	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				//DestructElements<VALUE>(&pAssoc->value, 1);
				//DestructElements<KEY>(&pAssoc->key, 1);
				pNode->value.~TYPE();		//> ѧϰvc7.0������
				pNode->key.~TYPE();			//> ѧϰvc7.0������
			}
		}
	}

	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::~CMap()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CPlex* newBlock = CPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CMap::CAssoc));
		// chain them into free list
		CMap::CAssoc* pAssoc = (CMap::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	CMap::CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
	ConstructElements<KEY>(&pAssoc->key, 1);
	ConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc( CAssoc* pAssoc)
{
	DestructElements<VALUE>(&pAssoc->value, 1);
	DestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey<ARG_KEY>(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (CompareElements(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	CAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey<ARG_KEY>(key) % m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (CompareElements(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(POSITION& rNextPosition,
	KEY& rKey, VALUE& rValue) const
{
	ASSERT_VALID(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}

#ifdef _DEBUG
	template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
	void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Dump(CDumpContext& dc) const
	{
		CObject::Dump(dc);

		dc << "with " << m_nCount << " elements";
		if (dc.GetDepth() > 0)
		{
			// Dump in format "[key] -> value"
			KEY key;
			VALUE val;

			POSITION pos = GetStartPosition();
			while (pos != NULL)
			{
				GetNextAssoc(pos, key, val);
				dc << "\n\t[";
				DumpElements<KEY>(dc, &key, 1);
				dc << "] = ";
				DumpElements<VALUE>(dc, &val, 1);
			}
		}

		dc << "\n";
	}

	template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
	void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::AssertValid() const
	{
		CObject::AssertValid();

		ASSERT(m_nHashTableSize > 0);
		ASSERT(m_nCount == 0 || m_pHashTable != NULL);
			// non-empty map should have hash table
	}
#endif //_DEBUG
#endif // #if !defined(__AFXTEMPL_H__)
//------------------------------------------------------------------------