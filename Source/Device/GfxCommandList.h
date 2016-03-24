#ifndef __INCLUDE_GFXCOMMANDLIST_H__
#define __INCLUDE_GFXCOMMANDLIST_H__


#include "GfxLibStdAfx.h"


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


		bool	Initialize( CoreSystem * coreSystem );

		void	Reset(CoreSystem * coreSystem);


	private:

		D3DPtr<ID3D12GraphicsCommandList>	m_pCmdList;

	};




}


#endif // !__INCLUDE_GFXCOMMANDLIST_H__
