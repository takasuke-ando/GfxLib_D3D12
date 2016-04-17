#ifndef __INCLUDE_GFXROOTSIGNATUREDESC_H__
#define __INCLUDE_GFXROOTSIGNATUREDESC_H__


#include <vector>
#include "System/GfxDefines.h"

namespace GfxLib
{

	struct DESCRIPTOR_RANGE;

	class RootSignatureDesc
	{
	public:
		RootSignatureDesc();
		~RootSignatureDesc();


		/***************************************************************
		@brief		ルートパラメータとして、デスクリプタテーブルを追加する
		@par	[説明]
			ルートパラメータとして、デスクリプタテーブルを追加する
			テーブルには一つのヒープしかBindできない
			複数のRange情報が指定できて、ヒープ内のどこのDescriptorを使用するかを参照する
		@param[in]	ranges:	レンジ情報の配列。内部でコピーされるのでこのメソッドの呼び出し直後に破棄しても構わない
		@param[in]	numRanges:	レンジ数
		@param[in]	visb:このデスクリプタテーブルは、どのシェーダから参照が可能か
		*/
		void		AddParam_DescriptorTable( const DESCRIPTOR_RANGE *ranges , uint32_t numRanges , ShaderVisibility visb = ShaderVisibility::All );


		/***************************************************************
		@brief		ルートパラメータとして32Bit定数を追加する
		@par	[説明]
			ルートパラメータとして32Bit定数を追加する
			軽量なシェーダではこの機能を使って、面倒な定数バッファ管理などをスキップできる
		@param
		*/
		void		AddParam_32BitConstants( uint32_t num32bitValues , uint32_t shaderRegister, ShaderVisibility = ShaderVisibility::All);


		void		AddParam_Cbv(uint32_t shaderRegister, ShaderVisibility = ShaderVisibility::All);
		void		AddParam_Srv(uint32_t shaderRegister, ShaderVisibility = ShaderVisibility::All);
		void		AddParam_Uav(uint32_t shaderRegister, ShaderVisibility = ShaderVisibility::All);



		const D3D12_ROOT_PARAMETER*	GetRootParameterArray() const {	return &(*m_vecRootParameter.begin());	}
		uint32_t					GetRootParameterCount() const { return (uint32_t)m_vecRootParameter.size(); }


	private:


		void		_AddParam_CbvSrvUav(D3D12_ROOT_PARAMETER_TYPE type , uint32_t shaderRegister, ShaderVisibility = ShaderVisibility::All);


		std::vector< D3D12_ROOT_PARAMETER >	m_vecRootParameter;

	};






}


#endif // !__INCLUDE_GFXROOTSIGNATUREDESC_H__
