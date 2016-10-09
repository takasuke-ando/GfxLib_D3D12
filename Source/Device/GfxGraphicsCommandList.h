#ifndef __INCLUDE_GRAPHCIS_COMMAND_LIST_H__
#define __INCLUDE_GRAPHCIS_COMMAND_LIST_H__

/***************************************************************
	@file	GfxGraphicsCommandList.h
	@brief
	@par	[説明]

		CommandListクラスの派生クラス
		ID3D12GraphicsCommandListをカプセル化する


***************************************************************/


#include "Device/GfxCommandList.h"


namespace GfxLib
{
	class CommandQueue;

	class GraphicsCommandList : 
		public CommandList
	{

		typedef CommandList SuperClass;


	public:
		GraphicsCommandList();
		~GraphicsCommandList();


		bool	Initialize(CommandQueue *cmdQueue);
		void	Finalize();

		ID3D12GraphicsCommandList*	GetD3DCommandList() const { return m_pCmdList; }

	private:
		D3DPtr<ID3D12GraphicsCommandList>	m_pCmdList;



	};








}




#endif // !__INCLUDE_GRAPHCIS_COMMAND_LIST_H__
