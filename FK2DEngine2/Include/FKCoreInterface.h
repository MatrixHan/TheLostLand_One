/**
*	created:		2013-4-23   15:09
*	filename: 		FKCoreInterface
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <string>
#include <iostream>
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace core
	{
		//! �ṩ���ü������ܵĻ���
		class IRefCount
		{
		public :
			virtual ~IRefCount(){}
			IRefCount():referenceCounter(0){} 
			void addRef(){
				referenceCounter ++;
			}
			bool decRef(){
				if (referenceCounter <= 0)
				{
					return false;
				}
				referenceCounter --;
				if (referenceCounter == 0)
				{
					delete this;
					return true;
				}
				return false;
			}
			int getRef() const
			{
				return referenceCounter;
			}
		private:
			int referenceCounter;
		};

		//! �ṩ������Ϣ���ܵĻ���
		class ITypeInfomation
		{
		public :
			virtual ~ITypeInfomation(){}
			virtual const std::wstring getTypeInfo() const  = 0;
		};

		//! �ṩ������Ϣ���ܵĻ���
		class IObjectInfomation
		{
		public :
			virtual ~IObjectInfomation(){}
			IObjectInfomation(std::wstring objectName_)
				:objectName(objectName_.c_str()){}
			const std::wstring & getObjectName() const {
				return objectName;
			}
		private:
			std::wstring objectName;
		};

		//! �ṩ���������ܵĻ���
		class IPersistence
		{
			friend std::istream & operator >> (std::istream & inStream,IPersistence & persistence);
			friend std::ostream & operator << (std::ostream & outStream,IPersistence & persistence);
		public:
			virtual ~IPersistence(){}
		protected:
			virtual void readFromStream(std::istream & inStream) = 0;
			virtual void writeToStream(std::ostream & outStream) = 0;
		};
	}
}
//------------------------------------------------------------------------