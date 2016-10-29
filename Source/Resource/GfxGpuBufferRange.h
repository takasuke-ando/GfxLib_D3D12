#ifndef __INCLUDE_GFXGPUBUFFER_H__
#define	__INCLUDE_GFXGPUBUFFER_H__



/***************************************************************
@file	DescriptorBuffer
@par	[説明]
	GPUバッファ
	バッファのサブ領域に対してこのクラスを経由してアクセスする

	リソースの一部領域を切り出してバッファとして
	渡したいときに一時的なインスタンスとして使われるクラス
	内部で保持しているResourceの寿命管理などは行わない
	
	そのためこのクラス自体を何かのクラスのメンバに保持するということは
	基本的にはない
*/


namespace GfxLib
{

	
	class GpuBufferRange
	{
	public:
		GpuBufferRange();
		GpuBufferRange(const GpuBufferRange& rhs) {
			*this = rhs;
		}
		GpuBufferRange(ID3D12Resource* res, size_t startOffset, void* cpuAddr, D3D12_GPU_VIRTUAL_ADDRESS gpuAddr, size_t Size);

		~GpuBufferRange();


		ID3D12Resource*	GetD3DResource() const { return m_pResource; }
		size_t			GetStartOffset()	const { return m_StartOffset; }		//!<	リソーストップからの開始位置
		void*	GetCpuAddr()						const {	return m_pStartAddr;		}
		D3D12_GPU_VIRTUAL_ADDRESS	GetGpuAddr()	const { return m_pStartGpuAddr;		}
		size_t						GetSize()		const { return m_Size;				}


		const GpuBufferRange&	operator = (const GpuBufferRange& rhs) {
			m_pResource = rhs.m_pResource;
			m_StartOffset = rhs.m_StartOffset;
			m_pStartAddr = rhs.m_pStartAddr;
			m_pStartGpuAddr = rhs.m_pStartGpuAddr;
			m_Size = rhs.m_Size;
		}


	private:
		ID3D12Resource	*m_pResource;
		size_t			m_StartOffset;					//!<	リソーストップからの開始位置
		void*			m_pStartAddr;					//!<	開始CPUアドレス
		D3D12_GPU_VIRTUAL_ADDRESS	m_pStartGpuAddr;	//!<	開始GPUアドレス
		size_t			m_Size;							//!<	このインスタンスが責任を持つサイズ


	};



}



#endif // !__INCLUDE_GFXGPUBUFFER_H__
