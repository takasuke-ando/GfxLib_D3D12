#pragma once



#include "Util/GfxAutoDescriptorHandle.h"


namespace GfxLib
{



	/***************************************************************
	@brief	サンプラクラス
	@par	[説明]
		サンプラリソースをカプセル化
	*/
	class Sampler
	{
	public:

		Sampler();
		~Sampler();


		BOOL	Initialize( const D3D12_SAMPLER_DESC &bd );
		void	Finalize();



		static D3D12_SAMPLER_DESC	GetDefaultDesc();

		D3D12_CPU_DESCRIPTOR_HANDLE	GetDescHandle() const { return m_SampHandle; }	//!<	CPUハンドル


	private:
		
		
		AutoDescriptorHandle< DescriptorHeapType::SAMPLER >	m_SampHandle;	//!<	CPUハンドル
	};

}