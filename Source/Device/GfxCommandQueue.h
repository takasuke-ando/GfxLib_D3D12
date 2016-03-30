#ifndef __INCLUDE_GFXCOMMANDQUEUE_H__
#define __INCLUDE_GFXCOMMANDQUEUE_H__



namespace GfxLib
{


	class CommandQueue
	{
	public:
		CommandQueue();
		~CommandQueue();


		bool Initialize();
		void Finalize();


		ID3D12CommandQueue*	GetD3DCommandQueue() const { return m_CmdQueue;  }


	private:
		D3DPtr<ID3D12CommandQueue>		m_CmdQueue;


	};






}










#endif // !__INCLUDE_GFXCOMMANDQUEUE_H__
