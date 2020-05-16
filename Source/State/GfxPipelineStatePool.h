#ifndef __INLUCDE_GFXPIPELINESTATEPOOL_H__
#define __INLUCDE_GFXPIPELINESTATEPOOL_H__

/***************************************************************
	@file	GfxPipelineStatePool
	@par	[説明]
		PSO作成結果のキャッシング
		内部でスレッド シリアライズ処理は行わない
		ミューテックスなど外部で行う
***************************************************************/


#include "System/GfxDefines.h"

#include <map>


namespace GfxLib
{

	class PipelineState;

	class PipelineStatePool
	{
	public:
		typedef uint32_t HashType;
	public:
		PipelineStatePool();
		~PipelineStatePool();


		void Finalize();


		/***************************************************************
		@brief	登録されたPipelineStateを取得
		@par	[説明]
		@param
		*/
		PipelineState*	Find(HashType hash) const;


		/***************************************************************
		@brief	PSOの新規登録
		@par	[説明]
			PSOの新規登録
			実態の削除は内部で行われる
		@param
		*/
		void	Add(HashType hash, PipelineState *pso);

	private:

		typedef std::map<HashType, PipelineState*>		PSOMap;
		PSOMap	m_PsoMap;


	};






}



#endif  //__INLUCDE_GFXPIPELINESTATEPOOL_H__