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


		D3D12_GPU_VIRTUAL_ADDRESS	GetGpuVirtualAddress() const { return m_gpuBuffer.GetGpuAddr();  }
		size_t						GetSizeInBytes() const { return m_gpuBuffer.GetSize(); }
		size_t						GetStrideInBytes() const { return m_gpuBuffer.GetSize(); }


		D3D12_GPU_VIRTUAL_ADDRESS_RANGE	GetGpuVirtualAddressRange() const {
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE range = {};
			range.StartAddress = m_gpuBuffer.GetGpuAddr();
			range.SizeInBytes = m_gpuBuffer.GetSize();
			return range;
		}

		D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE	GetGpuVirtualAddressRangeAndStride() const {
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE range = {};
			range.StartAddress = m_gpuBuffer.GetGpuAddr();
			range.SizeInBytes = m_gpuBuffer.GetSize();
			range.StrideInBytes = m_shaderRecordSize;
			return range;
		}


	private:
		uint32_t m_numShaderRecords;
		uint32_t m_shaderRecordSize;


		GpuBufferRange	m_gpuBuffer;

		uint8_t* m_pNextCopyDest;	//!<	次のレコードコピーアドレス

	};







}



#endif //__INCLUDE_GFXSHADERTABLE_H__