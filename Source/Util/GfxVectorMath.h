#ifndef __INCLUDE_GFXVECTORMATH_H__
#define	__INCLUDE_GFXVECTORMATH_H__

/***************************************************************
	@brief	算術ユーティリティ
	@par	[説明]
		算術系ユーティリティ関数群
*/


namespace GfxLib
{



	/***************************************************************
		@brief	Float4
		@par	[説明]
			DirectX::XMFLOAT4と似ているが
			いくつかのメソッドが追加されている
		@param
	*/
	struct Float4
	{
		float x;
		float y;
		float z;
		float w;

		Float4() = default;

		Float4(const Float4&) = default;
		Float4& operator=(const Float4&) = default;

		Float4(Float4&&) = default;
		Float4& operator=(Float4&&) = default;


		Float4(float _x, float _y, float _z,float _w) : x(_x), y(_y), z(_z),w(_w) {}
		explicit Float4(_In_reads_(4) const float* pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]) {}

		inline Float4(const DirectX::XMVECTOR& rhs);

		inline const Float4& operator=(const DirectX::XMVECTOR& rhs);
		inline const Float4& operator=(const DirectX::XMFLOAT4& rhs);

		inline DirectX::XMVECTOR	ToXMVECTOR() const;


		inline const Float4 operator*(const float rhs) const;


		inline const Float4& operator += (const Float4& rhs);
		inline const Float4& operator -= (const Float4& rhs);

	};

	/***************************************************************
		@brief	Float3
		@par	[説明]
			DirectX::XMFLOAT3が使いづらいので作成
		@param
	*/
	struct Float3
	{
		float x;
		float y;
		float z;

		Float3() = default;

		Float3(const Float3&) = default;
		Float3& operator=(const Float3&) = default;

		Float3(Float3&&) = default;
		Float3& operator=(Float3&&) = default;


		Float3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		explicit Float3(_In_reads_(3) const float* pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]) {}
		
		inline Float3(const DirectX::XMVECTOR& rhs);

		inline const Float3& operator=(const DirectX::XMVECTOR& rhs);
		inline const Float3& operator=(const DirectX::XMFLOAT3& rhs);

		inline DirectX::XMVECTOR	ToXMVECTOR(float w) const;

		inline const Float3 operator*(const float rhs) const;


		inline const Float3& operator += (const Float3& rhs);
		inline const Float3& operator -= (const Float3& rhs);


	};


	struct Float2
	{
		float x;
		float y;

		Float2() = default;

		Float2(const Float2&) = default;
		Float2& operator=(const Float2&) = default;

		Float2(Float2&&) = default;
		Float2& operator=(Float2&&) = default;

		Float2(float _x, float _y) : x(_x), y(_y){}
		explicit Float2(_In_reads_(2) const float* pArray) : x(pArray[0]), y(pArray[1]) {}

		inline Float2(const DirectX::XMVECTOR& rhs);


		inline const Float2& operator=(const DirectX::XMVECTOR& rhs);
		inline const Float2& operator=(const DirectX::XMFLOAT2& rhs);

		inline DirectX::XMVECTOR	ToXMVECTOR(float z,float w) const;


		inline const Float2 operator*(const float rhs) const;


		inline const Float2& operator += (const Float2& rhs);
		inline const Float2& operator -= (const Float2& rhs);
	};


}



#include "GfxVectorMath.inl"


#endif //__INCLUDE_GFXVECTORMATH_H__