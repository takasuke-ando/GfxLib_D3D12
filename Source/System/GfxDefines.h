﻿#ifndef __INCLUDE_GFXDEFINES_H__
#define	__INCLUDE_GFXDEFINES_H__


/***************************************************************
	@brief	各種定義など
	@par	[説明]
		各種構造体やenum などをここに定義する
/***************************************************************/



namespace GfxLib
{


	// 定数
	enum {
		MAX_FRAME_QUEUE = 3,
	};



	/*
		リソースバリアタイプ
	*/
	enum class BarrierType : uint32_t {

		Transition = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		Aliasing = D3D12_RESOURCE_BARRIER_TYPE_ALIASING,
		Uav = D3D12_RESOURCE_BARRIER_TYPE_UAV,

	};


	/*
		リソースステート
		D3D12_RESOURCE_STATESにキャスト可能
	*/
	enum class ResourceStates : uint32_t {

		/*
		      D3D12_RESOURCE_STATE_COMMON	= 0,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER	= 0x1,
        D3D12_RESOURCE_STATE_INDEX_BUFFER	= 0x2,
        D3D12_RESOURCE_STATE_RENDER_TARGET	= 0x4,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS	= 0x8,
        D3D12_RESOURCE_STATE_DEPTH_WRITE	= 0x10,
        D3D12_RESOURCE_STATE_DEPTH_READ	= 0x20,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE	= 0x40,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	= 0x80,
        D3D12_RESOURCE_STATE_STREAM_OUT	= 0x100,
        D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT	= 0x200,
        D3D12_RESOURCE_STATE_COPY_DEST	= 0x400,
        D3D12_RESOURCE_STATE_COPY_SOURCE	= 0x800,
        D3D12_RESOURCE_STATE_RESOLVE_DEST	= 0x1000,
        D3D12_RESOURCE_STATE_RESOLVE_SOURCE	= 0x2000,
        D3D12_RESOURCE_STATE_GENERIC_READ	= ( ( ( ( ( 0x1 | 0x2 )  | 0x40 )  | 0x80 )  | 0x200 )  | 0x800 ) ,
        D3D12_RESOURCE_STATE_PRESENT	= 0,
        D3D12_RESOURCE_STATE_PREDICATION	= 0x200
		
		*/

		CopyDest = D3D12_RESOURCE_STATE_COPY_DEST,
		GenericRead = D3D12_RESOURCE_STATE_GENERIC_READ,
		Present = D3D12_RESOURCE_STATE_PRESENT,
		RenderTarget = D3D12_RESOURCE_STATE_RENDER_TARGET,


	};


	/*
		Descriptor Range Type 
		D3D12_DESCRIPTOR_RANGE_TYPE にキャスト可能
		DescriptorTableに接続される複数のRange情報のうちの一つ
	*/
	enum class DescriptorRangeType : uint32_t {

		Srv = D3D12_DESCRIPTOR_RANGE_TYPE_SRV ,
		Uav = D3D12_DESCRIPTOR_RANGE_TYPE_UAV ,
		Cbv = D3D12_DESCRIPTOR_RANGE_TYPE_CBV ,
		Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,

	};



	/*
		
		D3D12_SHADER_VISIBILITYにキャスト可能
	
	*/

	enum class ShaderVisibility : uint32_t {
		All = D3D12_SHADER_VISIBILITY_ALL ,
		Vertex = D3D12_SHADER_VISIBILITY_VERTEX ,
		Hull = D3D12_SHADER_VISIBILITY_HULL ,
		Domain = D3D12_SHADER_VISIBILITY_DOMAIN ,
		Geometry = D3D12_SHADER_VISIBILITY_GEOMETRY ,
		Pixel  =D3D12_SHADER_VISIBILITY_PIXEL ,

	};



	/*
	D3D12_DESCRIPTOR_HEAP_TYPEにキャスト可能
	*/
	enum class DescriptorHeapType : uint32_t {
		CBV_SRV_UAV = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		SAMPLER = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		RTV = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		DSV = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,

		NUM_TYPES = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES,
	};


	/*

		DXGI_FORMAT にキャスト可能

	*/
	enum class Format : uint32_t {

		UNKNOWN					= DXGI_FORMAT_UNKNOWN,
		R32G32B32A32_TYPELESS	= DXGI_FORMAT_R32G32B32A32_TYPELESS,
		R32G32B32A32_FLOAT		= DXGI_FORMAT_R32G32B32A32_FLOAT,
		R32G32B32A32_UINT		= DXGI_FORMAT_R32G32B32A32_UINT,
		R32G32B32A32_SINT		= DXGI_FORMAT_R32G32B32A32_SINT,
		R32G32B32_TYPELESS		= DXGI_FORMAT_R32G32B32_TYPELESS,
		R32G32B32_FLOAT			= DXGI_FORMAT_R32G32B32_FLOAT,
		R32G32B32_UINT			= DXGI_FORMAT_R32G32B32_UINT,
		R32G32B32_SINT			= DXGI_FORMAT_R32G32B32_SINT,
		R16G16B16A16_TYPELESS	= DXGI_FORMAT_R16G16B16A16_TYPELESS,
		R16G16B16A16_FLOAT		= DXGI_FORMAT_R16G16B16A16_FLOAT,
		R16G16B16A16_UNORM		= DXGI_FORMAT_R16G16B16A16_UNORM,
		R16G16B16A16_UINT		= DXGI_FORMAT_R16G16B16A16_UINT,
		R16G16B16A16_SNORM		= DXGI_FORMAT_R16G16B16A16_SNORM,
		R16G16B16A16_SINT		= DXGI_FORMAT_R16G16B16A16_SINT,
		R32G32_TYPELESS			= DXGI_FORMAT_R32G32_TYPELESS,
		R32G32_FLOAT			= DXGI_FORMAT_R32G32_FLOAT,
		R32G32_UINT				= DXGI_FORMAT_R32G32_UINT,
		R32G32_SINT = DXGI_FORMAT_R32G32_SINT,
		R32G8X24_TYPELESS = DXGI_FORMAT_R32G8X24_TYPELESS,
		D32_FLOAT_S8X24_UINT = DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
		R32_FLOAT_X8X24_TYPELESS = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
		X32_TYPELESS_G8X24_UINT = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
		R10G10B10A2_TYPELESS = DXGI_FORMAT_R10G10B10A2_TYPELESS,
		R10G10B10A2_UNORM = DXGI_FORMAT_R10G10B10A2_UNORM,
		R10G10B10A2_UINT = DXGI_FORMAT_R10G10B10A2_UINT,
		R11G11B10_FLOAT = DXGI_FORMAT_R11G11B10_FLOAT,
		R8G8B8A8_TYPELESS = DXGI_FORMAT_R8G8B8A8_TYPELESS,
		R8G8B8A8_UNORM = DXGI_FORMAT_R8G8B8A8_UNORM,
		R8G8B8A8_UNORM_SRGB = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		R8G8B8A8_UINT = DXGI_FORMAT_R8G8B8A8_UINT,
		R8G8B8A8_SNORM = DXGI_FORMAT_R8G8B8A8_SNORM,
		R8G8B8A8_SINT = DXGI_FORMAT_R8G8B8A8_SINT,
		R16G16_TYPELESS = DXGI_FORMAT_R16G16_TYPELESS,
		R16G16_FLOAT = DXGI_FORMAT_R16G16_FLOAT,
		R16G16_UNORM = DXGI_FORMAT_R16G16_UNORM,
		R16G16_UINT = DXGI_FORMAT_R16G16_UINT,
		R16G16_SNORM = DXGI_FORMAT_R16G16_SNORM,
		R16G16_SINT = DXGI_FORMAT_R16G16_SINT,
		R32_TYPELESS = DXGI_FORMAT_R32_TYPELESS,
		D32_FLOAT = DXGI_FORMAT_D32_FLOAT,
		R32_FLOAT = DXGI_FORMAT_R32_FLOAT,
		R32_UINT = DXGI_FORMAT_R32_UINT,
		R32_SINT = DXGI_FORMAT_R32_SINT,
		R24G8_TYPELESS = DXGI_FORMAT_R24G8_TYPELESS,
		D24_UNORM_S8_UINT = DXGI_FORMAT_D24_UNORM_S8_UINT,
		R24_UNORM_X8_TYPELESS = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		X24_TYPELESS_G8_UINT = DXGI_FORMAT_X24_TYPELESS_G8_UINT,
		R8G8_TYPELESS = DXGI_FORMAT_R8G8_TYPELESS,
		R8G8_UNORM = DXGI_FORMAT_R8G8_UNORM,
		R8G8_UINT = DXGI_FORMAT_R8G8_UINT,
		R8G8_SNORM = DXGI_FORMAT_R8G8_SNORM,
		R8G8_SINT = DXGI_FORMAT_R8G8_SINT,
		R16_TYPELESS = DXGI_FORMAT_R16_TYPELESS,
		R16_FLOAT = DXGI_FORMAT_R16_FLOAT,
		D16_UNORM = DXGI_FORMAT_D16_UNORM,
		R16_UNORM = DXGI_FORMAT_R16_UNORM,
		R16_UINT = DXGI_FORMAT_R16_UINT,
		R16_SNORM = DXGI_FORMAT_R16_SNORM,
		R16_SINT = DXGI_FORMAT_R16_SINT,
		R8_TYPELESS = DXGI_FORMAT_R8_TYPELESS,
		R8_UNORM = DXGI_FORMAT_R8_UNORM,
		R8_UINT = DXGI_FORMAT_R8_UINT,
		R8_SNORM = DXGI_FORMAT_R8_SNORM,
		R8_SINT = DXGI_FORMAT_R8_SINT,
		A8_UNORM = DXGI_FORMAT_A8_UNORM,
		R1_UNORM = DXGI_FORMAT_R1_UNORM,
		R9G9B9E5_SHAREDEXP = DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
		R8G8_B8G8_UNORM = DXGI_FORMAT_R8G8_B8G8_UNORM,
		G8R8_G8B8_UNORM = DXGI_FORMAT_G8R8_G8B8_UNORM,
		BC1_TYPELESS = DXGI_FORMAT_BC1_TYPELESS,
		BC1_UNORM = DXGI_FORMAT_BC1_UNORM,
		BC1_UNORM_SRGB = DXGI_FORMAT_BC1_UNORM_SRGB,
		BC2_TYPELESS = DXGI_FORMAT_BC2_TYPELESS,
		BC2_UNORM = DXGI_FORMAT_BC2_UNORM,
		BC2_UNORM_SRGB = DXGI_FORMAT_BC2_UNORM_SRGB,
		BC3_TYPELESS = DXGI_FORMAT_BC3_TYPELESS,
		BC3_UNORM = DXGI_FORMAT_BC3_UNORM,
		BC3_UNORM_SRGB = DXGI_FORMAT_BC3_UNORM_SRGB,
		BC4_TYPELESS = DXGI_FORMAT_BC4_TYPELESS,
		BC4_UNORM = DXGI_FORMAT_BC4_UNORM,
		BC4_SNORM = DXGI_FORMAT_BC4_SNORM,
		BC5_TYPELESS = DXGI_FORMAT_BC5_TYPELESS,
		BC5_UNORM = DXGI_FORMAT_BC5_UNORM,
		BC5_SNORM = DXGI_FORMAT_BC5_SNORM,
		B5G6R5_UNORM = DXGI_FORMAT_B5G6R5_UNORM,
		B5G5R5A1_UNORM = DXGI_FORMAT_B5G5R5A1_UNORM,
		B8G8R8A8_UNORM = DXGI_FORMAT_B8G8R8A8_UNORM,
		B8G8R8X8_UNORM = DXGI_FORMAT_B8G8R8X8_UNORM,
		R10G10B10_XR_BIAS_A2_UNORM = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
		B8G8R8A8_TYPELESS = DXGI_FORMAT_B8G8R8A8_TYPELESS,
		B8G8R8A8_UNORM_SRGB = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		B8G8R8X8_TYPELESS = DXGI_FORMAT_B8G8R8X8_TYPELESS,
		B8G8R8X8_UNORM_SRGB = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
		BC6H_TYPELESS = DXGI_FORMAT_BC6H_TYPELESS,
		BC6H_UF16 = DXGI_FORMAT_BC6H_UF16,
		BC6H_SF16 = DXGI_FORMAT_BC6H_SF16,
		BC7_TYPELESS = DXGI_FORMAT_BC7_TYPELESS,
		BC7_UNORM = DXGI_FORMAT_BC7_UNORM,
		BC7_UNORM_SRGB = DXGI_FORMAT_BC7_UNORM_SRGB,
		AYUV		= DXGI_FORMAT_AYUV,
		Y410		= DXGI_FORMAT_Y410,
		Y416		= DXGI_FORMAT_Y416,
		NV12		= DXGI_FORMAT_NV12,
		P010		= DXGI_FORMAT_P010,
		P016		= DXGI_FORMAT_P016,
		_420_OPAQUE = DXGI_FORMAT_420_OPAQUE,
		YUY2		= DXGI_FORMAT_YUY2,
		Y210		= DXGI_FORMAT_Y210,
		Y216		= DXGI_FORMAT_Y216,
		NV11		= DXGI_FORMAT_NV11,
		AI44		= DXGI_FORMAT_AI44,
		IA44		= DXGI_FORMAT_IA44,
		P8			= DXGI_FORMAT_P8,
		A8P8		= DXGI_FORMAT_A8P8,
		B4G4R4A4_UNORM = DXGI_FORMAT_B4G4R4A4_UNORM,
		P208		= DXGI_FORMAT_P208,
		V208		= DXGI_FORMAT_V208,
		V408		= DXGI_FORMAT_V408,


	};


	/*
	
		D3D12_DESCRIPTOR_RANGE 構造体のクローン
		
	
	*/
	struct DESCRIPTOR_RANGE {
		DescriptorRangeType	RangeType;
		uint32_t NumDescriptors;		//	デスクリプタ数
		uint32_t BaseShaderRegister;	//	
		//UINT RegisterSpace;			//	シェーダでは→register(t3,space5)のような書き方でデスクリプタ配列を定義できるみたい
		//UINT OffsetInDescriptorsFromTableStart;	// ?

	};


	typedef		uint32_t	HashValueType;


	/*
	
		ID3D12GraphicsPipelineState のキャッシュ識別子
		このキャッシュIDにより、PSOは一意に識別できる

	*/
	struct GRAPHICS_PSO_CACHE_IDENTIFIER
	{

		/*
		    ID3D12RootSignature *pRootSignature;
			D3D12_SHADER_BYTECODE VS;
			D3D12_SHADER_BYTECODE PS;
			D3D12_SHADER_BYTECODE DS;
			D3D12_SHADER_BYTECODE HS;
			D3D12_SHADER_BYTECODE GS;
			D3D12_STREAM_OUTPUT_DESC StreamOutput;
			D3D12_BLEND_DESC BlendState;
			UINT SampleMask;
			D3D12_RASTERIZER_DESC RasterizerState;
			D3D12_DEPTH_STENCIL_DESC DepthStencilState;
			D3D12_INPUT_LAYOUT_DESC InputLayout;
			D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
			D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
			UINT NumRenderTargets;
			DXGI_FORMAT RTVFormats[ 8 ];
			DXGI_FORMAT DSVFormat;
			DXGI_SAMPLE_DESC SampleDesc;
			UINT NodeMask;
			D3D12_CACHED_PIPELINE_STATE CachedPSO;
			D3D12_PIPELINE_STATE_FLAGS Flags;
		
		*/

		HashValueType	hashRootSignature;
		HashValueType	hashVS;
		HashValueType	hashPS;
		HashValueType	hashDS;
		HashValueType	hashHS;
		HashValueType	hashGS;

		HashValueType	hashBlendState;
		HashValueType	hashRasterizerState;
		HashValueType	hashDepthStencilState;
		HashValueType	hashInputLayout;
		D3D12_INDEX_BUFFER_STRIP_CUT_VALUE	IBStripCutValue;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE		PrimitiveTopologyType;

		UINT			NumRenderTargets;
		DXGI_FORMAT		RTVFormats[8];
		DXGI_FORMAT		DSVFormat;
		DXGI_SAMPLE_DESC SampleDesc;
		//UINT NodeMask;
		//D3D12_CACHED_PIPELINE_STATE CachedPSO;
		D3D12_PIPELINE_STATE_FLAGS Flags;



		GRAPHICS_PSO_CACHE_IDENTIFIER() {

			memset(this, 0, sizeof(*this));

		}


	};



}





#endif // !__INCLUDE_GFXDEFINES_H__
