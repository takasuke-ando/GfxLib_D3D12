#pragma once

/***************************************************************
	@file	GfxRayTracingRenderer
	@par	[説明]

		RayTracingを使ったレンダラ
		描画全体で使うリソースの管理や
		描画フローの制御を行う

***************************************************************/


#include "Resource/GfxRootSignature.h"
#include "Shader/GfxShader.h"
#include "RayTracing/GfxStateObject.h"
#include "Resource/GfxSampler.h"



namespace GfxLib
{

	class GraphicsCommandList;
	class Texture2D;
	class RtModel;

	class TopLevelAccelerationStructure;
	class BottomLevelAccelerationStructure;

	namespace RayTracing {

		struct Viewport
		{
			float left;
			float top;
			float right;
			float bottom;

		};

		struct RayGenConstantBuffer
		{
			Viewport	viewport;
			Viewport	stencil;
			DirectX::XMMATRIX	mtxCamera;
		};

		struct ModelConstantBuffer
		{
			uint32_t	isIndex16bit;
			uint32_t	primitiveOffset;
			uint32_t	padd[2];
		};


		// 一時的
		struct SceneInfo
		{
			Viewport	vp;
			DirectX::XMMATRIX	mtxCamera;

			TopLevelAccelerationStructure* TLAS;
			BottomLevelAccelerationStructure* BLAS;

			GfxLib::RtModel			*rtModel;

			uint32_t			Width;
			uint32_t			Height;

			// 本当は2Dじゃない…
			GfxLib::Texture2D		*texSky;
			GfxLib::Texture2D		*texSkyRem;
			GfxLib::Texture2D		*texSkyIem;

		};


	};


	class RayTracingRenderer
	{
	public:
		RayTracingRenderer();
		~RayTracingRenderer();


		/***************************************************************
			@brief	初期化
			@par	[説明]
				内部リソースの初期化など行う
			@param
		*/
		bool	Initialize();
		void	Finalize(bool = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		/***************************************************************
			@brief
			@par	[説明]
			@param	outputUAV:	UAVステートになっていること
			@param	sceneInfo:	シーン情報
		*/
		void	Render(GfxLib::GraphicsCommandList& cmdList,D3D12_CPU_DESCRIPTOR_HANDLE outputUAV, const RayTracing::SceneInfo &sceneInfo);




	protected:
	private:


		void	_CreateRayTracingRootSignature();
		void	_CreateRayTracingPipelineStateObject();

		//	Ray Tracing
		GfxLib::RootSignature	m_globalRootSig;
		GfxLib::RootSignature	m_localRootSig;
		GfxLib::RootSignature	m_localRootSigMiss;
		GfxLib::RootSignature	m_localRootSigRayGen;
		GfxLib::Shader			m_rtShaderLib;
		GfxLib::Shader			m_rtShaderLibModel;
		GfxLib::StateObject		m_rtStateObject;

		GfxLib::Sampler			m_sampsLinear;

	};













}