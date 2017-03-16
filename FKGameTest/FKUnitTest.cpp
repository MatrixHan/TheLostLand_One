/**
*	created:		2013-4-24   7:36
*	filename: 		FKUnitTest
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "FKUnitTestControls.h"
#include "../FK2DEngine2/Include/UI/UnitTest/FKUnitTest.h"
//------------------------------------------------------------------------
using namespace FK2DEngine2;
//------------------------------------------------------------------------
#define ADD_UNIT_TEST( name )\
	GUnit* RegisterUnitTest_##name( FK2DEngine2::Controls::Base* tab );\
{\
	Controls::Button* pButton = cat->Add( #name );\
	pButton->SetName( #name );\
	GUnit* test = RegisterUnitTest_##name( pCenter );\
	test->Hide();\
	test->SetUnitTest( this );\
	pButton->onPress.Add( this, &ThisClass::OnCategorySelect, FK2DEngine2::Event::Packet( test ) );\
}\

//------------------------------------------------------------------------
FK2DEngine2::Controls::TabButton* pButton = NULL;
//------------------------------------------------------------------------
FK_CONTROL_CONSTRUCTOR( UnitTest )
{
	// ��ʼ���ؼ���¼��
	m_pLastControl = NULL;
	Dock( Pos::Fill );
	SetSize( 1024, 768 );

	// �����۵���ؼ�
	Controls::CollapsibleList* pList = new Controls::CollapsibleList( this );
	GetLeft()->GetTabControl()->AddPage( "�۵���", pList );
	GetLeft()->SetWidth( 150 );

	// ������Ϣ�����
	m_TextOutput = new Controls::ListBox( GetBottom() );
	pButton = GetBottom()->GetTabControl()->AddPage( "��Ϣ��", m_TextOutput );
	pButton->Dock( Pos::Center );
	GetBottom()->SetHeight( 200 );

	// �����ײ�״̬��
	m_StatusBar = new Controls::StatusBar( this );
	m_StatusBar->Dock( Pos::Bottom );

	Controls::Layout::Center* pCenter = new Controls::Layout::Center( this );
	pCenter->Dock( Pos::Fill );

	{
		Controls::CollapsibleCategory* cat = pList->Add( "�����ؼ�" );
		ADD_UNIT_TEST( UnitButton );
		ADD_UNIT_TEST( UnitLabel );
		ADD_UNIT_TEST( UnitLabelMultiline );
	}
	{
		Controls::CollapsibleCategory* cat = pList->Add( "�޽����Կؼ�" );
		ADD_UNIT_TEST( UnitProgressBar );
		ADD_UNIT_TEST( UnitGroupBox );
		ADD_UNIT_TEST( UnitImagePanel );
		ADD_UNIT_TEST( UnitStatusBar );
	}
	{
		Controls::CollapsibleCategory* cat = pList->Add( "����ؼ�" );
		ADD_UNIT_TEST( UnitComboBox );
		ADD_UNIT_TEST( UnitTextBox );
		ADD_UNIT_TEST( UnitListBox );
		ADD_UNIT_TEST( UnitCrossSplitter );
		ADD_UNIT_TEST( UnitRadioButton );
		ADD_UNIT_TEST( UnitCheckbox );
		ADD_UNIT_TEST( UnitNumeric );
		ADD_UNIT_TEST( UnitSlider );
		ADD_UNIT_TEST( UnitMenuStrip );
	}
	{
		Controls::CollapsibleCategory* cat = pList->Add( "�����ؼ�" );
		ADD_UNIT_TEST( UnitWindow );
		ADD_UNIT_TEST( UnitTreeControl );
		ADD_UNIT_TEST( UnitProperties );
		ADD_UNIT_TEST( UnitTabControl );
		ADD_UNIT_TEST( UnitScrollControl );
		ADD_UNIT_TEST( UnitPageControl );
	}
	{
		Controls::CollapsibleCategory* cat = pList->Add( "�Ǳ�׼�ؼ�" );
		ADD_UNIT_TEST( UnitCollapsibleList );
		ADD_UNIT_TEST( UnitColorPicker );
	}

	// ��״̬������UI�ײ�
	m_StatusBar->SendToBack();

	// ��Ϣ���������
	PrintText( L"UI��Ԫ���Կ�ʼ.\n" );

	// ˢ�¼���֡��
	m_fLastSecond = FK2DEngine2::Platform::GetTimeInSeconds();
	m_iFrames = 0;

	pList->GetNamedChildren( "MenuStrip" ).DoAction();
}
//------------------------------------------------------------------------
void UnitTest::OnCategorySelect( FK2DEngine2::Event::Info info )
{
	if ( m_pLastControl )
	{
		m_pLastControl->Hide();
	}

	info.Packet->Control->Show();
	m_pLastControl = info.Packet->Control;
}
//------------------------------------------------------------------------
void UnitTest::PrintText( const FK2DEngine2::UnicodeString & str )
{
	m_TextOutput->AddItem( str );
	m_TextOutput->ScrollToBottom();
}
//------------------------------------------------------------------------
void UnitTest::Render( FK2DEngine2::Skin::Base* skin )
{
	m_iFrames++;

	if ( m_fLastSecond < FK2DEngine2::Platform::GetTimeInSeconds() )
	{
		m_StatusBar->SetText( FK2DEngine2::Utility::Format( L"FK2DUIEngine2 ��Ԫ���ԣ���ǰ֡�� - %i fps", m_iFrames * 2 ) );
		m_fLastSecond = FK2DEngine2::Platform::GetTimeInSeconds() + 0.5f;
		m_iFrames = 0;
	}

	BaseClass::Render( skin );
}
//------------------------------------------------------------------------
void GUnit::UnitPrint( FK2DEngine2::UnicodeString str )
{
	m_pUnitTest->PrintText( str );
}
//------------------------------------------------------------------------
void GUnit::UnitPrint( FK2DEngine2::String str )
{
	UnitPrint( Utility::StringToUnicode( str ) );
}
//------------------------------------------------------------------------