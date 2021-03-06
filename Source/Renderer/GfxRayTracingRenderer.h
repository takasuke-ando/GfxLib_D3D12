﻿#pragma once

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
#include "Resource/GfxTexture2D.h"
#include "Util/GfxVectorMath.h"



namespace GfxLib
{

	class GraphicsCommandList;
	class Texture2D;
	class RtModel;

	class TopLevelAccelerationStructure;
	class BottomLevelAccelerationStructure;

	class RtSceneTargets;

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
			DirectX::XMFLOAT3X4	mtxCurToPrevView;
			float				globalTime;
			float				sceneRandom;	//0-1
			uint32_t			padd1;
			uint32_t			padd2;
		};

		struct ModelConstantBuffer
		{
			uint32_t	isIndex16bit;
			uint32_t	primitiveOffset;
			uint32_t	padd[2];
			Float3		diffuseAlbedo;
			float		roughness;
			Float3		specularAlbedo;	//	Specular Albedo
			uint32_t	padd2;
			Float3		emissive;
			uint32_t	padd3;
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

			double				globalTime;	//	アプリケーションが起動してからの時間

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
		void	Render(GfxLib::GraphicsCommandList& cmdList, RtSceneTargets &rtTargets, const RayTracing::SceneInfo &sceneInfo);



		enum DUMMY_TEX_NUM {
			DUMMY_TEX_WHITE,

			DUMMY_TEX_NUM,
		};



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
		GfxLib::Shader			m_rtShaderLibMiss;
		GfxLib::StateObject		m_rtStateObject;

		GfxLib::Sampler			m_sampsLinear;


		GfxLib::Texture2D		m_texDummy[DUMMY_TEX_NUM];

	};













}