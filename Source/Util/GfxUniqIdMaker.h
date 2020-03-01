#ifndef __INCLUDE_GFXUNIQIDMAKER_H__
#define __INCLUDE_GFXUNIQIDMAKER_H__



/***************************************************************
	@file	GfxUniqIdMaker
	@par	[説明]
		ユニークID生成
		単に最初から登録されたデータに対して
		ユニークに連番IDを振っていく
***************************************************************/


#include <vector>
#include <algorithm>


namespace GfxLib
{

	template<class T>
	class  UniqIdMaker
	{

	public:

		UniqIdMaker();
		~UniqIdMaker();


		/***************************************************************
		@brief	ユニークIDを生成
		@par	[説明]
			ユニークIDを生成
			単に最初に登録されたものから、0から順番に振っていく
			登録個数が増えるとO(N)で検索時間が増えていく
		@param
		*/
		uint32_t	MakeId(const T& data);
		uint32_t	MakeId_memcmp(const T& data);	//	operator == が無い場合用

		template<class Pred>
		uint32_t	MakeId(Pred pred);


	private:
		typedef std::vector<T>	DataVec;
		DataVec	m_vecData;


	};


	template<class T>
	UniqIdMaker<T>::UniqIdMaker()
	{

	}

	template<class T>
	UniqIdMaker<T>::~UniqIdMaker()
	{

	}

	template<class T>
	uint32_t UniqIdMaker<T>::MakeId(const T& data)
	{

		DataVec::iterator it = std::find(m_vecData.begin(),m_vecData.end(),data);

		if (it == m_vecData.end()) {

			m_vecData.push_back(data);
			return uint32_t(m_vecData.size()) - 1;

		} else {
			return uint32_t( it - m_vecData.begin() );
		}


	}

	template<class T>
	uint32_t UniqIdMaker<T>::MakeId_memcmp(const T& data)
	{

		DataVec::iterator it = std::find_if(m_vecData.begin(), m_vecData.end(), [&data](const T& x) { return memcmp(&x, &data, sizeof(T)) == 0; });

		if (it == m_vecData.end()) {

			m_vecData.push_back(data);
			return uint32_t(m_vecData.size()) - 1;

		} else {
			return uint32_t(it - m_vecData.begin());
		}


	}

	template<class T>
	template<class Pred>
	uint32_t UniqIdMaker<T>::MakeId(Pred pred)
	{

		DataVec::iterator it = std::find_if(m_vecData.begin(), m_vecData.end(), pred);

		if (it == m_vecData.end()) {

			m_vecData.push_back(data);
			return uint32_t(m_vecData.size()) - 1;

		} else {
			return uint32_t(it - m_vecData.begin());
		}


	}


}






#endif __INCLUDE_GFXUNIQIDMAKER_H__















