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




}


#endif // !__INLUCDE_GFXGRAPHICSCOMMANDLIST_INL__
