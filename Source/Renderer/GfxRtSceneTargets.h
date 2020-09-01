#ifndef __INCLUDE_GFXRTSCENETARGETS_H__
#define __INCLUDE_GFXRTSCENETARGETS_H__



#include "Resource/GfxTexture2D.h"




namespace GfxLib
{


	class RtSceneTargets
	{

		friend class RayTracingRenderer;

	public:

		RtSceneTargets();
		~RtSceneTargets();


		struct INITIALIZE_PARAM
		{
			uint32_t	Width;
			uint32_t	Height;

		};


		bool	Initialize(const INITIALIZE_PARAM& InitParam);
		void	Finalize(bool bDelayed=GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		D3D12_CPU_DESCRIPTOR_HANDLE		GetResultSrv() const { return m_rtOutput.GetSrvDescHandle();  }


		Texture2D& GetOutputTexture() { return m_rtOutput; }


		Texture2D& GetWriteSceneHDR() { return m_SceneHDR[m_nIdx]; }
		Texture2D& GetReadSceneHDR() { return m_SceneHDR[!m_nIdx]; }


		void	SwapIdx()  { m_nIdx = !m_nIdx;  }


	protected:
	private:

		Texture2D		m_rtOutput;

		Texture2D		m_SceneHDR[2];

		uint32_t		m_nIdx;		//	現在の書き込み先

		bool			m_bFirstDraw;

		struct {

			bool enable;
			DirectX::XMMATRIX  mtxCamera;


		}m_Prev;

	};



}






#endif // !__INCLUDE_GFXRTSCENETARGETS_H__
