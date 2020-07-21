#ifndef __INCLUDE_GFXSTATEOBJECT_H__
#define __INCLUDE_GFXSTATEOBJECT_H__

#include "System/GfxDefines.h"


namespace GfxLib
{


	class StateObject
	{
	public:
		StateObject();
		~StateObject();


		bool	Initialize(const D3D12_STATE_OBJECT_DESC &desc );

		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		ID3D12StateObject* GetD3DStateObject()const { return m_d3dStateObject; }


		const void* GetShaderIdentifier(const wchar_t* shaderName)const {
			return m_d3dProperties->GetShaderIdentifier(shaderName);
		}

		
	private:

		D3DPtr<ID3D12StateObject>				m_d3dStateObject;
		D3DPtr< ID3D12StateObjectProperties>	m_d3dProperties;		//	ShaderIdentifier取得用


	};





}





#endif //__INCLUDE_GFXSTATEOBJECT_H__