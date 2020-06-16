#ifndef __INCLUDE_GFXACCELERATIONSTRUCTURE_H__
#define __INCLUDE_GFXACCELERATIONSTRUCTURE_H__



#include "../Resource/GfxResource.h"


namespace GfxLib
{

	class RtGeometry;

	class AccelerationStructure :
		public Resource
	{
		typedef Resource SuperClass;
	public:
		AccelerationStructure();
		~AccelerationStructure();


		/*
			@brief	
				BLASを初期化。
				リソースの確保のみを行う

				構築処理は描画タイムラインで処理を行うため
				Build()を別途呼ぶ必要がある
		
		*/
		bool	InitializeBottomLevel(const RtGeometry* geometrys,size_t numGeom);
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE );


	private:

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO	m_prebuildInfo;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS	m_asInputs;

		//Bottom level
		std::vector<const RtGeometry*>	m_vecGeometry;				//!< 参照するレイトレーシングジオメトリ
		std::vector< D3D12_RAYTRACING_GEOMETRY_DESC> m_vecGeomDesc;	//!<	CPUタイムラインでしか参照されない（はず）


	};
		





}


#endif //__INCLUDE_GFXACCELERATIONSTRUCTURE_H__
