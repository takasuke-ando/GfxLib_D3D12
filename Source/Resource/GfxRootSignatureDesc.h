#ifndef __INCLUDE_GFXROOTSIGNATUREDESC_H__
#define __INCLUDE_GFXROOTSIGNATUREDESC_H__


#include <vector>
#include <array>
#include "System/GfxDefines.h"

namespace GfxLib
{

	struct DESCRIPTOR_RANGE;

	class RootSignatureDesc
	{
	public:
		RootSignatureDesc();
		~RootSignatureDesc();


		void Clear();

		/***************************************************************
		@brief		ルートパラメータとして、デスクリプタテーブルを追加する
		@par	[説明]
			ルートパラメータとして、デスクリプタテーブルを追加する
			テーブルには一つのヒープしかBindできない
			複数のRange情報が指定できて、ヒープ内のどこのDescriptorを使用するかを参照する
			一つのテーブル内に、Samplerと、CBV,SRV,UAV を混ぜることはできない
			CBV+UAV は可能だが、 Sampler + CBV などは不可
			これはデスクリプタヒープが異なるため、当然である
		@param[in]	ranges:	レンジ情報の配列。内部でコピーされるのでこのメソッドの呼び出し直後に破棄しても構わない
		@param[in]	numRanges:	レンジ数
		@param[in]	visb:このデスクリプタテーブルは、どのシェーダから参照が可能か
		*/
		void		AddParam_DescriptorTable( const DESCRIPTOR_RANGE *ranges , uint32_t numRanges , ShaderVisibility visb = ShaderVisibility::All );



		/*
			rootSigDesc.AddParam_DescriptorTable(std::array<GfxLib::DESCRIPTOR_RANGE,1>{ GfxLib::DescriptorRangeType::Cbv,1,0 } );
			rootSigDesc.AddParam_DescriptorTable(
				std::array<GfxLib::DESCRIPTOR_RANGE, 2>{ 
					GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Cbv, 1, 0 },
					GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Cbv,1,1 }
				}
			);
			な風に呼び出せる
			いまいちか…
		*/
		template< int SIZE >
		void		AddParam_DescriptorTable(const std::array<DESCRIPTOR_RANGE,SIZE> &rangeArray, ShaderVisibility visb = ShaderVisibility::All) {
			AddParam_DescriptorTable(&rangeArray[0], SIZE, visb);
		}


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

		void		SetFlags(D3D12_ROOT_SIGNATURE_FLAGS flags) { m_Flags = flags;  }
		D3D12_ROOT_SIGNATURE_FLAGS GetFlags() const { return m_Flags; }


	private:


		void		_AddParam_CbvSrvUav(D3D12_ROOT_PARAMETER_TYPE type , uint32_t shaderRegister, ShaderVisibility = ShaderVisibility::All);


		std::vector< D3D12_ROOT_PARAMETER >	m_vecRootParameter;

		D3D12_ROOT_SIGNATURE_FLAGS		m_Flags;

	};






}


#endif // !__INCLUDE_GFXROOTSIGNATUREDESC_H__
