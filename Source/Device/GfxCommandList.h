#ifndef __INCLUDE_GFXCOMMANDLIST_H__
#define __INCLUDE_GFXCOMMANDLIST_H__


#include "System/GfxDefines.h"


namespace GfxLib
{
	/*
	
		�R�}���h���X�g���J�v�Z����
		����O���t�B�b�N�X�p�C�v�̂�
	
	*/
	
	class CoreSystem;

	class CommandList
	{
	public:
		CommandList();
		~CommandList();


		bool	Initialize();
		void	Finalize();

		//	�R�}���h�̏������݂��\�ȏ�Ԃɂ���B�t���[���̍ŏ��ɌĂяo��
		void	Reset();

		/***************************************************************
			@brief	���\�[�X�o���A��ݒ肷��
			@par	[����]
				�g�����W�V�������\�[�X�o���A��ݒ肷��
			@param[in]	
		*/
		void	ResourceTransitionBarrier(ID3D12Resource* resource,ResourceStates stateBefore , ResourceStates stateAfter );

		ID3D12GraphicsCommandList*	GetD3DCommandList() const {		return m_pCmdList;		}


	private:

		D3DPtr<ID3D12GraphicsCommandList>	m_pCmdList;

	};




}


#endif // !__INCLUDE_GFXCOMMANDLIST_H__
