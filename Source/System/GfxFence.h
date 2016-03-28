#ifndef __INCLUDE_GFXFENCE_H__
#define __INCLUDE_GFXFENCE_H__



/*

	フェンスをカプセル化
	GPU－CPU間で同期を取りたい場合に使用する

	GPUとの同期をとり型で2通りの使い方がある

	ポーリングモードの場合
	Sync関数は即座に制御を返す

	待機モードの場合
	Syncは同期が行われるまで、待機する
	Signal とSyncは対になって呼び出されないといけない
	このモードの場合Windowsの同期オブジェクトを使用するため、大量に作成することは推奨されない

*/



namespace GfxLib
{
	class CoreSystem;

	class Fence
	{
		friend class CoreSystem;
	public:
		Fence();
		~Fence();


		/*
			フェンスの初期化を行う

			pollingMode : falseにすると、待機オブジェクトを作成する
		*/
		bool	Initialize(CoreSystem*,bool pollingMode);
		void	Finalize();


		/*
			同期を行う
			フェンスを通過済みの場合、TRUEが返る
			ポーリングモードの場合、即時に制御が戻る
		*/
		bool	Sync();

		bool	IsPollingMode() const { return m_event == NULL; }

	private:

		void	_Insert(CoreSystem *);

		D3DPtr<ID3D12Fence>	m_fence;
		bool				m_bWaiting;
		HANDLE				m_event;		//!<	winAPI 待機イベント
		uint64_t			m_waitingFenceValue;

	};






}






#endif // !__INCLUDE_GFXFENCE_H__
