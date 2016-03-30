#ifndef __INCLUDE_GFXSWAPCHAIN_H__
#define	__INCLUDE_GFXSWAPCHAIN_H__


/*
	�X���b�v�`�F�C�����J�v�Z����
	
*/


#include "Resource/GfxDescriptorHeap.h"


namespace GfxLib
{

	class CoreSystem;
	class CommandList;

	class SwapChain
	{
	public:
		SwapChain();
		~SwapChain();

		bool	Initialize(HWND hwnd );
		void	Finalize();


		/***************************************************************
		@brief		�t���[���̊J�n
		@par	[����]
			�����_�[�^�[�Q�b�g�ւ̏������݂��s���O�ɌĂяo���܂�
			�Ăяo����Y���ƃn�U�[�h���䂪�s��ꂸ�A�\�����̃o�b�N�o�b�t�@��`��Ɏg���Ă��܂����Ƃ�����܂�
		@param[in]	cmdList:	�R�}���h���X�g
		*/
		void	Begin(CommandList& cmdList);

		/***************************************************************
		@brief		�t���[���̏I��
		@par	[����]
			�t���[���̍Ō�ɌĂяo���܂�
			�ʏ�A���̒���ɃR�}���h���X�g��Close,
			ExecuteCommandList,Present�̏��ŌĂяo���܂�
		@param[in]	cmdList:	�R�}���h���X�g
		*/
		void	End(CommandList& cmdList );

		void	Present(uint32_t SyncInterval, uint32_t Flags);


		D3D12_CPU_DESCRIPTOR_HANDLE	GetCurrentRenderTargetHandle() const;

	private:


		D3DPtr< IDXGISwapChain3 >	m_GISwapChain;
		DescriptorHeap				m_RTDescHeap;		//	Render Target View

		uint32_t	m_nCurrentBackBufferIndex;

	};

	



}





#endif //__INCLUDE_GFXSWAPCHAIN_H__
