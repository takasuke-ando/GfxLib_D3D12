/***************************************************************
	@file	GfxPipelineStatePool
	@par	[説明]
		PSO作成結果のキャッシング

***************************************************************/










#include "stdafx.h"


#include "State/GfxPipelineStatePool.h"
#include "State/GfxPipelineState.h"



using  namespace GfxLib;


PipelineStatePool::PipelineStatePool()
{
}

PipelineStatePool::~PipelineStatePool()
{
	Finalize();
}


void PipelineStatePool::Finalize()
{

	//PSOMap::iterator it = m_PsoMap.begin();
	//for (; it != m_PsoMap.end(); ++it) {
	for (auto it : m_PsoMap ){
		PipelineState *state = (it).second;
		delete state;
	}

}


/***************************************************************
@brief	登録されたPipelineStateを取得
@par	[説明]
@param
*/
PipelineState*	PipelineStatePool::Find(HashType hash) const
{

	PSOMap::const_iterator it = m_PsoMap.find(hash);

	if (it == m_PsoMap.end()) {
		return nullptr;
	}

	return (*it).second;


}


/***************************************************************
@brief	PSOの新規登録
@par	[説明]
	PSOの新規登録
	実態の削除は内部で行われる
@param
*/
void	PipelineStatePool::Add(HashType hash, PipelineState *pso)
{

	GFX_ASSERT(m_PsoMap.find(hash) == m_PsoMap.end(), "pso hash conflict!");

	m_PsoMap.insert(std::pair<HashType,PipelineState*>(hash, pso));


}
