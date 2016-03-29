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
		bool	InitializeRTV( uint32_t bufferCount);


		void	Finalize();


		/*
			指定インデックスのデスクリプタハンドルを取得する
		*/
		D3D12_CPU_DESCRIPTOR_HANDLE		GetCPUDescriptorHandleByIndex( uint32_t idx ) const;

	private:

		D3DPtr<ID3D12DescriptorHeap>	m_descriptorHeap;

		uint32_t		m_descriptorSize;
		uint32_t		m_maxBufferCount;
	};





}



#endif // !__INCLUDE_GFXDESCRIPTORHEAP_H__
