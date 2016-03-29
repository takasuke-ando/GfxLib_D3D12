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

		// ���ׂĂ̕`�揈���́ABegin/End�����ŌĂяo���K�v������
		bool		Begin();
		void		End();


		// ���݂̃t���[���̃R�}���h�A���P�[�^���擾 Begin/End�u���P�b�g�����ŌĂяo���K�v������
		ID3D12CommandAllocator*	GetCurrentCommandAllocator() const { return m_aCmdAllocator[m_nCurrentCmdAllocatorIndex];	 }

		IDXGIFactory4*			GetDXGIFactory() const { return m_GIFactory; }
		ID3D12CommandQueue*		GetCommandQueue() const { return m_CmdQueue; }


		// �t�F���X��}�� Fence�I�u�W�F�N�g���g�p���āAGPU�Ƃ̓������Ƃ邱�Ƃ��\
		void					InsertFence(Fence *);
		
		//	�R�}���h�L���[�Ƀt�F���X��o�^���A�V�����l��Ԃ��B1�ȏ�̒l���Ԃ邱�Ƃ͕ۏ؂����
		uint64_t				Signal( ID3D12Fence *fence );

		//	���Ƀt�F���X�ɏ������܂��\��̒l���擾����
		uint64_t				GetNextFenceValue() const { return m_uFenceValue;  }

		// �C���X�^���X���擾����
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
