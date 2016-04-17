#ifndef __INCLUDE_GFXROOTSIGNATURE_H__
#define	__INCLUDE_GFXROOTSIGNATURE_H__


namespace GfxLib
{

	class RootSignatureDesc;


	class RootSignature
	{
	public:
		RootSignature();
		~RootSignature();


		bool	Initialize( const RootSignatureDesc &desc );
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


	private:
		D3DPtr< ID3D12RootSignature >	m_d3dRootSignature;

	};










}
#endif // !__INCLUDE_GFXROOTSIGNATURE_H__
