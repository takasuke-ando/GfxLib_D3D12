#ifndef __INCLUDE_GFXCORESYSTEM_H__
#define	__INCLUDE_GFXCORESYSTEM_H__



#include "GfxLibStdAfx.h"


namespace GfxLib
{


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


		bool		Begin();
		void		End();


		ID3D12CommandAllocator*	GetCurrentCommandAllocator() const { return m_aCmdAllocator[m_nCurrentCmdAllocatorIndex];	 }

		IDXGIFactory4*			GetDXGIFactory() const { return m_GIFactory; }
		ID3D12CommandQueue*		GetCommandQueue() const { return m_CmdQueue; }
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

		uint32_t			m_nCurrentCmdAllocatorIndex;


	};





}



#endif // !__INCLUDE_GFXCORESYSTEM_H__
