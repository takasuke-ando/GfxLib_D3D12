#ifndef __INCLUDE_GFXSHADERTABLE_H__
#define __INCLUDE_GFXSHADERTABLE_H__


#include "Resource/GfxGpuBufferRange.h"

namespace GfxLib
{

	/*
		@brief
			主にRayTracingで使うシェーダテーブルをカプセル化
			CommandListからバッファを受け取って、コピーを行う

			オンザフライで処理を行うので、常にテーブルはコマンドバッファから確保する
	
	*/

	class CommandList;

	class ShaderTable
	{

	public:
		ShaderTable(CommandList& cmdList , uint32_t numShaderRecords, uint32_t shaderRecordSize);
		~ShaderTable();


		/*
			@brief
				シェーダレコード（シェーダIdentifierおよびローカルルートアーギュメント）
				をシェーダテーブルに追記する
		
		*/
		void	AddRecord(const void* shaderIdentifier, const void* localRootArguments, uint32_t localRootArgumentsSize);


	private:
		uint32_t m_numShaderRecords;
		uint32_t m_shaderRecordSize;


		GpuBufferRange	m_gpuBuffer;

		uint8_t* m_pNextCopyDest;	//!<	次のレコードコピーアドレス

	};







}



#endif //__INCLUDE_GFXSHADERTABLE_H__