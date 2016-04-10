#ifndef __INCLUDE_GFXDELAYDELETE_H__
#define __INCLUDE_GFXDELAYDELETE_H__

/***************************************************************
	@brief	遅延開放
	@par	[説明]
		遅延開放
		D3Dオブジェクトなど、GPUから参照されている可能性がある
		メモリ上のオブジェクトを、安全なフレーム数遅延させた上で
		開放する

***************************************************************/



#include <vector>

namespace GfxLib
{

	class DelayDelete
	{
	public:
		DelayDelete();
		~DelayDelete();


		void	Update();

		// 登録したすべてのオブジェクトを即時解放する。通常、終了処理時に呼び出される
		void	DeleteAll();


		/***************************************************************
		@brief	IUnknownの登録
		@par	[説明]
			IUnknownの登録
			内部でAddRefされるため、呼び出し元では
			この関数直後にReleaseして構わない
		@param[in]	object:	遅延開放リストに登録するオブジェクト
		*/
		void	Regist(IUnknown* object);


	private:

		enum {

			MAX_FRAME_COUNT = 3,

		};


		struct FRAME_INFO
		{
			std::vector< IUnknown * >	vecIUnknown;
		};

		FRAME_INFO	m_aFrameInfo[MAX_FRAME_COUNT];

		uint32_t	m_nCurrentIndex;

	};



}



#endif // !__INCLUDE_GFXDELAYDELETE_H__
