/**
*	created:		2013-4-23   22:28
*	filename: 		FKI3DSceneManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKIBase.h"
#include "FKQuad.h"
#include "FKIVideoDriver.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace scene
	{
		namespace S3D
		{
			typedef enum {
				E_NODE_TYPE_EMPTY,
				//E_NODE_TYPE_CAMERA,
				E_NODE_TYPE_LIGHT,
				E_NODE_TYPE_RENDER_SOLID,
				//E_NODE_TYPE_RENDER_TRANSPARENT
			}E_NODE_TYPE;

			class ISceneNode ;
			class IRenderObjectSceneNode;
			class ICameraSceneNode;
			class ILightSceneNode;

			//! 3D�������������
			class I3DSceneManager : public FK2DEngine2::core::IBase
			{
			public:
				virtual const std::wstring getTypeInfo() const {
					return L"I3DSceneManager";
				}
				
				//! �����������һ���ճ����ڵ㡾ͨ��������һЩ�ڵ�ĸ��ڵ㡿
				virtual ISceneNode * addEmptySceneNode(const std::wstring & nodeMane,ISceneNode * parent,
					FK2DEngine2::math::Vector3 position) = 0;

				//! �����������һ��������ڵ㲢��������Ϊ���������������������������Add֮�������á�
				virtual ICameraSceneNode * addCameraSceneNode(const std::wstring & nodeName,ISceneNode * parent,
					FK2DEngine2::math::Vector3 relativeEyePos,
					FK2DEngine2::math::Vector3 at,
					FK2DEngine2::math::Vector3 up) = 0;

				//! �����������һ�����սڵ�
				virtual ILightSceneNode * addLightSceneNode(const std::wstring & nodeName,ISceneNode * parent,
					FK2DEngine2::math::Vector3 position,
					FK2DEngine2::video::SLight light) = 0;

				//! �����������һ�������νڵ�
				virtual IRenderObjectSceneNode * addQuadSceneNode(const std::wstring & nodeName,ISceneNode * parent,
					FK2DEngine2::math::Vector3 position,
					FK2DEngine2::math::Quad quad) = 0;

				//! �����������һ��������ڵ㡾��С = (width,height,depth)��
				virtual IRenderObjectSceneNode * addCubeSceneNode(const std::wstring & nodeName,ISceneNode * parent,
					FK2DEngine2::math::Vector3 position,
					FK2DEngine2::math::Vector3 size) = 0;

				//! ͨ�����ƻ�ȡһ���ڵ�ָ��
				virtual ISceneNode * getNodeByName(const std::wstring & nodeName) = 0;

				//! �ͷ�һ���ڵ�ָ��
				virtual void freeNode(ISceneNode * node) = 0;

				//! �ͷ�ȫ���ڵ�ָ��
				virtual void freeAllNodes() = 0;

				//! ���û�����
				virtual void setActiveCamera(ICameraSceneNode * camera) = 0;

				//! ��ȡ������
				virtual ICameraSceneNode * getActiveCamera() = 0;

				//! ��ȡͼ����Ⱦ��
				virtual FK2DEngine2::video::IVideoDriver * getVideoDriver() = 0;

				//! ��ʼ3D������Ⱦ
				virtual void begin3DScene() = 0;

				//! ��Ⱦ����
				virtual void drawAll () = 0;

				//! ֹͣ3D������Ⱦ
				virtual void end3Dscene () = 0;

				//! ע��һ���ڵ㣬���������Ⱦ
				virtual void registerNodeForRendering(ISceneNode * node,E_NODE_TYPE nodeType) = 0;
				
			};
		}
	}
}
//------------------------------------------------------------------------