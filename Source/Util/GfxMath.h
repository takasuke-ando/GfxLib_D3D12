#ifndef __INCLUDE_GFXMATH_H__
#define	__INCLUDE_GFXMATH_H__

/***************************************************************
	@brief	算術ユーティリティ
	@par	[説明]
		算術系ユーティリティ関数群
*/


namespace GfxLib
{



	/***************************************************************
		@brief	特定の値の倍数に繰り上げを行う
		@par	[説明]
			ある値を、何かの倍数に繰り上げる
			UpperBounds( 200 , 128 ) => 256
		@param
	*/
	inline uint32_t	UpperBounds(uint32_t x, uint32_t bounds)
	{

		return (x + bounds - 1) / bounds*bounds;

	}




	/***************************************************************
	@brief	特定の値の倍数に繰り上げを行う
	@par	[説明]
	ある値を、何かの倍数に繰り上げる
	UpperBounds( 200 , 128 ) => 256
	@param
	*/
	inline uint64_t	UpperBounds(uint64_t x, uint64_t bounds)
	{

		return (x + bounds - 1) / bounds*bounds;

	}

}



#include "GfxMath.inl"


#endif //__INCLUDE_GFXMATH_H__