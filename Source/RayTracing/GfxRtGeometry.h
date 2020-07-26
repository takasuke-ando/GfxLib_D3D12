#ifndef __INCLUDE_GFXRTGEOMETRY_H__
#define __INCLUDE_GFXRTGEOMETRY_H__



#include "../Resource/GfxBuffer.h"
#include "../Resource/GfxVertexBuffer.h"
#include "../Resource/GfxIndexBuffer.h"

namespace GfxLib
{

	/*
		@brief
			レイトレーシングジオメトリ
			レイトレーシング用ジオメトリ構造をカプセル化
			実際のところ持っているのは頂点バッファとインデックスバッファのみ
	*/
	class RtGeometry
	{
	public:
		RtGeometry();
		~RtGeometry();



		bool	Initialize(const void *vtxData, size_t vtxStrideSize, size_t vtxCount ,
				const void* idxData, Format idxFormat, size_t idxCount);

		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);



		const D3D12_RAYTRACING_GEOMETRY_DESC& GetDesc() const { return m_geomDesc; }

		D3D12_CPU_DESCRIPTOR_HANDLE		GetIndexBufferSRV() const { return m_idxBufferSRV.GetSrvDescHandle();  }


	private:
		VertexBuffer		m_vtxBuffer;
		IndexBuffer			m_idxBuffer;
		ByteAddressBuffer	m_idxBufferSRV;

		D3D12_RAYTRACING_GEOMETRY_DESC	m_geomDesc;

	};




}



#endif //__INCLUDE_GFXRTGEOMETRY_H__
