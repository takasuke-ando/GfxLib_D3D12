#pragma once


#include "Util/GfxVectorMath.h"

#include <string>

namespace GfxLib
{




	/***************************************************************
	@brief	中間モデルデータ
	@par	[説明]
		モデルデータの読み込み、操作などを行う

		テクスチャや頂点バッファなどのリソースは持たない

		頂点情報は単一インデックスの情報に集約し、後ほどバッファ作成など行いやすくする
		トポロジはトライアングルリストのみ
		表面は右回りが標準

	@param
	*/


	class	InterModelData
	{
	public:
		InterModelData();
		~InterModelData();



		bool	InitializeFromObjFile(const wchar_t *objfilepath , float scale = 1.f );
		void	Finalize();


		struct Vertex {

			Float3 pos;
			Float3 uv;
			Float3 norm;


			bool operator< (const Vertex& rhs) const {

				return memcmp(this, &rhs, sizeof(*this)) < 0;

			}

		};

		struct Triangle {

			uint32_t v0, v1, v2;

		};

		//
		const std::vector< Vertex >& GetVertex() const { return m_vecVertex; }

		// Mesh
		//const std::vector< Triangle >& GetTriangle() const { return m_vecTriangle; }


		/***************************************************************
			@brief
			@par	[説明]
				トータル三角形カウントを取得
			@param
		*/
		uint32_t GetTotalTriangleCount() const;


		class Material
		{
		public:
			Material(const wchar_t* name);
			~Material();


			Float3 m_DiffuseColor;
			Float3 m_SpecularColor;
			Float3 m_EmissiveColor;

			float	m_Roughness;
			std::wstring		m_DiffuseMap;

			const wchar_t* GetName() const {
				return m_strName.c_str();
			}

		private:
			std::wstring		m_strName;

		};




		/***************************************************************
			@brief	サブメッシュ情報
			@par	[説明]
				マテリアルなどの切り替わりが行われる単位
			@param
		*/
		class SubMesh
		{
		public:
			SubMesh();
			~SubMesh();

			const std::vector< Triangle >& GetTriangle() const { return m_vecTriangle; }

			// vectorを移動します
			/*
			void	MoveTriangles(std::vector< Triangle >&& vecTri)
			{
				m_vecTriangle = vecTri;
			}
			*/
			void	AddTriangle(const Triangle& t)
			{
				m_vecTriangle.push_back(t);
			}

			void	SetMaterial(int32_t idx) {
				m_nMaterial = idx;
			}

			// 未設定の時、-1
			// ただし不便なのでfallbackMaterialが設定されるので外部では気にしなくてよい
			int32_t GetMaterial() const {
				return m_nMaterial;
			}

		protected:
		private:
			// Mesh
			std::vector< Triangle > m_vecTriangle;

			int32_t			m_nMaterial;		//	マテリアルインデックス
		};

		const std::vector< SubMesh* >		&GetSubMeshes() const { return m_vecSubMesh;	}
		const std::vector< Material* >		&GetMaterials() const { return m_vecMaterial;	}

	protected:

		bool	LoadMtlFile(const wchar_t* mtlfilepath);



	private:

		std::vector< SubMesh* >		m_vecSubMesh;
		std::vector< Material* >	m_vecMaterial;

		std::vector< Vertex >	m_vecVertex;


	};












};