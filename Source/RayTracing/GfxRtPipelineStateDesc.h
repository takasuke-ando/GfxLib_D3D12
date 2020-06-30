#ifndef __INCLUDE_GFXRTPIPELINESTATEDESC_H__
#define __INCLUDE_GFXRTPIPELINESTATEDESC_H__


#include <vector>

#include "Util/GfxStringContainer.h"

namespace GfxLib
{

	/***************************************************************
	@brief	シェーダクラス
	@par	[説明]
		RTPSOを作成するための
		D3D12_STATE_SUBOBJECT配列を生成するためのヘルパークラス

		サブオブジェクト情報を登録していき
		最終的にResolveを呼び出すことで
		D3D12_STATE_SUBOBJECT配列が構築される


		@param
	*/


	class PipelineStateSubobject;	//	基底クラス


	class PipelineState_DxilLibrary;
	class PipelineState_HitGroup;
	class PipelineState_RaytracingShaderConfig;
	class PipelineState_RootSignature;
	class PipelineState_LocalRootSignature;
	class PipelineState_GlobalRootSignature;
	class PipelineState_SubobjectToExportsAssociation;
	class PipelineState_PipelineConfig;



	class RtPipelineStateDesc
	{
	public:
		RtPipelineStateDesc();
		~RtPipelineStateDesc();


		/*
			D3D12_STATE_SUBOBJECT 配列の構築
		*/
		void Resolve();


		template<class SUBOBJECT>
		SUBOBJECT* CreateSubObject() 
		{

			SUBOBJECT* subobject = new SUBOBJECT;
			m_vecSubobject.push_back(subobject);

			return subobject;
		}


		const	D3D12_STATE_SUBOBJECT* GetStateSubObjects() const { return m_pStateSubObject; }
		uint32_t						GetNumStateSubObjects()	const { return m_numStateSubObjects; }


	private:
		std::vector< PipelineStateSubobject* >	m_vecSubobject;
		
		D3D12_STATE_SUBOBJECT*					m_pStateSubObject;
		uint32_t								m_numStateSubObjects;

	};




	


}




#endif //__INCLUDE_GFXRTPIPELINESTATEDESC_H__
