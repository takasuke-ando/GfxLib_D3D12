#ifndef __INCLUDE_GFXCOMMANDLIST_H__
#define __INCLUDE_GFXCOMMANDLIST_H__




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
		void	Finalize();

		//	コマンドの書き込みを可能な状態にする。フレームの最初に呼び出す
		void	Reset(CoreSystem * coreSystem);



		ID3D12GraphicsCommandList*	GetD3DCommandList() const {		return m_pCmdList;		}


	private:

		D3DPtr<ID3D12GraphicsCommandList>	m_pCmdList;

	};




}


#endif // !__INCLUDE_GFXCOMMANDLIST_H__
