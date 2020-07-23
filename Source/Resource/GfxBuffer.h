#ifndef __INCLUDE_GFXBUFFER_H__
#define	__INCLUDE_GFXBUFFER_H__


#include "GfxResource.h"


namespace GfxLib
{

	class Buffer : 
		public Resource
	{

		typedef Resource SuperClass;

	public:
		Buffer();
		~Buffer();



		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		/***************************************************************
		@brief	初期化
		@par	[説明]
			サイズを指定して、初期化を行う
			CPUアドレスを経由して書き込みを行える
		@param
		*/
		bool Initialize( size_t byteSize );


		/***************************************************************
		@brief	初期化
		@par	[説明]
			任意のステート、フラグを指定してリソースを初期化
			CPUアクセスはできない
		@param
		*/
		bool Initialize( size_t byteSize , D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES states );

		/***************************************************************
		@brief	初期化
		@par	[説明]
			サイズを指定して、初期化を行う
			GPUからしかアクセスできない状態になる
		@param
		*/
		bool InitializeImmutable(const void* pData, size_t byteSize, ResourceStates states = ResourceStates::GenericRead );





		uint32_t	GetBufferSize() const { return m_BuffSize; }

		D3D12_GPU_VIRTUAL_ADDRESS	GetGpuVirtualAddress(void)	const { return m_pGpuAddress; }
		void*						GetCpuAddress(void)			const { return m_pCpuAddress; }

	private:

		D3D12_GPU_VIRTUAL_ADDRESS	m_pGpuAddress;
		void*						m_pCpuAddress;
		uint32_t					m_BuffSize;
	};


	/***************************************************************
	@brief	ByteAddressBufferをカプセル化
	*/
	class ByteAddressBuffer : 
		public Buffer
	{
		typedef Buffer SuperClass;
	public:
		ByteAddressBuffer();
		~ByteAddressBuffer();


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);



		/***************************************************************
		@brief	初期化
		@par	[説明]
			任意のステート、フラグを指定してリソースを初期化
			CPUアクセスはできない
		@param
		*/
		bool Initialize(size_t byteSize, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES states);


		/***************************************************************
		@brief	初期化
		@par	[説明]
			サイズを指定して、初期化を行う
			GPUからしかアクセスできない状態になる
		@param
		*/
		bool InitializeImmutable(const void* pData, size_t byteSize , ResourceStates states );


		D3D12_CPU_DESCRIPTOR_HANDLE	GetSrvDescHandle() const { return m_SrvHandle; }	//!<	CPUハンドル

	protected:

		void 	_InitSRV();

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE	m_SrvHandle;


	};




	/***************************************************************
	@brief	ByteAddressBufferをカプセル化
	*/
	class StructuredBuffer :
		public Buffer
	{
		typedef Buffer SuperClass;
	public:
		StructuredBuffer();
		~StructuredBuffer();


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);



		/***************************************************************
		@brief	初期化
		@par	[説明]
			任意のステート、フラグを指定してリソースを初期化
			CPUアクセスはできない
		@param
		*/
		bool Initialize( uint32_t elementSize , uint32_t elementCount , D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES states);


		/***************************************************************
		@brief	初期化
		@par	[説明]
			サイズを指定して、初期化を行う
			GPUからしかアクセスできない状態になる
		@param
		*/
		bool InitializeImmutable(const void* pData, uint32_t elementSize, uint32_t elementCount, ResourceStates states);


		D3D12_CPU_DESCRIPTOR_HANDLE	GetSrvDescHandle() const { return m_SrvHandle; }	//!<	CPUハンドル

	protected:

		void 	_InitSRV();

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE	m_SrvHandle;

		uint32_t	m_ElementSize;
		uint32_t	m_ElementCount;


	};



}


#endif // !__INCLUDE_GFXVERTEXBUFFER_H__
