




#include "stdafx.h"


#include "GfxInterModelData.h"
#include "Util/GfxMath.h"



#include <fstream>
#include <map>
#include <codecvt>

using namespace GfxLib;
using namespace DirectX;



namespace
{

	float g_fMinSpecularColor = 0.01f;	//	スペキュラアルベドの最小値


	template < class T >
	class Tokenizer
	{
	public:
		Tokenizer(const T* str , T splitchar)
			:m_str(str)
			,m_splitter(splitchar)
		{

		}


		void GetNext(std::string& str) 
		{
			uint32_t count = 0;
			
			const T* tmp = m_str;
			while (*tmp != 0 && *tmp != m_splitter) {
				++count;
				++tmp;
			}

			str.assign(m_str, count);

			if (*tmp == m_splitter) ++tmp;

			m_str = tmp;
		}

void Skip()
{
	const T* tmp = m_str;
	while (*tmp != 0 && *tmp != m_splitter) {
		++tmp;
	}
	if (*tmp == m_splitter) ++tmp;

	m_str = tmp;
}

bool IsEnd() const {

	if (*m_str == 0) return true;

	return false;
}

const T* GetCurrent() const {
	return m_str;
}


	private:

		const T* m_str;
		T m_splitter;

	};


	//	ファイル名の位置を取得
	uint32_t	FindFileName(const wchar_t* filepath)
	{

		uint32_t idx = 0;
		uint32_t lastidx = 0;

		while (filepath[idx] != 0)
		{
			if (filepath[idx] == L'/' || filepath[idx] == L'\\') {
				lastidx = idx + 1;
			}


			++idx;
		}

		return lastidx;


	}



}


InterModelData::InterModelData()
{


}


InterModelData::~InterModelData()
{

	Finalize();

}



bool	InterModelData::InitializeFromObjFile(const wchar_t* objfilepath, float scale)
{
	Finalize();

	// どうやら.OBJは手前がZ？
	const bool invertZ = true;

	std::ifstream	ifs(objfilepath);

	if (ifs.fail()) {

		GFX_ASSERT(0, L"[InterModelData] Cannot open file <%s>", objfilepath)

			return false;
	}


	std::vector< Float3 >	vecNormal;
	std::vector< Float3 >	vecPosition;
	std::vector< Float3 >	vecUv;


	auto findMaterial = [&](const wchar_t* mtlname) -> int32_t {

		int32_t idx = 0;
		for (auto it : m_vecMaterial) {

			if (wcscmp(it->GetName(), mtlname) == 0) {

				return idx;

			}
			++idx;

		}

		GFX_ASSERT(0,L"Material Not found (%s)",mtlname);

		return -1;
	};
	


	SubMesh* subMesh = nullptr;

	struct VerteIndices
	{

		int32_t npos;
		int32_t nuv;
		int32_t nnorm;

		bool operator < (const VerteIndices& rhs) const {

			if (npos < rhs.npos)	return true;
			if (npos > rhs.npos)	return false;

			if (nuv < rhs.nuv)		return true;
			if (nuv > rhs.nuv)		return false;

			if (nnorm < rhs.nnorm)	return true;
			if (nnorm > rhs.nnorm)	return false;

			return false;
		}

	};
	std::map< Vertex, uint32_t >  uniqIndices;
	int32_t curline = -1;

	while (!ifs.eof()) {
		char line[256];
		ifs.getline(line, 256);
		++curline;

		std::string sline = line;




		switch (line[0]) 
		{
		case 0:
		case '\n':
		case '#':
			// Comment
			continue;
			break;
		case 'v':
		{

			if (line[1] == 'n') {

				Float3 norm = {};
				sscanf_s(line+2, "%f %f %f", &norm.x, &norm.y, &norm.z);

				if (invertZ) norm.z *= -1.f;

				vecNormal.push_back(norm);

			} else if (line[1] == ' ') {


				Float3 pos = {};
				sscanf_s(line + 2, "%f %f %f", &pos.x, &pos.y, &pos.z);

				pos.x *= scale;
				pos.y *= scale;
				pos.z *= scale;

				if (invertZ) pos.z *= -1.f;

				vecPosition.push_back(pos);

			} else if (line[1] == 't') {

				Float3 uv = {};
				sscanf_s(line + 2, "%f %f %f", &uv.x, &uv.y, &uv.z);

				vecUv.push_back(uv);

			} else {

				// ??

			}

			break;
		}
		case 'u':
			{
				Tokenizer<char>	tokenizer(line, ' ');
				std::string key;
				tokenizer.GetNext(key);

				if (key == "usemtl") {

					std::string name;
					tokenizer.GetNext(name);

					std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
					std::wstring wmtlname = converter.from_bytes(name);

					int32_t idx = findMaterial(wmtlname.c_str());


					if (subMesh) {

						subMesh->SetMaterial(idx);

					}

				}


			}
			break;
		case 'm':
			{
				Tokenizer<char>	tokenizer(line,' ');

				std::string key;
				tokenizer.GetNext(key);

				if (key == "mtllib") {

					std::string mtlname;
					tokenizer.GetNext(mtlname);


					//	Load Material
					uint32_t filenameidx = FindFileName(objfilepath);

					// .mtlのパスに変換する
					std::wstring mtlpath(objfilepath, filenameidx);
					std::wstring wmtlname;

					std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
					wmtlname = converter.from_bytes(mtlname);
					mtlpath += wmtlname;

					LoadMtlFile(mtlpath.c_str());
					

				}


			}
			break;
		case 'g':
			{
				if (subMesh) {
					m_vecSubMesh.push_back(subMesh);
				}
				subMesh = new SubMesh;
			}
			break;
		case 'f':
			{
			
				
				// 面を解釈
				if (subMesh == nullptr) {

					subMesh = new SubMesh;

				} 


				char* t = line;
				uint32_t elemcount = 0;

				/*
				while (*t != 0) {
					if (*t == ' ') {
						++elemcount;
					}
					++t;
				}

				if (elemcount < 3) {

					GFX_WARN(L"[InterModelData] Face Element Count too small");
					continue;

				}
				*/

				Tokenizer<char> tokenizer(line,' ');

				// f はスキップ
				tokenizer.Skip();



				VerteIndices vindices[4] = { {-1,-1,-1}, {-1,-1,-1} ,{-1,-1,-1}, {-1,-1,-1} };

				uint32_t vsingle[4] = {};


				// vindicesを作成
				for (uint32_t e = 0; e < 4; ++e) {

					std::string svtx;
					tokenizer.GetNext(svtx);

					Tokenizer<char> indextokenizer(svtx.c_str(), '/');

					std::string pos, uv, norm;

					indextokenizer.GetNext(pos);
					indextokenizer.GetNext(uv);
					indextokenizer.GetNext(norm);

					int32_t npos = atoi(pos.c_str());
					int32_t nuv = atoi(uv.c_str());
					int32_t nnorm = atoi(norm.c_str());


					if (npos >= 0) {
						npos = npos - 1; // 1スタートなのでずらす
					} else {
						// 負の場合、今までに宣言された頂点の逆インデックスとなる
						npos = (int32_t)vecPosition.size() + npos;
					}

					if (nuv >= 0) {
						nuv = nuv - 1;
					} else {
						nuv = (int32_t)vecUv.size() + nuv;
					}

					if (nnorm >= 0) {
						nnorm = nnorm - 1;
					} else {
						nnorm = (int32_t)vecNormal.size() + nnorm;
					}


					VerteIndices vi = { npos, nuv, nnorm };

					vindices[e] = vi;
					++elemcount;


					if (tokenizer.IsEnd()) {
						break;
					}
				}


				// 面法線
				XMVECTOR faceNormal;

				{
					Float3 v0 = (vecPosition[vindices[0].npos]);
					Float3 v1 = (vecPosition[vindices[1].npos]);
					Float3 v2 = (vecPosition[vindices[2].npos]);
				
					XMVECTOR vx0 = XMVectorSet(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, 0.f);
					XMVECTOR vx1 = XMVectorSet(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z, 0.f);

					if ( invertZ )
						faceNormal = XMVector3Cross(vx0,vx1);
					else
						faceNormal = XMVector3Cross(vx1, vx0);

					faceNormal = XMVector3Normalize(faceNormal);
				}


				// 頂点インデックスを解決

				for (uint32_t e = 0; e < elemcount; ++e) {

					VerteIndices vi = vindices[e];

					Vertex v = {

						vecPosition[vi.npos],
						vi.nuv >= 0 ? vecUv[vi.nuv] : Float3{0,0,0},
						vi.nnorm >= 0 ? vecNormal[vi.nnorm] : Float3{XMVectorGetX(faceNormal),XMVectorGetY(faceNormal),XMVectorGetZ(faceNormal)},// 

					};

					auto it = uniqIndices.find(v);

					if ( it == uniqIndices.end()) {

						// pos,uv,norm の集合で、ユニークとなるインデックスを作成する

						/*
						Vertex v = {

							vecPosition[vi.npos],
							vi.nuv >= 0 ? vecUv[vi.nuv] : Float3{0,0,0},
							vi.nnorm >= 0 ? vecNormal[vi.nnorm] : faceNormal,// 

						};
						*/

						uint32_t idx = (int32_t)m_vecVertex.size();
						m_vecVertex.push_back(v);

						vsingle[e] = idx;

						uniqIndices.insert(std::pair<Vertex,uint32_t>(v,idx));
					} else {

						vsingle[e] = (*it).second;

					}


					vindices[e] = vi;

				}


				//OBJは左回りルールなので、右回りに変換
				//Triangle t0 = { vsingle[0],vsingle[1],vsingle[2] };
				Triangle t0 = { vsingle[0],vsingle[2],vsingle[1] };
				//m_vecTriangle.push_back(t0);

				subMesh->AddTriangle(t0);

				if (elemcount > 3) {
					//Triangle t1 = { vsingle[0],vsingle[2],vsingle[3] };
					Triangle t1 = { vsingle[0],vsingle[3],vsingle[2] };
					subMesh->AddTriangle(t1);

				}



			}
			break;
		default:

			break;
		}


	}

	if (subMesh) {
		m_vecSubMesh.push_back(subMesh);
	}

	ifs.close();


	//	マテリアルがないと不便なので、-1指定しているのがあればフォールバックマテリアルを指定
	bool needFallbackMaterial = false;
	for (auto it : m_vecSubMesh) {

		if (it->GetMaterial() == -1) {
			needFallbackMaterial = true;
			break;
		}
	}

	if (needFallbackMaterial) {
		int32_t idx = (int32_t)m_vecMaterial.size();

		Material* mat = new Material(L"__fallback_material");
		m_vecMaterial.push_back(mat);

		for (auto it : m_vecSubMesh) {
			if (it->GetMaterial() == -1) {
				it->SetMaterial(idx);
			}
		}

	}



	return true;

}



bool	InterModelData::LoadMtlFile(const wchar_t* mtlfilepath)
{


	std::ifstream	ifs(mtlfilepath);

	if (ifs.fail()) {

		GFX_ASSERT(0, L"[InterModelData] Cannot open mtl file <%s>", mtlfilepath)

		return false;
	}


	int32_t curline = -1;

	Material* material = nullptr;

	while (!ifs.eof()) {
		char _line[256];
		ifs.getline(_line, 256);
		++curline;


		char* line = _line;

		// 先頭のタブやスペースをスキップ
		while (*line==' '||*line=='\t') {
			++line;
		}



		std::string sline = line;

		Tokenizer<char>  tokenier(line, ' ');

		std::string key;
		tokenier.GetNext(key);


		if (key == "#") {
			continue;
		}else if (key == "newmtl") {

			// マテリアルを定義


			std::string mtlname;
			tokenier.GetNext(mtlname);

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wmtlname = converter.from_bytes(mtlname);

			material = new Material(wmtlname.c_str());
			m_vecMaterial.push_back(material);


		} else if (key == "Ns") {

			std::string value;
			tokenier.GetNext(value);

			float Ns = (float)atof( value.c_str() );

			// 1000くらいでroughness=0になる様に
			float roughness = 1.f - logf(Ns + 1.f) / logf(2.f) * 0.1f;

			roughness = Clamp<float>(roughness, 0.f, 1.f);

			material->m_Roughness = roughness;

		} else if (key == "Kd") {

			Float3 diffcolor = {1.f,1.f,1.f};

			sscanf_s(tokenier.GetCurrent(), "%f %f %f", &diffcolor.x, &diffcolor.y, &diffcolor.z);

			material->m_DiffuseColor = diffcolor;
		} else if (key == "Ks") {

			Float3 speccolor = {};

			sscanf_s(tokenier.GetCurrent(), "%f %f %f", &speccolor.x, &speccolor.y, &speccolor.z);

			speccolor.x = Max<float>(speccolor.x, g_fMinSpecularColor);
			speccolor.y = Max<float>(speccolor.y, g_fMinSpecularColor);
			speccolor.z = Max<float>(speccolor.z, g_fMinSpecularColor);


			material->m_SpecularColor = speccolor;

		} else if (key == "map_Kd") {

			std::string name;
			tokenier.GetNext(name);

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wname = converter.from_bytes(name);

			material->m_DiffuseMap = wname;

		}


		int i = 0;
		i = 0;

	}




	return true;

}



/***************************************************************
	@brief
	@par	[説明]
		トータル三角形カウントを取得
	@param
*/
uint32_t InterModelData::GetTotalTriangleCount() const
{
	uint32_t count = 0;
	for (auto it : m_vecSubMesh) {
		count += (uint32_t)it->GetTriangle().size();
	}

	return count;
}



void	InterModelData::Finalize()
{

	m_vecVertex.clear();

	for (auto it : m_vecSubMesh) {
		delete it;
	}
	for (auto it : m_vecMaterial) {
		delete it;
	}
	m_vecSubMesh.clear();
	m_vecMaterial.clear();
}


InterModelData::Material::Material(const wchar_t* name)
{
	m_strName = name;
	m_DiffuseColor = Float3(1.f, 1.f, 1.f);
	m_SpecularColor = Float3(0.04f, 0.04f, 0.04f);
	m_Roughness = 0.5f;
}


InterModelData::Material::~Material()
{


}


InterModelData::SubMesh::SubMesh()
{

	m_nMaterial = -1;

}

InterModelData::SubMesh::~SubMesh()
{
	m_vecTriangle.clear();

}