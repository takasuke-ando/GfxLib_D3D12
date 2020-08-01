#pragma once



#include <vector>


#include "GfxRtGeometry.h"
#include "Resource/GfxBuffer.h"

namespace GfxLib
{

	class InterModelData;

	/*
		@brief
			RayTracing用のモデル構造

			InterModelDataから生成することができる

			このモデル情報を参照して、
			複数のインスタンスを生成することができる

	*/


	class RtModel
	{


	public:
		RtModel();
		~RtModel();

		/*
			@brief	初期化
			@par[説明]
				初期化後にはモデルデータは破棄しても構わない
		
		*/
		bool	Initialize(const InterModelData &data);
		void	Finalize(bool delayed=GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		// 現状、頂点フォーマットは固定

		// 頂点ポジション
		struct RtVertex { float v1, v2, v3; };

		// 頂点属性
		// TODO:パックしてサイズ削減
		struct RtAttrib {

			DirectX::XMFLOAT3	Normal;
			DirectX::XMFLOAT3	BaseColor;
			DirectX::XMFLOAT2	Uv;
			// Tangent / Binormal
		};



		const std::vector< D3D12_RAYTRACING_GEOMETRY_DESC >& GetGeomDescs() const { return m_vecGeomDesc; }



		class Material
		{
		public:
			Material()  {}
			~Material() {}

		protected:
		};


		/*
			マテリアルが割り当てられる単位となるサブグループ
			Indexは、モデル全体のインデックス

			サブグループ内のインデックスに変換したら、インデックスバッファサイズの節約になる可能性がある
		*/
		class SubGroup
		{
		public:
			SubGroup(uint32_t startIndex, uint32_t idx_count):m_startIndex(startIndex),m_indexCount(idx_count) {}
			~SubGroup(){}

			uint32_t GetStartIndex() const {
				return m_startIndex;
			}
			uint32_t GetIndexCount() const {
				return m_indexCount;
			}

		protected:
		private:

			uint32_t m_startIndex;
			uint32_t m_indexCount;

		};


		const std::vector< SubGroup* >	GetSubGroups() const { return m_vecSubGroup; }

		D3D12_CPU_DESCRIPTOR_HANDLE		GetIndexBufferSRV() const { return m_rtGeometry.GetIndexBufferSRV(); }

		D3D12_CPU_DESCRIPTOR_HANDLE		GetGeomAttributeSRV() const {	return m_rtGeomAttrib.GetSrvDescHandle();}


		Format	GetIndexFormat() const { return m_rtGeometry.GetIndexFormat(); }

	private:

		std::vector< SubGroup* >	m_vecSubGroup;
		std::vector< Material* >	m_vecMaterial;
		std::vector< D3D12_RAYTRACING_GEOMETRY_DESC >	m_vecGeomDesc;


		GfxLib::RtGeometry			m_rtGeometry;		//	Position & Index
		GfxLib::StructuredBuffer	m_rtGeomAttrib;		//	Other Attribute ( Color , Normal , etc...)

	};









}