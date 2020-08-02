#ifndef __INCLUDE_GFXVECTORMATH_INL__
#define	__INCLUDE_GFXVECTORMATH_INL__

/***************************************************************
	@brief	ベクタ算術ユーティリティ
	@par	[説明]
		ベクタ算術系ユーティリティ関数群
*/


namespace GfxLib
{



	inline const Float3& Float3::operator=(const DirectX::XMVECTOR& rhs)
	{
		x = DirectX::XMVectorGetX( rhs );
		y = DirectX::XMVectorGetY( rhs );
		z = DirectX::XMVectorGetZ( rhs );

		return *this;
	}

	inline const Float3& Float3::operator=(const DirectX::XMFLOAT3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;

		return *this;
	}


	inline DirectX::XMVECTOR	Float3::ToXMVECTOR(float w) const
	{

		return DirectX::XMVectorSet(x,y,z,w);

	}


	inline const Float2& Float2::operator=(const DirectX::XMVECTOR& rhs)
	{
		x = DirectX::XMVectorGetX(rhs);
		y = DirectX::XMVectorGetY(rhs);

		return *this;
	}

	inline const Float2& Float2::operator=(const DirectX::XMFLOAT2& rhs)
	{
		x = rhs.x;
		y = rhs.y;

		return *this;
	}


	inline DirectX::XMVECTOR	Float2::ToXMVECTOR(float z,float w) const
	{

		return DirectX::XMVectorSet(x, y, z, w);

	}

}




#endif //__INCLUDE_GFXVECTORMATH_INL__