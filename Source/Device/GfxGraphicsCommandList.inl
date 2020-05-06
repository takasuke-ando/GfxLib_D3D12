#ifndef __INLUCDE_GFXGRAPHICSCOMMANDLIST_INL__
#define	__INLUCDE_GFXGRAPHICSCOMMANDLIST_INL__


namespace GfxLib
{


	inline void	GraphicsCommandList::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology) {
		if (m_PipelineState.PrimitiveTopologyType != topology) {
			m_bPipelineDirty = true;
			m_PipelineState.PrimitiveTopologyType = topology;
		}
	}





	inline void GraphicsCommandList::IASetIndexBuffer(
		const D3D12_INDEX_BUFFER_VIEW *pView)
	{
		GetD3DCommandList()->IASetIndexBuffer(pView);
	}

	inline void GraphicsCommandList::IASetVertexBuffers(
		UINT StartSlot,
		UINT NumViews,
		const D3D12_VERTEX_BUFFER_VIEW *pViews)
	{
		GetD3DCommandList()->IASetVertexBuffers(StartSlot, NumViews, pViews);
	}


	inline void GraphicsCommandList::IASetPrimitiveTopology(
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology)
	{
		GetD3DCommandList()->IASetPrimitiveTopology(PrimitiveTopology);
	}


	inline void GraphicsCommandList::SetGraphicsRootDescriptorTable(
		uint32_t RootParameterIndex,
		const DescriptorBuffer &descBuffer)
	{

		m_pCmdList->SetGraphicsRootDescriptorTable(RootParameterIndex, descBuffer.GetGPUDescriptorHandle());

	}




}


#endif // !__INLUCDE_GFXGRAPHICSCOMMANDLIST_INL__
