/**
*	created:		2013-5-3   3:51
*	filename: 		LoginSceneUI
*	author:			FreeKnight
*	Copyright (C): 	 FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../MainMacro.h"
//------------------------------------------------------------------------
class CLoginSceneUI : public FK2DEngine2::Controls::DockBase
{
public:
	FK_CONTROL( CLoginSceneUI, FK2DEngine2::Controls::DockBase );
private:
	ImagePanel*			m_pTitleImage;			// ����ͼƬ

	Label*				m_pAccoutLabel;			// �˺�
	Label*				m_pPasswordLabel;		// ����
	CheckBoxWithLabel*	m_pSaveAccoutCheckBox;	// ��¼�˺�
	ComboBox*			m_pGameSelectComboBox;	// ѡ����Ϸ
	TextBox*			m_pAccoutTextBox;		// �˺������
	TextBox*			m_pPasswordTextBox;		// ���������
	Button*				m_pEnterGameButton;		// ������Ϸ

	WindowControl*		m_pLoginWindow;			// ��¼���
	Button*				m_pDevelopButton;		// ������Ա
	Button*				m_pOfficalWebButton;	// ��Ϸ����
	Button*				m_pRegisteAccoutButton;	// ע���˺�
	Button*				m_pExitGameButton;		// �뿪��Ϸ

	WindowControl*		m_pConnectUsPanel;		// ��ϵ�������
	WindowControl*		m_pDevelopPanel;		// ������Ա���

	ListBox*			m_pServerListBox;		// �������б�

	Label*				m_pInfoLabel1;			// ��ʾ����1
	Label*				m_pInfoLabel2;			// ��ʾ����2
private:
	IScene*				m_pOwner;
public:
	void				Init();
	void				SetOwner( IScene* scene );
	void				Render( Skin::Base* skin );
private:
	void				onDevelopButton( Controls::Base* pControl );
	void				onConnectUsButton( Controls::Base* pControl );
	void				onRegisteAccoutButton( Controls::Base* pControl );
	void				onExitGameButton( Controls::Base* pControl );
	void				onOpenWebSite( Controls::Base* pControl );
	void				onAccoutEditChange( Controls::Base* pControl );
	void				onEnterGame( Controls::Base* pControl );
	void				onChangeGame( Controls::Base* pControl );
};
//------------------------------------------------------------------------