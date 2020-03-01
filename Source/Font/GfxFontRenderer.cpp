
/*

	フォントレンダラ

	通常、スタック上にこのインスタンスを生成する

	このインスタンスをなるべく使いまわす事がパフォーマンス上重要

*/



#include "stdafx.h"
#include "GfxFontRenderer.h"
#include "Device\GfxGraphicsCommandList.h"
#include "GfxFontSystem.h"
#include "GfxFontData.h"
#include <stdio.h>


using namespace DirectX;

namespace GfxLib
{


FontRenderer::FontRenderer( GfxLib::GraphicsCommandList *pContext , FontSystem* pSystem , FontData *pFontData, const D3D12_VIEWPORT&vp )
{
	m_pContext = pContext;
	m_pFontData = pFontData;
	m_pFontSystem = pSystem;
	m_NestedBeginEndCount = 0;

	UINT numViewports = 1;

	m_vp = vp;

	m_pFontBuff = NULL;
	m_FontBuffRest = 0;
	m_fFontPosX = 0;
	m_fFontPosY = 0;
	m_FontBuffWrote = 0;
	m_NextDrawIndex = 0;
}



FontRenderer::~FontRenderer()
{

	GFX_ASSERT( m_NestedBeginEndCount == 0 , "mismatch Begin/End Branket" );


}



void	FontRenderer::DrawText( float x , float y ,  const wchar_t *text )
{
	Begin();


	_SetPosition(x,y);
	_DrawText(text);


	End();
}


void	FontRenderer::DrawFormatText( float x , float y , const wchar_t *text , ... )
{

	Begin();


	_SetPosition(x,y);

	va_list vlist;
	va_start( vlist , text );


	wchar_t buff[128];
	vswprintf_s( buff , text , vlist );

	va_end( vlist );

	_DrawText(buff);


	End();
}



void	FontRenderer::_SetPosition( float x , float y )
{
	m_fFontPosX = x;
	m_fFontPosY = y;
}





void	FontRenderer::Begin()
{

	++m_NestedBeginEndCount;
	if ( m_NestedBeginEndCount == 1 )
	{

		_Begin();

	}
}



void	FontRenderer::End()
{

	GFX_ASSERT( m_NestedBeginEndCount > 0 , "Invalid Begin/End Branket" );

	--m_NestedBeginEndCount;

	if ( m_NestedBeginEndCount == 0 )
	{
		_End();
	}	

}



/*

	一連の描画処理の前準備

*/


void FontRenderer::_Begin()
{

	m_pFontSystem->SetupStates( *m_pContext );

	// todo サンプラ設定等

	//m_pContext->PSSetShaderResource( 0 , &m_pFontData->GetTexture() );


}



/*

	フォント描画処理の後始末 

*/
void	FontRenderer::_End()
{
	_Flush();
}



/*

	テキストのレンダリング
	（実際には頂点の生成のみ）
	を行う

	DrawText11DXUTよりコピペ

*/

void	FontRenderer::_DrawText( const wchar_t * strText )
{

	
	//RECT rcScreen( m_fFontPosX , m_fFontPosY , m_fFontPosX + 100 , m_fFontPosY + 100 );
	XMFLOAT4 FontColor ( 1.f , 1.f, 1.f, 1.f  );
	float fBBWidth = m_vp.Width;
	float fBBHeight= m_vp.Height;
	bool bCenter = FALSE;



	float fCharTexSizeX = 0.010526315f;
	//float fGlyphSizeX = 14.0f / fBBWidth;
	//float fGlyphSizeY = 32.0f / fBBHeight;
	//float fGlyphSizeX = 15.0f / fBBWidth;
	//float fGlyphSizeY = 42.0f / fBBHeight;
	float fGlyphSizeX = 10.0f * 2 / fBBWidth;
	float fGlyphSizeY = 20.0f * 2 / fBBHeight;


	float fRectLeft = m_fFontPosX / fBBWidth;
	float fRectTop = 1.0f - m_fFontPosY / fBBHeight;

	fRectLeft = fRectLeft * 2.0f - 1.0f;
	fRectTop = fRectTop * 2.0f - 1.0f;

	int NumChars = (int)wcslen( strText );
	/*
	if (bCenter) {
		float fRectRight = rcScreen.right / fBBWidth;
		fRectRight = fRectRight * 2.0f - 1.0f;
		float fRectBottom = 1.0f - rcScreen.bottom / fBBHeight;
		fRectBottom = fRectBottom * 2.0f - 1.0f;
		float fcenterx = ((fRectRight - fRectLeft) - (float)NumChars*fGlyphSizeX) *0.5f;
		float fcentery = ((fRectTop - fRectBottom) - (float)1*fGlyphSizeY) *0.5f;
		fRectLeft += fcenterx ;    
		fRectTop -= fcentery;
	}
	*/
	float fOriginalLeft = fRectLeft;
	float fTexTop = 0.0f;
	float fTexBottom = 1.0f;

	float fDepth = 0.5f;
	for( int i=0; i<NumChars; i++ )
	{
		if( strText[i] == '\n' )
		{
			fRectLeft = fOriginalLeft;
			fRectTop -= fGlyphSizeY;

			continue;
		}
		else if( strText[i] < 32 || strText[i] > 126 )
		{
			continue;
		}

		// Add 6 sprite vertices
		FontVertex SpriteVertex[6];
		float fRectRight = fRectLeft + fGlyphSizeX;
		float fRectBottom = fRectTop - fGlyphSizeY;
		float fTexLeft = ( strText[i] - 32 ) * fCharTexSizeX;
		float fTexRight = fTexLeft + fCharTexSizeX;

// 		fRectLeft = -1.f;
// 		fRectRight = 1.f;
// 		fRectTop = 1.f;
// 		fRectBottom = -1.f;

		// tri1
		SpriteVertex[0].vPosition = XMFLOAT3( fRectLeft, fRectTop, fDepth );
		SpriteVertex[0].vTexCoord = XMFLOAT2( fTexLeft, fTexTop );
		SpriteVertex[0].vColor = FontColor;

		SpriteVertex[1].vPosition = XMFLOAT3( fRectRight, fRectTop, fDepth );
		SpriteVertex[1].vTexCoord = XMFLOAT2( fTexRight, fTexTop );
		SpriteVertex[1].vColor = FontColor;

		SpriteVertex[2].vPosition = XMFLOAT3( fRectLeft, fRectBottom, fDepth );
		SpriteVertex[2].vTexCoord = XMFLOAT2( fTexLeft, fTexBottom );
		SpriteVertex[2].vColor = FontColor;

		// tri2
		SpriteVertex[3].vPosition = XMFLOAT3( fRectRight, fRectTop, fDepth );
		SpriteVertex[3].vTexCoord = XMFLOAT2( fTexRight, fTexTop );
		SpriteVertex[3].vColor = FontColor;

		SpriteVertex[4].vPosition = XMFLOAT3( fRectRight, fRectBottom, fDepth );
		SpriteVertex[4].vTexCoord = XMFLOAT2( fTexRight, fTexBottom );
		SpriteVertex[4].vColor = FontColor;

		SpriteVertex[5].vPosition = XMFLOAT3( fRectLeft, fRectBottom, fDepth );
		SpriteVertex[5].vTexCoord = XMFLOAT2( fTexLeft, fTexBottom );
		SpriteVertex[5].vColor = FontColor;


		_InputVertices( SpriteVertex , 6 );

		fRectLeft += fGlyphSizeX;

	}


}




void	FontRenderer::_InputVertices( const FontVertex fontVtx[] , UINT vtxNum )
{
	if ( m_FontBuffRest < vtxNum )
	{
		_Flush();
		_AllocateBuffer();
	}

	CopyMemory( m_pFontBuff , fontVtx , sizeof( FontVertex ) * vtxNum );

	//	バッファ位置を進める
	m_FontBuffRest -= vtxNum;
	m_FontBuffWrote += vtxNum;

	m_pFontBuff += vtxNum;

}



void	FontRenderer::_Flush()
{

	if ( m_FontBuffWrote > 0 )
	{

		//m_pContext->EndWriteVertexBuffer( &m_pFontSystem->GetFontVertexBuffer() );
		//_pContext->Draw( m_FontBuffWrote , 0 );

		m_pContext->FlushPipeline();

		uint32_t startVertex = m_NextDrawIndex;
		m_pContext->GetD3DCommandList()->DrawInstanced(m_FontBuffWrote, 1, startVertex, 0);

		m_NextDrawIndex += m_FontBuffWrote;
		m_FontBuffWrote = 0;
		m_FontBuffRest = 0;
		m_pFontBuff = NULL;

	}



}



void	FontRenderer::_AllocateBuffer()
{
	_Flush();

	/*
		毎度Discardする
	*/
	//m_pFontBuff = (FontVertex*) m_pContext->BeginWriteVertexBuffer( &m_pFontSystem->GetFontVertexBuffer() , TRUE );

	uint32_t AllocateSize = 128 * 6 * sizeof(FontVertex);
	GpuBufferRange buffRange = m_pContext->AllocateGpuBuffer(AllocateSize, 16);

	m_pFontBuff = (FontVertex*)buffRange.GetCpuAddr();

	if ( m_pFontBuff )
	{

		//	フォントバッファ残り
		m_FontBuffRest = AllocateSize / sizeof( FontVertex );

		D3D12_VERTEX_BUFFER_VIEW	vbv = {};
		vbv.BufferLocation = buffRange.GetGpuAddr();
		vbv.SizeInBytes = (UINT)buffRange.GetSize();
		vbv.StrideInBytes = sizeof(FontVertex);

		m_pContext->GetD3DCommandList()->IASetVertexBuffers(0, 1, &vbv);

		m_NextDrawIndex = 0;

	}


}





}

