/**
*	created:		2013-4-11   21:57
*	filename: 		FKDragAndDrop
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../Include/FKUIOutSideHead.h"
//------------------------------------------------------------------------
using namespace FK2DEngine2;
using namespace FK2DEngine2::DragAndDrop;
//------------------------------------------------------------------------
DragAndDrop::Package* DragAndDrop::CurrentPackage = NULL;
FK2DEngine2::Controls::Base* DragAndDrop::HoveredControl = NULL;
FK2DEngine2::Controls::Base* DragAndDrop::SourceControl = NULL;
//------------------------------------------------------------------------
static FK2DEngine2::Controls::Base* LastPressedControl = NULL;
static FK2DEngine2::Controls::Base* NewHoveredControl = NULL;
static FK2DEngine2::Point LastPressedPos;
//------------------------------------------------------------------------
void DragAndDrop::ControlDeleted( FK2DEngine2::Controls::Base* pControl )
{
	if ( SourceControl == pControl )
	{
		SourceControl = NULL;
		CurrentPackage = NULL;
		HoveredControl = NULL;
		LastPressedControl = NULL;
	}

	if ( LastPressedControl == pControl )
	{ LastPressedControl = NULL; }

	if ( HoveredControl == pControl )
	{ HoveredControl = NULL; }

	if ( NewHoveredControl == pControl )
	{ NewHoveredControl = NULL; }
}
//------------------------------------------------------------------------
static int m_iMouseX = 0;
static int m_iMouseY = 0;
//------------------------------------------------------------------------
bool DragAndDrop::Start( FK2DEngine2::Controls::Base* pControl, Package* pPackage )
{
	if ( CurrentPackage )
	{
		return false;
	}

	CurrentPackage = pPackage;
	SourceControl = pControl;
	return true;
}
//------------------------------------------------------------------------
bool OnDrop( int x, int y )
{
	bool bSuccess = false;

	if ( DragAndDrop::HoveredControl )
	{
		DragAndDrop::HoveredControl->DragAndDrop_HoverLeave( DragAndDrop::CurrentPackage );
		bSuccess = DragAndDrop::HoveredControl->DragAndDrop_HandleDrop( DragAndDrop::CurrentPackage, x, y );
	}

	DragAndDrop::SourceControl->DragAndDrop_EndDragging( bSuccess, x, y );
	DragAndDrop::SourceControl->Redraw();
	DragAndDrop::CurrentPackage = NULL;
	DragAndDrop::SourceControl = NULL;
	return true;
}
//------------------------------------------------------------------------
bool DragAndDrop::OnMouseButton( FK2DEngine2::Controls::Base* pHoveredControl, int x, int y, bool bDown )
{
	if ( !bDown )
	{
		LastPressedControl = NULL;

		if ( !CurrentPackage )
		{ return false; }

		OnDrop( x, y );
		return true;
	}

	if ( !pHoveredControl ) { return false; }

	if ( !pHoveredControl->DragAndDrop_Draggable() ) { return false; }

	// �����ϴΰ��µĿؼ����Ա������ק���ʹ���
	LastPressedPos = FK2DEngine2::Point( x, y );
	LastPressedControl = pHoveredControl;
	return false;
}
//------------------------------------------------------------------------
bool ShouldStartDraggingControl( int x, int y )
{
	// ���ؼ�������״̬���������ƶ�
	if ( !LastPressedControl ) { return false; }

	// �ƶ���Χ��С
	int iLength = abs( x - LastPressedPos.x ) + abs( y - LastPressedPos.y );

	if ( iLength < 5 ) { return false; }

	// �����ƶ���
	DragAndDrop::CurrentPackage = LastPressedControl->DragAndDrop_GetPackage( LastPressedPos.x, LastPressedPos.y );

	// ����ʧ��
	if ( !DragAndDrop::CurrentPackage )
	{
		LastPressedControl = NULL;
		DragAndDrop::SourceControl = NULL;
		return false;
	}

	// ��ʼ�ƶ�
	DragAndDrop::SourceControl = LastPressedControl;
	FK2DEngine2::MouseFocus = NULL;
	LastPressedControl = NULL;
	DragAndDrop::CurrentPackage->drawcontrol = NULL;

	// ���ֿؼ��Լ������Ƿ�Ҫ������ק
	if ( !DragAndDrop::SourceControl->DragAndDrop_ShouldStartDrag() )
	{
		DragAndDrop::SourceControl = NULL;
		DragAndDrop::CurrentPackage = NULL;
		return false;
	}

	DragAndDrop::SourceControl->DragAndDrop_StartDragging( DragAndDrop::CurrentPackage, LastPressedPos.x, LastPressedPos.y );
	return true;
}
//------------------------------------------------------------------------
void UpdateHoveredControl( FK2DEngine2::Controls::Base* pCtrl, int x, int y )
{
	NewHoveredControl = pCtrl;

	if ( DragAndDrop::HoveredControl == NewHoveredControl ) { return; }

	if ( DragAndDrop::HoveredControl && DragAndDrop::HoveredControl != NewHoveredControl )
	{ DragAndDrop::HoveredControl->DragAndDrop_HoverLeave( DragAndDrop::CurrentPackage ); }

	if ( NewHoveredControl == DragAndDrop::SourceControl )
	{ NewHoveredControl = NULL; }

	while ( NewHoveredControl && !NewHoveredControl->DragAndDrop_CanAcceptPackage( DragAndDrop::CurrentPackage ) )
	{

		NewHoveredControl = NewHoveredControl->GetParent();

		if ( !NewHoveredControl )
		{
			Platform::SetCursor( CursorType::No );
		}
	}

	DragAndDrop::HoveredControl = NewHoveredControl;

	if ( DragAndDrop::HoveredControl )
	{
		DragAndDrop::HoveredControl->DragAndDrop_HoverEnter( DragAndDrop::CurrentPackage, x, y );
	}

	NewHoveredControl = NULL;
}
//------------------------------------------------------------------------
void DragAndDrop::OnMouseMoved( FK2DEngine2::Controls::Base* pHoveredControl, int x, int y )
{
	// ������һ���ƶ�������
	m_iMouseX = x;
	m_iMouseY = y;

	// ���û��ʲô��Ҫ�ƶ��Ŀؼ����򷵻ء�
	if ( !CurrentPackage && !ShouldStartDraggingControl( x, y ) )
	{ return; }

	// �ƶ����������ػ�
	if ( CurrentPackage && CurrentPackage->drawcontrol )
	{ CurrentPackage->drawcontrol->Redraw(); }

	UpdateHoveredControl( pHoveredControl, x, y );

	if ( !HoveredControl ) { return; }

	HoveredControl->DragAndDrop_Hover( CurrentPackage, x, y );

	Platform::SetCursor( CursorType::Normal );
	pHoveredControl->Redraw();
}
//------------------------------------------------------------------------
void DragAndDrop::RenderOverlay( FK2DEngine2::Controls::Canvas* /*pCanvas*/, Skin::Base* skin )
{
	if ( !CurrentPackage ) { return; }

	if ( !CurrentPackage->drawcontrol ) { return; }

	FK2DEngine2::Point pntOld = skin->GetRender()->GetRenderOffset();
	skin->GetRender()->AddRenderOffset( FK2DEngine2::Rect( m_iMouseX - SourceControl->X() - CurrentPackage->holdoffset.x,
		m_iMouseY - SourceControl->Y() - CurrentPackage->holdoffset.y, 0, 0 ) );
	CurrentPackage->drawcontrol->DoRender( skin );
	skin->GetRender()->SetRenderOffset( pntOld );
}
//------------------------------------------------------------------------