#ifndef ___INCLUDE_GFXLIB_D3D12_H__
#define	___INCLUDE_GFXLIB_D3D12_H__


/*

	外部用インクルード
	プリコンパイルヘッダに入れるのを推奨

*/


#include "GfxLibStdAfx.h"
#include "System/GfxCoreSystem.h"
#include "System/GfxFence.h"
#include "System/GfxDefines.h"
#include "System/GfxDescriptorAllocator.h"

#include "Device/GfxCommandList.h"
#include "Device/GfxGraphicsCommandList.h"
#include "Device/GfxSwapChain.h"

#include "Resource/GfxRootSignature.h"
#include "Resource/GfxRootSignatureDesc.h"
#include "Resource/GfxVertexBuffer.h"
#include "Resource/GfxIndexBuffer.h"
#include "Resource/GfxConstantBuffer.h"
#include "Resource/GfxRenderTarget.h"
#include "Resource/GfxDepthStencil.h"
#include "Resource/GfxTexture2D.h"

#include "State/GfxPipelineState.h"
#include "State/GfxBlendState.h"
#include "State/GfxDepthStencilState.h"
#include "State/GfxInputLayout.h"
#include "State/GfxPipelineState.h"
#include "State/GfxRasterizerState.h"

#include "Shader/GfxShader.h"

#include "Font/GfxFontData.h"
#include "Font/GfxFontRenderer.h"
#include "Font/GfxFontSystem.h"


#include "RayTracing/GfxAccelerationStructure.h"
#include "RayTracing/GfxPipelineStateSubobject.h"
#include "RayTracing/GfxRtGeometry.h"
#include "RayTracing/GfxRtPipelineStateDesc.h"
#include "RayTracing/GfxShaderTable.h"


#endif // !___INCLUDE_GFXLIB_D3D12_H__
