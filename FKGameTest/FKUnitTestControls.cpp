/**
*	created:		2013-4-24   8:23
*	filename: 		FKUnitTestControls
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "FKUnitTestControls.h"
#include "../FK2DEngine2/Include/UI/UnitTest/FKUnitTest.h"
//------------------------------------------------------------------------
class UnitButton : public GUnit
{
public:
	FK_CONTROL_INLINE( UnitButton, GUnit )
	{
		// Normal button
		Controls::Button* pButtonA = new Controls::Button( this );
		pButtonA->SetText( L"Event Tester" );
		pButtonA->onPress.Add( this, &UnitButton::onButtonA );
		{
			Controls::Button* pButtonA = new Controls::Button( this );
			pButtonA->SetBounds( 200, 30, 300, 200 );
			pButtonA->SetText( L"Event Tester" );
			pButtonA->onPress.Add( this, &UnitButton::onButtonA );
		}
		// Unicode test
		Controls::Button* pButtonB = new Controls::Button( this );
		pButtonB->SetText( L"\u0417\u0430\u043C\u0435\u0436\u043D\u0430\u044F \u043C\u043E\u0432\u0430" );
		FK2DEngine2::Align::PlaceBelow( pButtonB, pButtonA, 10 );
		// Image with text
		Controls::Button* pButtonC = new Controls::Button( this );
		pButtonC->SetText( L"Image Button" );
		pButtonC->SetImage( L"test16.png" );
		FK2DEngine2::Align::PlaceBelow( pButtonC, pButtonB, 10 );
		// Just image
		Controls::Button* pButtonD = new Controls::Button( this );
		pButtonD->SetText( L"" );
		pButtonD->SetImage( L"test16.png" );
		pButtonD->SetSize( 20, 20 );
		FK2DEngine2::Align::PlaceBelow( pButtonD, pButtonC, 10 );
		// Toggle button
		Controls::Button* pButtonE = new Controls::Button( this );
		pButtonE->SetText( L"Toggle Me" );
		pButtonE->SetIsToggle( true );
		pButtonE->onToggle.Add( this, &UnitButton::OnToggle );
		pButtonE->onToggleOn.Add( this, &UnitButton::OnToggleOn );
		pButtonE->onToggleOff.Add( this, &UnitButton::OnToggleOff );
		FK2DEngine2::Align::PlaceBelow( pButtonE, pButtonD, 10 );
		// Disabled Button
		Controls::Button* pButtonF = new Controls::Button( this );
		pButtonF->SetText( L"Disabled :D" );
		pButtonF->SetDisabled( true );
		FK2DEngine2::Align::PlaceBelow( pButtonF, pButtonE, 10 );
		// Tooltip Button
		Controls::Button* pButtonG = new Controls::Button( this );
		pButtonG->SetText( L"With Tooltip" );
		pButtonG->SetToolTip( "This is a tooltip!" );
		FK2DEngine2::Align::PlaceBelow( pButtonG, pButtonF, 10 );
	}

	void onButtonA( Controls::Base* pControl )
	{
		UnitPrint( L"Button Pressed (using 'OnPress' event)" );
	}

	void OnToggle( Controls::Base* pControl )
	{
		UnitPrint( L"Button Toggled (using 'OnToggle' event)" );
	}

	void OnToggleOn( Controls::Base* pControl )
	{
		UnitPrint( L"Button Toggled ON (using 'OnToggleOn' event)" );
	}

	void OnToggleOff( Controls::Base* pControl )
	{
		UnitPrint( L"Button Toggled Off (using 'OnToggleOff' event)" );
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitButton, L"Button" );
//------------------------------------------------------------------------
class UnitCheckbox : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitCheckbox, GUnit )
	{
		FK2DEngine2::Controls::CheckBox* check = new FK2DEngine2::Controls::CheckBox( this );
		check->SetPos( 10, 10 );
		check->onChecked.Add( this, &UnitCheckbox::OnChecked );
		check->onUnChecked.Add( this, &UnitCheckbox::OnUnchecked );
		check->onCheckChanged.Add( this, &UnitCheckbox::OnCheckChanged );
		FK2DEngine2::Controls::CheckBoxWithLabel* labeled = new FK2DEngine2::Controls::CheckBoxWithLabel( this );
		labeled->SetPos( 10, 30 );
		labeled->Label()->SetText( "Labeled CheckBox" );
		labeled->Checkbox()->onChecked.Add( this, &UnitCheckbox::OnChecked );
		labeled->Checkbox()->onUnChecked.Add( this, &UnitCheckbox::OnUnchecked );
		labeled->Checkbox()->onCheckChanged.Add( this, &UnitCheckbox::OnCheckChanged );
		{
			FK2DEngine2::Controls::CheckBox* check = new FK2DEngine2::Controls::CheckBox( this );
			check->SetPos( 10, 54 );
			check->SetDisabled( true );
		}
	}

	void OnChecked( Controls::Base* pControl )
	{
		UnitPrint( L"Checkbox Checked (using 'OnChecked' event)" );
	}

	void OnUnchecked( Controls::Base* pControl )
	{
		UnitPrint( L"Checkbox Unchecked (using 'OnUnchecked' event)" );
	}

	void OnCheckChanged( Controls::Base* pControl )
	{
		UnitPrint( L"Checkbox CheckChanged (using 'OnCheckChanged' event)" );
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitCheckbox, L"Checkbox" );
//------------------------------------------------------------------------
class UnitColorPicker : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitColorPicker, GUnit )
	{
		Controls::ColorPicker* pCtrl = new Controls::ColorPicker( this );
		pCtrl->SetBounds( 0, 10, 250, 250 );
		Controls::HSVColorPicker* pHSVCtrl = new Controls::HSVColorPicker( this );
		pHSVCtrl->SetBounds( 260, 10, 250, 150 );
	}
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitColorPicker, L"ColorPicker" );
//------------------------------------------------------------------------
class UnitComboBox : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitComboBox, GUnit )
	{
		{
			FK2DEngine2::Controls::ComboBox* combo = new FK2DEngine2::Controls::ComboBox( this );
			combo->SetPos( 50, 50 );
			combo->SetWidth( 200 );
			combo->AddItem( L"Option One", "one" );
			combo->AddItem( L"Number Two", "two" );
			combo->AddItem( L"Door Three", "three" );
			combo->AddItem( L"Four Legs", "four" );
			combo->AddItem( L"Five Birds", "five" );
			combo->onSelection.Add( this, &UnitComboBox::OnComboSelect );
		}
		{
			// Empty..
			FK2DEngine2::Controls::ComboBox* combo = new FK2DEngine2::Controls::ComboBox( this );
			combo->SetPos( 50, 80 );
			combo->SetWidth( 200 );
		}
		{
			// Empty..
			FK2DEngine2::Controls::ComboBox* combo = new FK2DEngine2::Controls::ComboBox( this );
			combo->SetPos( 50, 110 );
			combo->SetWidth( 200 );

			for ( int i = 0; i < 500; i++ )
			{ combo->AddItem( L"Lots Of Options" ); }
		}
	}

	void OnComboSelect( FK2DEngine2::Controls::Base* pControl )
	{
		FK2DEngine2::Controls::ComboBox* combo = ( FK2DEngine2::Controls::ComboBox* ) pControl;
		UnitPrint( Utility::Format( L"Combo Changed: %ls", combo->GetSelectedItem()->GetText().GetUnicode().c_str() ) );
	}
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitComboBox, L"ComboBox" );
//------------------------------------------------------------------------
class UnitCollapsibleList : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitCollapsibleList, GUnit )
	{
		FK2DEngine2::Controls::CollapsibleList* pControl = new FK2DEngine2::Controls::CollapsibleList( this );
		pControl->SetSize( 100, 200 );
		pControl->SetPos( 10, 10 );
		{
			FK2DEngine2::Controls::CollapsibleCategory* cat = pControl->Add( "Category One" );
			cat->Add( "Hello" );
			cat->Add( "Two" );
			cat->Add( "Three" );
			cat->Add( "Four" );
		}
		{
			FK2DEngine2::Controls::CollapsibleCategory* cat = pControl->Add( "Shopping" );
			cat->Add( "Special" );
			cat->Add( "Two Noses" );
			cat->Add( "Orange ears" );
			cat->Add( "Beer" );
			cat->Add( "Three Eyes" );
			cat->Add( "Special" );
			cat->Add( "Two Noses" );
			cat->Add( "Orange ears" );
			cat->Add( "Beer" );
			cat->Add( "Three Eyes" );
			cat->Add( "Special" );
			cat->Add( "Two Noses" );
			cat->Add( "Orange ears" );
			cat->Add( "Beer" );
			cat->Add( "Three Eyes" );
		}
		{
			FK2DEngine2::Controls::CollapsibleCategory* cat = pControl->Add( "Category One" );
			cat->Add( "Hello" );
			cat->Add( "Two" );
			cat->Add( "Three" );
			cat->Add( "Four" );
		}
	}
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitCollapsibleList, L"CollapsibleList" );
//------------------------------------------------------------------------
class UnitCrossSplitter : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitCrossSplitter, GUnit )
	{
		Dock( Pos::Fill );
		m_bSplittersVisible = false;
		m_iCurZoom = 0;
		m_Splitter = new FK2DEngine2::Controls::CrossSplitter( this );
		m_Splitter->SetPos( 0, 0 );
		m_Splitter->Dock( Pos::Fill );
		{
			FK2DEngine2::Controls::Button* testButton =  new FK2DEngine2::Controls::Button( m_Splitter );
			testButton->SetText( "TOPLEFT" );
			m_Splitter->SetPanel( 0, testButton );
		}
		{
			FK2DEngine2::Controls::Button* testButton =  new FK2DEngine2::Controls::Button( m_Splitter );
			testButton->SetText( "TOPRIGHT" );
			m_Splitter->SetPanel( 1, testButton );
		}
		{
			FK2DEngine2::Controls::Button* testButton =  new FK2DEngine2::Controls::Button( m_Splitter );
			testButton->SetText( "BOTTOMRIGHT" );
			m_Splitter->SetPanel( 2, testButton );
		}
		{
			FK2DEngine2::Controls::Button* testButton =  new FK2DEngine2::Controls::Button( m_Splitter );
			testButton->SetText( "BOTTOMLEFT" );
			m_Splitter->SetPanel( 3, testButton );
		}
		//Status bar to hold unit testing buttons
		FK2DEngine2::Controls::StatusBar* pStatus = new FK2DEngine2::Controls::StatusBar( this );
		pStatus->Dock( Pos::Bottom );
		{
			FK2DEngine2::Controls::Button* pButton = new FK2DEngine2::Controls::Button( pStatus );
			pButton->SetText( "Zoom" );
			pButton->onPress.Add( this, &UnitCrossSplitter::ZoomTest );
			pStatus->AddControl( pButton, false );
		}
		{
			FK2DEngine2::Controls::Button* pButton = new FK2DEngine2::Controls::Button( pStatus );
			pButton->SetText( "UnZoom" );
			pButton->onPress.Add( this, &UnitCrossSplitter::UnZoomTest );
			pStatus->AddControl( pButton, false );
		}
		{
			FK2DEngine2::Controls::Button* pButton = new FK2DEngine2::Controls::Button( pStatus );
			pButton->SetText( "CenterPanels" );
			pButton->onPress.Add( this, &UnitCrossSplitter::CenterPanels );
			pStatus->AddControl( pButton, true );
		}
	}

	void ZoomTest( FK2DEngine2::Controls::Base* pFromPanel )
	{
		m_Splitter->Zoom( m_iCurZoom );
		m_iCurZoom++;

		if ( m_iCurZoom == 4 )
		{ m_iCurZoom = 0; }
	}

	void UnZoomTest( FK2DEngine2::Controls::Base* pFromPanel )
	{
		m_Splitter->UnZoom();
	}

	void CenterPanels( FK2DEngine2::Controls::Base* pFromPanel )
	{
		m_Splitter->CenterPanels();
		m_Splitter->UnZoom();
	}

	void Layout( FK2DEngine2::Skin::Base* skin ) {}

	bool m_bSplittersVisible;
	int	m_iCurZoom;
	Controls::CrossSplitter* m_Splitter;

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitCrossSplitter, L"CrossSplitter" );
//------------------------------------------------------------------------
class UnitGroupBox : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitGroupBox, GUnit )
	{
		FK2DEngine2::Controls::GroupBox* pGroup = new FK2DEngine2::Controls::GroupBox( this );
		pGroup->SetText( "Group Box" );
		pGroup->SetSize( 300, 200 );
	}
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitGroupBox, L"GroupBox" );
//------------------------------------------------------------------------
class UnitImagePanel : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitImagePanel, GUnit )
	{
		// Normal
		{
			Controls::ImagePanel* img = new Controls::ImagePanel( this );
			img->SetImage( L"FK.png" );
			img->SetBounds( 10, 10, 100, 100 );
		}
		// Missing
		{
			Controls::ImagePanel* img = new Controls::ImagePanel( this );
			img->SetImage( L"missingimage.png" );
			img->SetBounds( 120, 10, 100, 100 );
		}
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitImagePanel, L"ImagePanel" );
//------------------------------------------------------------------------
class UnitLabel : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitLabel, GUnit )
	{
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( "Garry's Normal Label" );
			label->SizeToContents();
			label->SetPos( 10, 10 );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( L"Chinese: \u4E45\u6709\u5F52\u5929\u613F \u7EC8\u8FC7\u9B3C\u95E8\u5173" );
			label->SizeToContents();
			label->SetPos( 10, 30 );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( L"Japanese: \u751F\u3080\u304E\u3000\u751F\u3054\u3081\u3000\u751F\u305F\u307E\u3054" );
			label->SizeToContents();
			label->SetPos( 10, 50 );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( L"Korean: \uADF9\uC9C0\uD0D0\uD5D8\u3000\uD611\uD68C\uACB0\uC131\u3000\uCCB4\uACC4\uC801\u3000\uC5F0\uAD6C" );
			label->SizeToContents();
			label->SetPos( 10, 70 );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( L"Hindi: \u092F\u0947 \u0905\u0928\u0941\u091A\u094D\u091B\u0947\u0926 \u0939\u093F\u0928\u094D\u0926\u0940 \u092E\u0947\u0902 \u0939\u0948\u0964" );
			label->SizeToContents();
			label->SetPos( 10, 90 );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( L"Arabic: \u0627\u0644\u0622\u0646 \u0644\u062D\u0636\u0648\u0631 \u0627\u0644\u0645\u0624\u062A\u0645\u0631 \u0627\u0644\u062F\u0648\u0644\u064A" );
			label->SizeToContents();
			label->SetPos( 10, 110 );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( L"Wow, Coloured Text" );
			label->SetTextColor( FK2DEngine2::Color( 0, 0, 255, 255 ) );
			label->SizeToContents();
			label->SetPos( 10, 130 );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( L"Coloured Text With Alpha" );
			label->SetTextColor( FK2DEngine2::Color( 0, 0, 255, 100 ) );
			label->SizeToContents();
			label->SetPos( 10, 150 );
		}
		{
			m_Font.facename = L"Comic Sans MS";
			m_Font.size = 25;
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( this );
			label->SetText( L"Custom Font (Comic Sans 25)" );
			label->SetFont( &m_Font );
			label->SizeToContents();
			label->SetPos( 10, 170 );
		}
	}

	FK2DEngine2::Font	m_Font;
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitLabel, L"Label" );
//------------------------------------------------------------------------
class UnitLabelMultiline : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitLabelMultiline, GUnit )
	{
		Dock( Pos::Fill );
		FK2DEngine2::Controls::CrossSplitter* pSplitter = new FK2DEngine2::Controls::CrossSplitter( this );
		pSplitter->Dock( Pos::Fill );
		pSplitter->SetMargin( Margin( 10, 10, 10, 10 ) );
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( pSplitter );
			label->SetText( "I think we're losing sight of the real issue here, which is: what are we gonna call ourselves?\n\nErm, and I think it comes down to a choice between \"The League Against Salivating Monsters\" or my own personal preference, which is \"The Committee for the Liberation and Integration of Terrifying Organisms and their Rehabilitation Into Society.\" Erm, one drawback with that: the abbreviation is \"CLITORIS.\"" );
			label->SetWrap( true );
			pSplitter->SetPanel( 0, label );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( pSplitter );
			label->SetText( "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ultrices pharetra scelerisque. Ut posuere velit a ligula suscipit ut lobortis ligula congue. Aliquam lacinia cursus est, quis aliquam nisl scelerisque vitae. Nunc porta eros sem, nec rhoncus eros. Integer elementum, quam vitae egestas dictum, mi quam gravida augue, non fringilla lacus nisi sit amet nunc. Maecenas dolor tellus, consequat sed sodales ut, aliquam ac enim. Nulla facilisi. Maecenas eleifend, velit a lobortis vehicula, nunc lacus egestas leo, volutpat egestas augue nulla nec turpis. Aenean convallis diam magna. Duis ac lacinia massa. In id dui vel dui laoreet congue. Aliquam suscipit quam et augue sagittis egestas. Integer posuere arcu sit amet lacus porttitor et malesuada enim mollis. Duis luctus est in purus consectetur sit amet volutpat tortor euismod. Nulla facilisi." );
			label->SetWrap( true );
			pSplitter->SetPanel( 1, label );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( pSplitter );
			label->SetText( "Integer eget rutrum nisi. Ut eget dui et turpis volutpat pulvinar non sed lacus. Proin vestibulum felis nec elit tristique non imperdiet eros pretium. Nullam pulvinar sem eleifend turpis eleifend vel dapibus dui sodales. Curabitur euismod hendrerit felis nec vestibulum. Suspendisse tempus gravida ligula a vestibulum. Phasellus et eros at justo hendrerit cursus. Phasellus rutrum porta diam, in mollis ante aliquam at. Vestibulum interdum ligula at massa auctor scelerisque. Sed at tincidunt risus. Donec ut est dui. Vestibulum blandit urna eu metus malesuada blandit" );
			label->SetWrap( true );
			pSplitter->SetPanel( 2, label );
		}
		{
			FK2DEngine2::Controls::Label* label = new FK2DEngine2::Controls::Label( pSplitter );
			label->SetText( "Nullam vel risus eget lacus consectetur rutrum. Curabitur eros libero, porta sed commodo vel, euismod non quam. Fusce bibendum posuere metus, nec mollis odio rutrum ac. Cras nec sapien et mauris dapibus pretium id quis dolor. Sed a velit vel tellus viverra sodales. Praesent tempor purus et elit ultrices tristique. Sed in enim nec elit molestie fermentum et quis enim. Nullam varius placerat lacus nec ultrices. Aliquam erat volutpat. Suspendisse potenti. Nullam euismod pulvinar luctus. Vestibulum ut dui nisi, eget tempus est. Vivamus molestie arcu non enim pulvinar sollicitudin. Pellentesque dapibus risus sit amet diam tempor faucibus accumsan ante porta. Phasellus quis facilisis quam. Fusce eget adipiscing magna." );
			label->SetWrap( true );
			pSplitter->SetPanel( 3, label );
		}
	}

	FK2DEngine2::Font	m_Font;
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitLabelMultiline, L"LabelMultiline" );
//------------------------------------------------------------------------
class UnitListBox : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitListBox, GUnit )
	{
		{
			FK2DEngine2::Controls::ListBox* ctrl = new FK2DEngine2::Controls::ListBox( this );
			ctrl->SetBounds( 10, 10, 100, 200 );
			ctrl->AddItem( L"First" );
			ctrl->AddItem( L"Blue" );
			ctrl->AddItem( L"Yellow" );
			ctrl->AddItem( L"Orange" );
			ctrl->AddItem( L"Brown" );
			ctrl->AddItem( L"Black" );
			ctrl->AddItem( L"Green" );
			ctrl->AddItem( L"Dog" );
			ctrl->AddItem( L"Cat Blue" );
			ctrl->AddItem( L"Shoes" );
			ctrl->AddItem( L"Shirts" );
			ctrl->AddItem( L"Chair" );
			ctrl->AddItem( L"Last" );
			ctrl->SelectByString( "Bl*", true );
			ctrl->SetAllowMultiSelect( true );
			ctrl->SetKeyboardInputEnabled( true );
			ctrl->onRowSelected.Add( this, &ThisClass::RowSelected );
		}
		{
			FK2DEngine2::Controls::ListBox* ctrl = new FK2DEngine2::Controls::ListBox( this );
			ctrl->SetBounds( 120, 10, 200, 200 );
			ctrl->SetColumnCount( 3 );
			ctrl->SetAllowMultiSelect( true );
			ctrl->onRowSelected.Add( this, &ThisClass::RowSelected );
			{
				FK2DEngine2::Controls::Layout::TableRow* pRow = ctrl->AddItem( L"Baked Beans" );
				pRow->SetCellText( 1, L"Heinz" );
				pRow->SetCellText( 2, "£3.50" );
			}
			{
				FK2DEngine2::Controls::Layout::TableRow* pRow = ctrl->AddItem( L"Bananas" );
				pRow->SetCellText( 1, L"Trees" );
				pRow->SetCellText( 2, L"$1.27" );
			}
			{
				FK2DEngine2::Controls::Layout::TableRow* pRow = ctrl->AddItem( L"Chicken" );
				pRow->SetCellText( 1, L"\u5355\u5143\u6D4B\u8BD5" );
				pRow->SetCellText( 2, L"\u20AC8.95" );
			}
		}
	}


	void RowSelected( FK2DEngine2::Controls::Base* pControl )
	{
		FK2DEngine2::Controls::ListBox* ctrl = ( FK2DEngine2::Controls::ListBox* ) pControl;
		UnitPrint( Utility::Format( L"Listbox Item Selected: %ls", ctrl->GetSelectedRow()->GetText( 0 ).GetUnicode().c_str() ) );
	}

	FK2DEngine2::Font	m_Font;
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitListBox, L"ListBox" );
//------------------------------------------------------------------------
class UnitMenuStrip : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitMenuStrip, GUnit )
	{
		Dock( Pos::Fill );
		FK2DEngine2::Controls::MenuStrip* menu = new FK2DEngine2::Controls::MenuStrip( this );
		{
			FK2DEngine2::Controls::MenuItem* pRoot = menu->AddItem( L"File" );
			pRoot->GetMenu()->AddItem( L"New", L"test16.png", "Ctrl + N" )->SetAction( this, &ThisClass::MenuItemSelect );
			pRoot->GetMenu()->AddItem( L"Load", L"test16.png", "Ctrl+L" )->SetAction( this, &ThisClass::MenuItemSelect );
			pRoot->GetMenu()->AddItem( L"Save", "", "CTRL+S" )->SetAction( this, &ThisClass::MenuItemSelect );
			pRoot->GetMenu()->AddItem( L"Save As..", "", "Ctrl+Shift+S" )->SetAction( this, &ThisClass::MenuItemSelect );
			pRoot->GetMenu()->AddItem( L"Quit", "", "Ctrl+Q" )->SetAction( this, &ThisClass::MenuItemSelect );
		}
		{
			FK2DEngine2::Controls::MenuItem* pRoot = menu->AddItem( L"\u043F\u0438\u0440\u0430\u0442\u0441\u0442\u0432\u043E" );
			pRoot->GetMenu()->AddItem( L"\u5355\u5143\u6D4B\u8BD5" )->SetAction( this, &ThisClass::MenuItemSelect );
			pRoot->GetMenu()->AddItem( L"\u0111\u01A1n v\u1ECB th\u1EED nghi\u1EC7m" )->SetAction( this, &ThisClass::MenuItemSelect );
		}
		{
			FK2DEngine2::Controls::MenuItem* pRoot = menu->AddItem( L"Submenu" );
			FK2DEngine2::Controls::MenuItem* pCheckable = pRoot->GetMenu()->AddItem( "Checkable" );
			pCheckable->SetCheckable( true );
			pCheckable->SetChecked( true );
			{
				FK2DEngine2::Controls::MenuItem* pRootB = pRoot->GetMenu()->AddItem( "Two" );
				pRootB->GetMenu()->AddItem( "Two.One" );
				pRootB->GetMenu()->AddItem( "Two.Two" );
				pRootB->GetMenu()->AddItem( "Two.Three" );
				pRootB->GetMenu()->AddItem( "Two.Four" );
				pRootB->GetMenu()->AddItem( "Two.Five" );
				pRootB->GetMenu()->AddItem( "Two.Six" );
				pRootB->GetMenu()->AddItem( "Two.Seven" );
				pRootB->GetMenu()->AddItem( "Two.Eight" );
				pRootB->GetMenu()->AddItem( "Two.Nine", "test16.png" );
			}
			pRoot->GetMenu()->AddItem( "Three" );
			pRoot->GetMenu()->AddItem( "Four" );
			pRoot->GetMenu()->AddItem( "Five" );
			{
				FK2DEngine2::Controls::MenuItem* pRootB = pRoot->GetMenu()->AddItem( "Six" );
				pRootB->GetMenu()->AddItem( "Six.One" );
				pRootB->GetMenu()->AddItem( "Six.Two" );
				pRootB->GetMenu()->AddItem( "Six.Three" );
				pRootB->GetMenu()->AddItem( "Six.Four" );
				pRootB->GetMenu()->AddItem( "Six.Five", "test16.png" );
				{
					FK2DEngine2::Controls::MenuItem* pRootC = pRootB->GetMenu()->AddItem( "Six.Six" );
					pRootC->GetMenu()->AddItem( "Sheep" );
					pRootC->GetMenu()->AddItem( "Goose" );
					{
						FK2DEngine2::Controls::MenuItem* pRootD = pRootC->GetMenu()->AddItem( "Camel" );
						pRootD->GetMenu()->AddItem( "Eyes" );
						pRootD->GetMenu()->AddItem( "Nose" );
						{
							FK2DEngine2::Controls::MenuItem* pRootE = pRootD->GetMenu()->AddItem( "Hair" );
							pRootE->GetMenu()->AddItem( "Blonde" );
							pRootE->GetMenu()->AddItem( "Black" );
							{
								FK2DEngine2::Controls::MenuItem* pRootF = pRootE->GetMenu()->AddItem( "Red" );
								pRootF->GetMenu()->AddItem( "Light" );
								pRootF->GetMenu()->AddItem( "Medium" );
								pRootF->GetMenu()->AddItem( "Dark" );
							}
							pRootE->GetMenu()->AddItem( "Brown" );
						}
						pRootD->GetMenu()->AddItem( "Ears" );
					}
					pRootC->GetMenu()->AddItem( "Duck" );
				}
				pRootB->GetMenu()->AddItem( "Six.Seven" );
				pRootB->GetMenu()->AddItem( "Six.Eight" );
				pRootB->GetMenu()->AddItem( "Six.Nine" );
			}
			pRoot->GetMenu()->AddItem( "Seven" );
		}
	}


	void MenuItemSelect( Base* pControl )
	{
		FK2DEngine2::Controls::MenuItem* pMenuItem = ( FK2DEngine2::Controls::MenuItem* ) pControl;
		UnitPrint( Utility::Format( L"Menu Selected: %ls", pMenuItem->GetText().GetUnicode().c_str() ) );
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitMenuStrip, L"MenuStrip" );
//------------------------------------------------------------------------
class UnitNumeric : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitNumeric, GUnit )
	{
		Controls::NumericUpDown* pCtrl = new Controls::NumericUpDown( this );
		pCtrl->SetBounds( 10, 10, 50, 20 );
		pCtrl->SetValue( 50 );
		pCtrl->SetMax( 1000 );
		pCtrl->SetMin( -1000 );
		//	pCtrl->onPress.Add( this, &ThisClass::onButtonA );
	}

	void onButtonA( Controls::Base* pControl )
	{
		//	UnitPrint( L"Button Pressed (using 'OnPress' event)" );
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitNumeric, L"Numeric" );
//------------------------------------------------------------------------
class UnitPageControl : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitPageControl, GUnit )
	{
		FK2DEngine2::Controls::PageControl* control = new FK2DEngine2::Controls::PageControl( this );
		control->SetSize( 500, 300 );
		control->SetPageCount( 5 );
		control->onPageChanged.Add( this, &ThisClass::OnPageChanged );
		control->onFinish.Add( this, &ThisClass::OnFinish );
		// Page 0
		{
			FK2DEngine2::Controls::Button* pButton = new FK2DEngine2::Controls::Button( control->GetPage( 0 ) );
			pButton->Dock( Pos::Fill );
			pButton->SetText( "This button is fill docked on page 0" );
		}
		// Page 1
		{
			FK2DEngine2::Controls::Button* pButton = new FK2DEngine2::Controls::Button( control->GetPage( 1 ) );
			pButton->Dock( Pos::Top );
			pButton->SetText( "This button is top docked on page 1" );
		}
		// Page 2
		{
			FK2DEngine2::Controls::Button* pButton = new FK2DEngine2::Controls::Button( control->GetPage( 2 ) );
			pButton->SetSize( 400, 1000 );
			pButton->SetPos( 50, 50 );
			pButton->SetText( "This button is long to test scrolling (page 2)" );
		}
	}

	void OnPageChanged( Event::Info info )
	{
		UnitPrint( Utility::Format( L"PageControl Changed Page: %i", info.Integer ) );
	}

	void OnFinish( Event::Info info )
	{
		UnitPrint( L"Finish Pressed" );
	}
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitPageControl, L"PageControl" );
//------------------------------------------------------------------------
class UnitProgressBar : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitProgressBar, GUnit )
	{
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 110, 20, 200, 20 ) );
			pb->SetValue( 0.27f );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 110, 50, 200, 20 ) );
			pb->SetValue( 0.66f );
			pb->SetAlignment( Pos::Right | Pos::CenterV );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 110, 80, 200, 20 ) );
			pb->SetValue( 0.88f );
			pb->SetAlignment( Pos::Left | Pos::CenterV );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 110, 110, 200, 20 ) );
			pb->SetAutoLabel( false );
			pb->SetValue( 0.20f );
			pb->SetAlignment( Pos::Right | Pos::CenterV );
			pb->SetText( L"40,245 MB" );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 110, 140, 200, 20 ) );
			pb->SetAutoLabel( false );
			pb->SetValue( 1.00f );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 110, 170, 200, 20 ) );
			pb->SetAutoLabel( false );
			pb->SetValue( 0.00f );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 110, 200, 200, 20 ) );
			pb->SetAutoLabel( false );
			pb->SetValue( 0.50f );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 20, 20, 25, 200 ) );
			pb->SetVertical();
			pb->SetValue( 0.25f );
			pb->SetAlignment( Pos::Top | Pos::CenterH );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 50, 20, 25, 200 ) );
			pb->SetVertical();
			pb->SetValue( 0.40f );
		}
		{
			FK2DEngine2::Controls::ProgressBar* pb = new FK2DEngine2::Controls::ProgressBar( this );
			pb->SetBounds( FK2DEngine2::Rect( 80, 20, 25, 200 ) );
			pb->SetVertical();
			pb->SetAlignment( Pos::Bottom | Pos::CenterH );
			pb->SetValue( 0.65f );
		}
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitProgressBar, L"ProgressBar" );
//------------------------------------------------------------------------
class UnitProperties : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitProperties, GUnit )
	{
		{
			FK2DEngine2::Controls::Properties* props = new FK2DEngine2::Controls::Properties( this );
			props->SetBounds( 10, 10, 150, 300 );
			{
				{
					FK2DEngine2::Controls::PropertyRow* pRow = props->Add( L"First Name" );
					pRow->onChange.Add( this, &UnitProperties::OnFirstNameChanged );
				}
				props->Add( L"Middle Name" );
				props->Add( L"Last Name" );
			}
		}
		{
			FK2DEngine2::Controls::PropertyTree* ptree = new FK2DEngine2::Controls::PropertyTree( this );
			ptree->SetBounds( 200, 10, 200, 200 );
			{
				FK2DEngine2::Controls::Properties* props = ptree->Add( L"Item One" );
				props->Add( L"Middle Name" );
				props->Add( L"Last Name" );
				props->Add( L"Four" );
			}
			{
				FK2DEngine2::Controls::Properties* props = ptree->Add( L"Item Two" );
				props->Add( L"More Items" );
				props->Add( L"Checkbox", new FK2DEngine2::Controls::Property::Checkbox( props ), L"1" );
				props->Add( L"To Fill" );
				props->Add( L"ColorSelector", new FK2DEngine2::Controls::Property::ColorSelector( props ), L"255 0 0" );
				props->Add( L"Out Here" );
				// Combo Box Test
				{
					FK2DEngine2::Controls::Property::ComboBox* pCombo = new FK2DEngine2::Controls::Property::ComboBox( props );
					pCombo->GetComboBox()->AddItem( L"Option One", "one" );
					pCombo->GetComboBox()->AddItem( L"Number Two", "two" );
					pCombo->GetComboBox()->AddItem( L"Door Three", "three" );
					pCombo->GetComboBox()->AddItem( L"Four Legs", "four" );
					pCombo->GetComboBox()->AddItem( L"Five Birds", "five" );
					FK2DEngine2::Controls::PropertyRow* pRow = props->Add( L"ComboBox", pCombo, L"1" );
					pRow->onChange.Add( this, &UnitProperties::OnFirstNameChanged );
				}
			}
			ptree->ExpandAll();
		}
	}

	void OnFirstNameChanged( Controls::Base* pControl )
	{
		FK2DEngine2::Controls::PropertyRow* pRow = ( FK2DEngine2::Controls::PropertyRow* ) pControl;
		UnitPrint( Utility::Format( L"First Name Changed: %ls", pRow->GetProperty()->GetPropertyValue().GetUnicode().c_str() ) );
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitProperties, L"Properties" );
//------------------------------------------------------------------------
class UnitRadioButton : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitRadioButton, GUnit )
	{
		FK2DEngine2::Controls::RadioButtonController* rc = new FK2DEngine2::Controls::RadioButtonController( this );
		rc->AddOption( "Option 1" );
		rc->AddOption( "Option 2" );
		rc->AddOption( "Option 3" );
		rc->AddOption( L"\u0627\u0644\u0622\u0646 \u0644\u062D\u0636\u0648\u0631" );
		rc->SetBounds( 30, 30, 200, 200 );
		rc->onSelectionChange.Add( this, &UnitRadioButton::OnChange );
	}

	void OnChange( Controls::Base* pControl )
	{
		FK2DEngine2::Controls::RadioButtonController* rc = ( FK2DEngine2::Controls::RadioButtonController* ) pControl;
		FK2DEngine2::Controls::LabeledRadioButton* pSelected = rc->GetSelected();
		UnitPrint( Utility::Format( L"RadioButton changed (using 'OnChange' event)\n Chosen Item: '%ls'", pSelected->GetLabel()->GetText().GetUnicode().c_str() ) );
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitRadioButton, L"RadioButton" );
//------------------------------------------------------------------------
class UnitScrollControl : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitScrollControl, GUnit )
	{
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 10, 10, 100, 100 );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Twice As Big" );
			pTestButton->SetBounds( 0, 0, 200, 200 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 110, 10, 100, 100 );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Same Size" );
			pTestButton->SetBounds( 0, 0, 100, 100 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 210, 10, 100, 100 );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Wide" );
			pTestButton->SetBounds( 0, 0, 200, 50 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 310, 10, 100, 100 );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Tall" );
			pTestButton->SetBounds( 0, 0, 50, 200 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 410, 10, 100, 100 );
			pCtrl->SetScroll( false, true );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Vertical" );
			pTestButton->SetBounds( 0, 0, 200, 200 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 510, 10, 100, 100 );
			pCtrl->SetScroll( true, false );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Horinzontal" );
			pTestButton->SetBounds( 0, 0, 200, 200 );
		}
		// Bottom Row
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 10, 110, 100, 100 );
			pCtrl->SetAutoHideBars( true );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Twice As Big" );
			pTestButton->SetBounds( 0, 0, 200, 200 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 110, 110, 100, 100 );
			pCtrl->SetAutoHideBars( true );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Same Size" );
			pTestButton->SetBounds( 0, 0, 100, 100 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 210, 110, 100, 100 );
			pCtrl->SetAutoHideBars( true );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Wide" );
			pTestButton->SetBounds( 0, 0, 200, 50 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 310, 110, 100, 100 );
			pCtrl->SetAutoHideBars( true );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Tall" );
			pTestButton->SetBounds( 0, 0, 50, 200 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 410, 110, 100, 100 );
			pCtrl->SetAutoHideBars( true );
			pCtrl->SetScroll( false, true );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Vertical" );
			pTestButton->SetBounds( 0, 0, 200, 200 );
		}
		{
			FK2DEngine2::Controls::ScrollControl* pCtrl = new FK2DEngine2::Controls::ScrollControl( this );
			pCtrl->SetBounds( 510, 110, 100, 100 );
			pCtrl->SetAutoHideBars( true );
			pCtrl->SetScroll( true, false );
			Controls::Button* pTestButton = new Controls::Button( pCtrl );
			pTestButton->SetText( L"Horinzontal" );
			pTestButton->SetBounds( 0, 0, 200, 200 );
		}
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitScrollControl, L"Scroll" );
//------------------------------------------------------------------------
class UnitSlider : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitSlider, GUnit )
	{
		{
			FK2DEngine2::Controls::HorizontalSlider* pSlider = new FK2DEngine2::Controls::HorizontalSlider( this );
			pSlider->SetPos( 10, 10 );
			pSlider->SetSize( 150, 20 );
			pSlider->SetRange( 0, 100 );
			pSlider->SetFloatValue( 25 );
			pSlider->onValueChanged.Add( this, &UnitSlider::SliderMoved );
		}
		{
			FK2DEngine2::Controls::HorizontalSlider* pSlider = new FK2DEngine2::Controls::HorizontalSlider( this );
			pSlider->SetPos( 10, 40 );
			pSlider->SetSize( 150, 20 );
			pSlider->SetRange( 0, 100 );
			pSlider->SetFloatValue( 20 );
			pSlider->SetNotchCount( 10 );
			pSlider->SetClampToNotches( true );
			pSlider->onValueChanged.Add( this, &UnitSlider::SliderMoved );
		}
		{
			FK2DEngine2::Controls::VerticalSlider* pSlider = new FK2DEngine2::Controls::VerticalSlider( this );
			pSlider->SetPos( 160, 10 );
			pSlider->SetSize( 20, 200 );
			pSlider->SetRange( 0, 100 );
			pSlider->SetFloatValue( 25 );
			pSlider->onValueChanged.Add( this, &UnitSlider::SliderMoved );
		}
		{
			FK2DEngine2::Controls::VerticalSlider* pSlider = new FK2DEngine2::Controls::VerticalSlider( this );
			pSlider->SetPos( 190, 10 );
			pSlider->SetSize( 20, 200 );
			pSlider->SetRange( 0, 100 );
			pSlider->SetFloatValue( 20 );
			pSlider->SetNotchCount( 10 );
			pSlider->SetClampToNotches( true );
			pSlider->onValueChanged.Add( this, &UnitSlider::SliderMoved );
		}
	}

	void SliderMoved( Base* pControl )
	{
		FK2DEngine2::Controls::Slider* pSlider = ( FK2DEngine2::Controls::Slider* ) pControl;
		UnitPrint( Utility::Format( L"Slider Value: %.2f", ( float ) pSlider->GetFloatValue() ) );
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitSlider, L"Slider" );
//------------------------------------------------------------------------
class UnitStatusBar : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitStatusBar, GUnit )
	{
		Dock( Pos::Fill );
		FK2DEngine2::Controls::StatusBar* pStatus = new FK2DEngine2::Controls::StatusBar( this );
		FK2DEngine2::Controls::Label* pRight = new FK2DEngine2::Controls::Label( pStatus );
		pRight->SetText( L"Label Added to Right" );
		pStatus->AddControl( pRight, true );
	}

	void Layout( FK2DEngine2::Skin::Base* skin ) {}
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitStatusBar, L"StatusBar" );
//------------------------------------------------------------------------
class UnitTabControl : public GUnit
{
public:

	Controls::TabControl* m_pDockControlLeft;

	FK_CONTROL_INLINE( UnitTabControl, GUnit )
	{
		{
			m_pDockControlLeft = new Controls::TabControl( this );
			m_pDockControlLeft->SetBounds( 10, 10, 200, 200 );
			{
				Controls::TabButton* pButton = m_pDockControlLeft->AddPage( L"Controls" );
				Base* pPage = pButton->GetPage();
				{
					Controls::RadioButtonController* pRadio = new Controls::RadioButtonController( pPage );
					pRadio->SetBounds( 10, 10, 100, 100 );
					pRadio->AddOption( "Top" )->Select();
					pRadio->AddOption( "Bottom" );
					pRadio->AddOption( "Left" );
					pRadio->AddOption( "Right" );
					pRadio->onSelectionChange.Add( this, &ThisClass::OnDockChange );
				}
			}
			m_pDockControlLeft->AddPage( L"Red" );
			m_pDockControlLeft->AddPage( L"Green" );
			m_pDockControlLeft->AddPage( L"Blue" );
		}
		{
			Controls::TabControl* pDragMe = new Controls::TabControl( this );
			pDragMe->SetBounds( 220, 10, 200, 200 );
			pDragMe->AddPage( L"You" );
			pDragMe->AddPage( L"Can" );
			pDragMe->AddPage( L"Reorder" )->SetImage( L"test16.png" );
			pDragMe->AddPage( L"These" );
			pDragMe->AddPage( L"Tabs" );
			pDragMe->SetAllowReorder( true );
		}
	}

	void OnDockChange( FK2DEngine2::Controls::Base* pControl )
	{
		FK2DEngine2::Controls::RadioButtonController* rc = ( FK2DEngine2::Controls::RadioButtonController* ) pControl;

		if ( rc->GetSelectedLabel() == L"Top" )		{ m_pDockControlLeft->SetTabStripPosition( Pos::Top ); }
		if ( rc->GetSelectedLabel() == L"Bottom" )	{ m_pDockControlLeft->SetTabStripPosition( Pos::Bottom ); }
		if ( rc->GetSelectedLabel() == L"Left" )	{ m_pDockControlLeft->SetTabStripPosition( Pos::Left ); }
		if ( rc->GetSelectedLabel() == L"Right" )	{ m_pDockControlLeft->SetTabStripPosition( Pos::Right ); }
	}

	FK2DEngine2::Font	m_Font;
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitTabControl, L"TabControl" );
//------------------------------------------------------------------------
class UnitTextBox : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitTextBox, GUnit )
	{
		{
			FK2DEngine2::Controls::TextBox* label = new FK2DEngine2::Controls::TextBox( this );
			label->SetText( "" );
			label->SetPos( 10, 10 );
			label->onTextChanged.Add( this, &ThisClass::OnEdit );
			label->onReturnPressed.Add( this, &ThisClass::OnSubmit );
		}
		{
			FK2DEngine2::Controls::TextBox* label = new FK2DEngine2::Controls::TextBox( this );
			label->SetText( "Normal Everyday Label" );
			label->SetPos( 10, 10 + 25 );
		}
		{
			FK2DEngine2::Controls::TextBox* label = new FK2DEngine2::Controls::TextBox( this );
			label->SetText( "Select All Text On Focus" );
			label->SetPos( 10, 10 + 25 * 2 );
			label->SetSelectAllOnFocus( true );
		}
		{
			FK2DEngine2::Controls::TextBox* label = new FK2DEngine2::Controls::TextBox( this );
			label->SetText( L"Different Coloured Text, for some reason" );
			label->SetTextColor( FK2DEngine2::Color( 255, 0, 255, 255 ) );
			label->SetPos( 10, 10 + 25 * 3 );
		}
		{
			FK2DEngine2::Controls::TextBoxNumeric* label = new FK2DEngine2::Controls::TextBoxNumeric( this );
			label->SetText( L"2004" );
			label->SetTextColor( FK2DEngine2::Color( 255, 0, 255, 255 ) );
			label->SetPos( 10, 10 + 25 * 4 );
		}
		{
			m_Font.facename = L"Impact";
			m_Font.size = 50;
			FK2DEngine2::Controls::TextBox* label = new FK2DEngine2::Controls::TextBox( this );
			label->SetText( L"Different Font" );
			label->SetPos( 10, 10 + 25 * 5 );
			label->SetFont( &m_Font );
			label->SetSize( 200, 55 );
		}
		{
			FK2DEngine2::Controls::TextBoxMultiline* label = new FK2DEngine2::Controls::TextBoxMultiline( this );
			label->SetText( "I think we're losing sight of the real issue here, which is: what are we gonna call ourselves?\n\nErm, and I think it comes down to a choice between \"The League Against Salivating Monsters\" or my own personal preference, which is \"The Committee for the Liberation and Integration of Terrifying Organisms and their Rehabilitation Into Society.\" Erm, one drawback with that: the abbreviation is \"CLITORIS.\"" );
			label->SetPos( 220, 10 );
			label->SetSize( 200, 180 );
		}
	}

	void OnEdit( FK2DEngine2::Controls::Base* pControl )
	{

		FK2DEngine2::Controls::TextBox* textbox = ( FK2DEngine2::Controls::TextBox* )( pControl );
		UnitPrint( Utility::Format( L"Textbox Edit: [%ls]\n", textbox->GetText().GetUnicode().c_str() ) );
	}

	void OnSubmit( FK2DEngine2::Controls::Base* pControl )
	{
		FK2DEngine2::Controls::TextBox* textbox = ( FK2DEngine2::Controls::TextBox* )( pControl );
		UnitPrint( Utility::Format( L"Textbox Submit: [%ls]\n", textbox->GetText().GetUnicode().c_str() ) );
	}

	FK2DEngine2::Font	m_Font;
};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitTextBox, L"TextBox" );
//------------------------------------------------------------------------
class UnitTreeControl : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitTreeControl, GUnit )
	{
		{
			FK2DEngine2::Controls::TreeControl* ctrl = new FK2DEngine2::Controls::TreeControl( this );
			ctrl->AddNode( L"Node One" );
			FK2DEngine2::Controls::TreeNode* pNode = ctrl->AddNode( L"Node Two" );
			pNode->AddNode( L"Node Two Inside" );
			pNode->AddNode( L"Eyes" );
			pNode->AddNode( L"Brown" )->AddNode( L"Node Two Inside" )->AddNode( L"Eyes" )->AddNode( L"Brown" );
			pNode->AddNode( L"More" );
			pNode->AddNode( L"Nodes" );
			ctrl->AddNode( L"Node Three" );
			ctrl->SetBounds( 30, 30, 200, 200 );
			ctrl->ExpandAll();
		}
		{
			FK2DEngine2::Controls::TreeControl* ctrl = new FK2DEngine2::Controls::TreeControl( this );
			ctrl->AllowMultiSelect( true );
			ctrl->AddNode( L"Node One" );
			FK2DEngine2::Controls::TreeNode* pNode = ctrl->AddNode( L"Node Two" );
			pNode->AddNode( L"Node Two Inside" );
			pNode->AddNode( L"Eyes" );
			FK2DEngine2::Controls::TreeNode* pNodeTwo = pNode->AddNode( L"Brown" )->AddNode( L"Node Two Inside" )->AddNode( L"Eyes" );
			pNodeTwo->AddNode( L"Brown" );
			pNodeTwo->AddNode( L"Green" );
			pNodeTwo->AddNode( L"Slime" );
			pNodeTwo->AddNode( L"Grass" );
			pNodeTwo->AddNode( L"Pipe" );
			pNode->AddNode( L"More" );
			pNode->AddNode( L"Nodes" );
			ctrl->AddNode( L"Node Three" );
			ctrl->SetBounds( 240, 30, 200, 200 );
			ctrl->ExpandAll();
		}
	}

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitTreeControl, L"TreeControl" );
//------------------------------------------------------------------------
class UnitWindow : public GUnit
{
public:

	FK_CONTROL_INLINE( UnitWindow, GUnit )
	{
		{
			Controls::Button* pButton = new Controls::Button( this );
			pButton->SetText( "Normal Window" );
			pButton->onPress.Add( this, &ThisClass::OpenWindow );
			pButton->SetPos( 0, 0 );
		}
		{
			Controls::Button* pButton = new Controls::Button( this );
			pButton->SetText( "Modal Window" );
			pButton->onPress.Add( this, &ThisClass::OpenModalWindow );
			pButton->SetPos( 0, 32 );
		}
		m_iWindowCount = 1;
	}

	void OpenWindow()
	{
		Controls::WindowControl* pWindow = new Controls::WindowControl( GetCanvas() );
		pWindow->SetTitle( Utility::Format( L"Window %i", m_iWindowCount ) );
		pWindow->SetSize( 200 + rand() % 100, 200 + rand() % 100 );
		pWindow->SetPos( rand() % 700, rand() % 400 );
		pWindow->SetDeleteOnClose( true );
		m_iWindowCount++;
	}

	void OpenModalWindow()
	{
		Controls::WindowControl* pWindow = new Controls::WindowControl( GetCanvas() );
		pWindow->SetTitle( Utility::Format( L"Window %i", m_iWindowCount ) );
		pWindow->SetSize( 200 + rand() % 100, 200 + rand() % 100 );
		pWindow->MakeModal( true );
		pWindow->Position( Pos::Center );
		pWindow->SetDeleteOnClose( true );
		m_iWindowCount++;
	}

	int m_iWindowCount;

};
//------------------------------------------------------------------------
DEFINE_UNIT_TEST( UnitWindow, L"Window" );
//------------------------------------------------------------------------