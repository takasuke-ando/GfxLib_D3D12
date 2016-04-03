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





}





#endif // !__INCLUDE_GFXDEFINES_H__
