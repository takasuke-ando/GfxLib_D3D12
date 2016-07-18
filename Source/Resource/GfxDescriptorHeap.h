#ifndef __INCLUDE_GFXDESCRIPTORHEAP_H__
#define	__INCLUDE_GFXDESCRIPTORHEAP_H__





namespace GfxLib
{
	class  CoreSystem;

	class DescriptorHeap
	{
	public:
		DescriptorHeap();
		~DescriptorHeap();


		// RTV
		bool	InitializeRTV(uint32_t bufferCount);

		// DSV
		bool	InitializeDSV( uint32_t bufferCount);

		//	CBV SRV UAV
		bool	InitializeCBV_SRV_UAV(uint32_t bufferCount);

		//	Sampler
		bool	InitializeSampler(uint32_t bufferCount);

		void	Finalize(bool delayedDelete = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);



		bool	IsValid() const { return m_descriptorHeap != nullptr; }

		/*
			指定インデックスのデスクリプタハンドルを取得する
		*/
		D3D12_CPU_DESCRIPTOR_HANDLE		GetCPUDescriptorHandleByIndex(uint32_t idx) const;
		D3D12_GPU_DESCRIPTOR_HANDLE		GetGPUDescriptorHandleByIndex( uint32_t idx ) const;


		ID3D12DescriptorHeap*	GetD3DDescriptorHeap() const { return	m_descriptorHeap; }


		/*
			バッファ数
		*/
		uint32_t				GetBufferCount() const {
			return m_maxBufferCount;
		}


		D3D12_DESCRIPTOR_HEAP_TYPE		GetHeapType() const {
			return m_heapType;
		}



	private:

		D3DPtr<ID3D12DescriptorHeap>	m_descriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE		m_heapType;

		uint32_t		m_descriptorSize;
		uint32_t		m_maxBufferCount;
	};





}



#endif // !__INCLUDE_GFXDESCRIPTORHEAP_H__
