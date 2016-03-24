#ifndef __INCLUDE_GFXCOMMANDLIST_H__
#define __INCLUDE_GFXCOMMANDLIST_H__


#include "GfxLibStdAfx.h"


namespace GfxLib
{
	/*
	
		コマンドリストをカプセル化
		現状グラフィックスパイプのみ
	
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
