#ifndef __INCLUDE_GFXCORESYSTEM_H__
#define	__INCLUDE_GFXCORESYSTEM_H__


#include "GfxFence.h"


namespace GfxLib
{

	class Fence;

	class CoreSystem
	{

	public:
		enum {
			MAX_FRAME_QUEUE = 3,
		};

	public:
		CoreSystem();
		~CoreSystem();




		bool	Initialize();
		void	Finalize();

		void	Update();

		ID3D12Device*			GetD3DDevice()			const { return m_pd3dDev; }
		
		float		GetFps()	const { return m_fFps; }

		// すべての描画処理は、Begin/End内部で呼び出す必要がある
		bool		Begin();
		void		End();


		// 現在のフレームのコマンドアロケータを取得 Begin/Endブラケット内部で呼び出す必要がある
		ID3D12CommandAllocator*	GetCurrentCommandAllocator() const { return m_aCmdAllocator[m_nCurrentCmdAllocatorIndex];	 }

		IDXGIFactory4*			GetDXGIFactory() const { return m_GIFactory; }
		ID3D12CommandQueue*		GetCommandQueue() const { return m_CmdQueue; }


		// フェンスを挿入 Fenceオブジェクトを使用して、GPUとの同期をとることが可能
		void					InsertFence(Fence *);
		
		//	コマンドキューにフェンスを登録し、新しい値を返す。1以上の値が返ることは保証される
		uint64_t				Signal( ID3D12Fence *fence );

		//	次にフェンスに書き込まれる予定の値を取得する
		uint64_t				GetNextFenceValue() const { return m_uFenceValue;  }

		// インスタンスを取得する
		static CoreSystem*	GetInstance() { return s_pInstance; }


	private:

		ID3D12Device*		m_pd3dDev;

		D3DPtr<ID3D12CommandQueue>		m_CmdQueue;
		D3DPtr<IDXGIFactory4>			m_GIFactory;
		D3DPtr<ID3D12CommandAllocator>	m_aCmdAllocator[MAX_FRAME_QUEUE];


		D3D_FEATURE_LEVEL	m_featureLevel;
		D3D_DRIVER_TYPE		m_driverType;


		UINT				m_nUpdateCount;
		UINT				m_nFrameCount;
		LARGE_INTEGER		m_lastFpsUpdateTime;
		float				m_fFps;

		Fence				m_aFence[MAX_FRAME_QUEUE];

		uint32_t			m_nCurrentCmdAllocatorIndex;
		uint64_t			m_uFenceValue;


		//	Singleton
		static CoreSystem*			s_pInstance;

	};





}



#endif // !__INCLUDE_GFXCORESYSTEM_H__
