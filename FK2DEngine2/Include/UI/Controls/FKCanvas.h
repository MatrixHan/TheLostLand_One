/**
*	created:		2013-4-11   19:44
*	filename: 		FKCanvas
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <set>
#include "FKBase.h"
#include "../FKInputHandler.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace Controls
	{
		class FK_EXPORT Canvas : public Base
		{
			public:

				typedef Controls::Base BaseClass;

				Canvas( Skin::Base* pSkin );
				virtual ~Canvas();

				// ��ʼ��
				virtual void Initialize() {};

				// ����Canvas
				virtual void RenderCanvas();

				// ÿ֡����һ�Σ��������봦��
				virtual void DoThink();

				// �ж� / ���� һ������Ƿ���Ҫ���»���
				virtual bool NeedsRedraw() { return m_bNeedsRedraw; }
				virtual void Redraw() { m_bNeedsRedraw = true; }

				// �ڲ����������������
				virtual void Render( Skin::Base* pRender );

				// �������Բ�ͣ���� parent->GetCanvas() ֱ����㸸�����
				virtual Controls::Canvas* GetCanvas() { return this; }

				virtual void SetScale( float f );
				virtual float Scale() const { return m_fScale; }

				virtual void OnBoundsChanged( FK2DEngine2::Rect oldBounds );

				// �ͷ�ȫ������塾ע���������������ѵ��á�
				virtual void ReleaseChildren();

				// �ӳ�ɾ��
				virtual void AddDelayedDelete( Controls::Base* pControl );
				virtual void ProcessDelayedDeletes();

				Controls::Base*	FirstTab;
				Controls::Base*	NextTab;

				// ���봦��
				virtual bool InputMouseMoved( int x, int y, int deltaX, int deltaY );
				virtual bool InputMouseButton( int iButton, bool bDown );
				virtual bool InputKey( int iKey, bool bDown );
				virtual bool InputCharacter( FK2DEngine2::UnicodeChar chr );
				virtual bool InputMouseWheel( int val );
				virtual bool InputQuit() { return true; };

				// ��������
				virtual void SetBackgroundColor( const FK2DEngine2::Color & color ) { m_BackgroundColor = color; }
				virtual void SetDrawBackground( bool bShouldDraw ) { m_bDrawBackground = bShouldDraw; }

			protected:

				bool	m_bNeedsRedraw;
				bool	m_bAnyDelete;
				float	m_fScale;

				Controls::Base::List	m_DeleteList;
				std::set< Controls::Base* > m_DeleteSet;
				friend class Controls::Base;
				void PreDeleteCanvas( Controls::Base* );

				bool			m_bDrawBackground;
				FK2DEngine2::Color		m_BackgroundColor;


		};
	}
}
//------------------------------------------------------------------------