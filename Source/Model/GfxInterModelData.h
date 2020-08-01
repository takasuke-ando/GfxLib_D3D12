#pragma once


namespace GfxLib
{




	/***************************************************************
	@brief	中間モデルデータ
	@par	[説明]
		モデルデータの読み込み、操作などを行う

		テクスチャや頂点バッファなどのリソースは持たない

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

			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 uv;
			DirectX::XMFLOAT3 norm;


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

		protected:
		private:
			// Mesh
			std::vector< Triangle > m_vecTriangle;
		};

		const std::vector< SubMesh* >	GetSubMeshes() const { return m_vecSubMesh; }


	protected:
	private:

		std::vector< SubMesh* >	m_vecSubMesh;

		std::vector< Vertex >	m_vecVertex;


	};












};