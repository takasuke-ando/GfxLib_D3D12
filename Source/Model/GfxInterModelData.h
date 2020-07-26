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



		bool	InitializeFromObjFile(const wchar_t *objfilepath );
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
		const std::vector< Triangle >& GetTriangle() const { return m_vecTriangle; }

	protected:
	private:

		std::vector< Vertex >	m_vecVertex;

		// Mesh
		std::vector< Triangle > m_vecTriangle;

	};












};