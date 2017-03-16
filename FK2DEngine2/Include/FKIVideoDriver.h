/**
*	created:		2013-4-23   15:48
*	filename: 		FKIVideoDriver
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <cstdarg>
#include <windows.h>

#include "FKLine3D.h"
#include "FKTriangle.h"
#include "FKQuad.h"

#include "FKException.h"
#include "FKResourceMap.h"
#include "FKLogger.h"
#include "FKIBase.h"

#include "FKEVideoDriverType.h"
#include "FKITexture.h"
#include "FKSColor.h"
#include "FKLight.h"
#include "FKSMaterial.h"
#include "FKIFont.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace video
	{
		//------------------------------------------------------------------------
		// ö�پ�������
		typedef enum{
			E_TRANSFORM_MATRIX_WORLD,
			E_TRANSFORM_MATRIX_VIEW,
			E_TRANSFORM_MATRIX_PROJECTION,
			E_TRAMSFORM_MATRIX_COUNT
		}E_TRANSFORM_MATRIX;
		//------------------------------------------------------------------------
		// ����������ʽö��
		typedef enum{
			E_CLEAR_NONE = 0x00000000,
			E_CLEAR_ALL = 0xffffffff,
			E_CLEAR_COLOR = 0x00000001,
			E_CLEAR_ZBUFFER = 0x00000002,
			E_CLEAR_STRENCIL = 0x00000004,
		}E_CLEAR_BUFFER_BIT;
		//------------------------------------------------------------------------
		// ��Ⱦ��״̬ö��
		typedef enum {
			E_VIDEODRIVER_STATE_ALPHATESTING,
			E_VIDEODRIVER_STATE_ALPHABLENDING,
			E_VIDEODRIVER_STATE_LIGHTING,
			E_VIDEODRIVER_STATE_ZBUFFER,
			E_VIDEODRIVER_STATE_SPECULARENABLE
		}E_VIDEODRIVER_STATE_TYPE;
		//------------------------------------------------------------------------
		class IVideoDriver : public FK2DEngine2::core::IBase
		{
		public :
			virtual ~IVideoDriver(){};
			IVideoDriver(){};

			//! ��ʼ����Ⱦ��
			//! \return �Ƿ�ɹ�
			virtual bool initDriver( int width,int height,int zdepth,HWND hwnd,
									bool fullscreen = false,
									bool zbuffer = false,
									bool alphatesting = false,
									bool alphablend = true,
									bool lighting = false,
									bool immediate = true) = 0;
			//! �ر���Ⱦ�� 
			//! \return �Ƿ�ɹ�
			virtual bool shutDriver() = 0;

			//! ������Ⱦ��
			//! \return ��Ⱦ��״̬
			virtual int reset(int width,int height,int zdepth,HWND hwnd,
								bool fullscreen = false,
								bool zbuffer = false,
								bool alphatesting = false,
								bool alphablend = true,
								bool lighting = false,
								bool immediate = true) = 0;

			//! ������Ⱦ����ת������
			virtual int setTransform(E_TRANSFORM_MATRIX tm,FK2DEngine2::math::Matrix4 & m4) = 0;

			//! ������Ⱦ�����ӿ�
			virtual int setViewport(unsigned int x,unsigned int y,unsigned int width,unsigned int height) = 0;
			
			//! ���õ�ǰ��Ⱦ����
			virtual int setMaterial(const SMaterial & mat) = 0;

			//! ���ù������ڵĹ�����Ϣ
			virtual int setLight(const SLight & light,int index) = 0;

			//! ����Ⱦ���Ĺ����������ӹ���
			//!\return ���ظù����ڹ������е�����ֵ
			virtual int addLight(const SLight & light) = 0;

			//! ������Ⱦ���ĵ�ǰ����
			virtual void ClearCurTexture() = 0;

			//! ��ȡ��ǰ��Ⱦ���Ƿ�������
			virtual bool CurHasTexture() = 0;

			//! ɾ��ȫ������
			virtual void deleteAllLights() = 0;

			//! ���õ�ǰ��Ⱦ���� ���ڲ���Ը����� addRef��
			virtual int setTexture(ITexture * texture) = 0;

			//! ������Ⱦ��״̬
			virtual int setVideodriverState(E_VIDEODRIVER_STATE_TYPE state,bool enable) = 0;

			//! ������Ⱦ�����������
			//!\params renderTarget: ��Ⱦ�������� �������ֵΪNULL�����ʾ��Ⱦ��������Ϊȫ��Ļ��
			virtual int setRenderTarget(ITexture * renderTarget,unsigned long flag = E_CLEAR_NONE,SColor color = SColor::black()) = 0;

			//! ��ȡ��ǰ��Ⱦ���� �������Ⱦ������ȫ��Ļ���򷵻� NULL��
			virtual ITexture * getRenderTarget() = 0;
			
			//! ����ָ����ŵĹ���
			virtual int enableLight(int index,bool enable) = 0;
			
			//! ���ȫ�����壬��ʼ������Ⱦ
			virtual int beginScene(SColor color,unsigned long flag) = 0;

			//! ��Ⱦ��ϣ����к�̨����������
			virtual int endScene() = 0;

			//! ����һ������
			virtual int drawPixel(FK2DEngine2::math::Vertex & pixel) = 0;

			//! ����һ����
			virtual int drawLine(FK2DEngine2::math::Line3D & line) = 0;
			
			//! ����һ��������
			virtual int drawTriangle(FK2DEngine2::math::Triangle & triangle) = 0;

			//! ����һ���ķ���
			virtual int drawQuad(FK2DEngine2::math::Quad & quad) = 0;

			//! ����һ����������
			virtual int drawTriangleList(FK2DEngine2::math::Triangle * triangleList,int count) = 0;

			//! ����һ����������
			virtual int drawTriangleFan(FK2DEngine2::math::Vertex * triangleFanList,int vertexCount) = 0;

			//! ����һ�������α�
			virtual int drawTriangleStrip(FK2DEngine2::math::Vertex * triangleStripList,int vertexCount) = 0;

			//! ����һ������������������
			virtual int drawIndexTriangleList(FK2DEngine2::math::Vertex * trianglelist,int vertexCount,const unsigned short * indexBuffer,int triangleCount) = 0;

			//! ����һ������������������
			virtual int drawIndexTriangleFan(FK2DEngine2::math::Vertex * trianglelist,int vertexCount,const unsigned short * indexBuffer,int triangleCount) = 0;

			//! ����һ���������������α�
			virtual int drawIndexTriangleStrip(FK2DEngine2::math::Vertex * trianglelist,int vertexCount,const unsigned short * indexBuffer,int triangleCount) = 0;

			//! ��������
			virtual int drawText(IFont * font,float x,float y,SColor color,const char * format,...) = 0;

			//! ��һ�����������ڻ�������
			virtual int drawTextInRect(IFont * font,float centerX,float centerY,float width,float height,SColor color, const char * format,...) = 0;
			
			//! ����һ������ ��ͨ��������������������ᱻ���뵽��Դ���ڡ�
			virtual ITexture * createTexture(const std::wstring & texName,int width,int height) = 0;

			//! ����һ��������Դ��ͨ��������������������ᱻ���뵽��Դ���ڡ�
			virtual ITexture * loadTexture(const std::wstring & texName,const std::wstring & fileName) = 0;

			//! �ͷ�һ������ ������Դ����ɾ����
			virtual bool freeTexture(const std::wstring & texName) = 0;

			//! �ͷ�һ������ ������Դ����ɾ����
			virtual bool freeTexture(ITexture * texture) = 0;

			//! ����һ������ ��������ᱻ���뵽��Դ���ڡ�
			virtual IFont * loadFont(const std::wstring & resourceName,int height,int width,int weight,const std::wstring & fontStyleName) = 0;

			//! �ͷ�һ������ ������Դ����ɾ����
			virtual bool freeFont(const std::wstring & resourceName) = 0;

			//! �ͷ�һ������ ������Դ����ɾ����
			virtual bool freeFont(IFont * font) = 0;

		public:
			FK2DEngine2::core::CResourceMap * getResourceMap(){
				return resourceMap;
			}
			FK2DEngine2::core::CResourceMap * setResourceMap(FK2DEngine2::core::CResourceMap * resourceMap_){
				FK2DEngine2::core::CResourceMap * ret = resourceMap;
				resourceMap = resourceMap_;
				return ret;
			}
		protected: 
			FK2DEngine2::core::CResourceMap * resourceMap;
		};
	}
}
//------------------------------------------------------------------------