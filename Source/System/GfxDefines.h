#ifndef __INCLUDE_GFXDEFINES_H__
#define	__INCLUDE_GFXDEFINES_H__


/***************************************************************
	@brief	各種定義など
	@par	[説明]
		各種構造体やenum などをここに定義する
/***************************************************************/



namespace GfxLib
{

	/*
		リソースバリアタイプ
	*/
	enum class BarrierType : uint32_t {

		Transition = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		Aliasing = D3D12_RESOURCE_BARRIER_TYPE_ALIASING,
		Uav = D3D12_RESOURCE_BARRIER_TYPE_UAV,

	};


	/*
		リソースステート
		D3D12_RESOURCE_STATESにキャスト可能
	*/
	enum class ResourceStates : uint32_t {

		Present = D3D12_RESOURCE_STATE_PRESENT,
		RenderTarget = D3D12_RESOURCE_STATE_RENDER_TARGET,


	};


	/*
		Descriptor Range Type 
		D3D12_DESCRIPTOR_RANGE_TYPE にキャスト可能
		DescriptorTableに接続される複数のRange情報のうちの一つ
	*/
	enum class DescriptorRangeType : uint32_t {

		Srv = D3D12_DESCRIPTOR_RANGE_TYPE_SRV ,
		Uav = D3D12_DESCRIPTOR_RANGE_TYPE_UAV ,
		Cbv = D3D12_DESCRIPTOR_RANGE_TYPE_CBV ,
		Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,

	};



	/*
		
		D3D12_SHADER_VISIBILITYにキャスト可能
	
	*/

	enum class ShaderVisibility : uint32_t {
		All = D3D12_SHADER_VISIBILITY_ALL ,
		Vertex = D3D12_SHADER_VISIBILITY_VERTEX ,
		Hull = D3D12_SHADER_VISIBILITY_HULL ,
		Domain = D3D12_SHADER_VISIBILITY_DOMAIN ,
		Geometry = D3D12_SHADER_VISIBILITY_GEOMETRY ,
		Pixel  =D3D12_SHADER_VISIBILITY_PIXEL ,

	};

	/*
	
		D3D12_DESCRIPTOR_RANGE 構造体のクローン
		
	
	*/
	struct DESCRIPTOR_RANGE {
		DescriptorRangeType	RangeType;
		uint32_t NumDescriptors;		//	デスクリプタ数
		uint32_t BaseShaderRegister;	//	
		//UINT RegisterSpace;			//	シェーダでは→register(t3,space5)のような書き方でデスクリプタ配列を定義できるみたい
		//UINT OffsetInDescriptorsFromTableStart;	// ?

	};




}





#endif // !__INCLUDE_GFXDEFINES_H__
