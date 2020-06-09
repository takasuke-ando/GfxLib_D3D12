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


	class RtPipeStateSubobject;	//	基底クラス



	class RtPipelineStateDesc
	{
	public:
		RtPipelineStateDesc();
		~RtPipelineStateDesc();




	private:
		std::vector< RtPipeStateSubobject* >	m_vecSubobject;

	};




	


}




#endif //__INCLUDE_GFXRTPIPELINESTATEDESC_H__
