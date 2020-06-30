#ifndef __INCLUDE_GFXSTATEOBJECT_H__
#define __INCLUDE_GFXSTATEOBJECT_H__

#include "System/GfxDefines.h"


namespace GfxLib
{


	class StateObject
	{
	public:
		StateObject();
		~StateObject();


		bool	Initialize(const D3D12_STATE_OBJECT_DESC &desc );

		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


	private:

		D3DPtr<ID3D12StateObject>	m_d3dStateObject;


	};





}





#endif //__INCLUDE_GFXSTATEOBJECT_H__