#ifndef __INCLUDE_GFXVECTORMATH_INL__
#define	__INCLUDE_GFXVECTORMATH_INL__

/***************************************************************
	@brief	ベクタ算術ユーティリティ
	@par	[説明]
		ベクタ算術系ユーティリティ関数群
*/


namespace GfxLib
{

	inline Float4::Float4(const DirectX::XMVECTOR& rhs)
	{
		x = DirectX::XMVectorGetX(rhs);
		y = DirectX::XMVectorGetY(rhs);
		z = DirectX::XMVectorGetZ(rhs);
		w = DirectX::XMVectorGetW(rhs);
	}

	inline const Float4& Float4::operator=(const DirectX::XMVECTOR& rhs)
	{
		x = DirectX::XMVectorGetX(rhs);
		y = DirectX::XMVectorGetY(rhs);
		z = DirectX::XMVectorGetZ(rhs);
		w = DirectX::XMVectorGetW(rhs);
	}

	inline const Float4& Float4::operator=(const DirectX::XMFLOAT4& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;

	}

	inline DirectX::XMVECTOR	Float4::ToXMVECTOR() const
	{

		return DirectX::XMVectorSet(x, y, z, w);

	}


	inline const Float4 Float4::operator*(const float rhs) const
	{
		return Float4(x * rhs, y * rhs, z * rhs , w* rhs);
	}


	inline const Float4& Float4::operator += (const Float4& rhs)
	{

		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;


		return *this;
	}

	inline const Float4& Float4::operator -= (const Float4& rhs)
	{

		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;


		return *this;
	}


	inline Float3::Float3(const DirectX::XMVECTOR& rhs)
	{
		x = DirectX::XMVectorGetX(rhs);
		y = DirectX::XMVectorGetY(rhs);
		z = DirectX::XMVectorGetZ(rhs);
	}

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


	inline const Float3 Float3::operator*(const float rhs) const
	{
		return Float3(x*rhs,y*rhs,z*rhs);
	}


	inline const Float3& Float3::operator += (const Float3& rhs)
	{

		x += rhs.x;
		y += rhs.y;
		z += rhs.z;


		return *this;
	}

	inline const Float3& Float3::operator -= (const Float3& rhs)
	{

		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;


		return *this;
	}

	inline Float2::Float2(const DirectX::XMVECTOR& rhs)
	{
		x = DirectX::XMVectorGetX(rhs);
		y = DirectX::XMVectorGetY(rhs);
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


	inline const Float2 Float2::operator*(const float rhs) const
	{
		return Float2(x * rhs, y * rhs);
	}


	inline const Float2& Float2::operator += (const Float2& rhs)
	{

		x += rhs.x;
		y += rhs.y;


		return *this;
	}

	inline const Float2& Float2::operator -= (const Float2& rhs)
	{

		x -= rhs.x;
		y -= rhs.y;


		return *this;
	}

}




#endif //__INCLUDE_GFXVECTORMATH_INL__