#ifndef __INCLUDE_GFXPIPELINESTATESUBOBJECT_H__
#define __INCLUDE_GFXPIPELINESTATESUBOBJECT_H__



#include "Util/GfxStringContainer.h"


namespace GfxLib
{




	/***************************************************************
	@brief	サブオブジェクトクラス
	@par	[説明]
		D3D12_STATE_SUBOBJECTを表現するためのクラス

		RtPipelineStateDesc内で使用される


		D3D12_STATE_SUBOBJECT配列は外部で生成されるため、内部では持たず、
		pDescに設定する各種構造体を派生クラスで保持する

		PreResolveで必要になる構造体数を返し、
		Resolveで構造体配列に設定する



		@param
	*/

	class PipelineStateSubobject
	{
	public:
		PipelineStateSubobject() {};
		virtual ~PipelineStateSubobject() {};


		virtual uint32_t PreResolve() { return 0; } // SUBOBJECT 構造体がいくつ必要か？を返す
		virtual uint32_t Resolve(D3D12_STATE_SUBOBJECT* pDst) { return 0; }	// いくつ格納を行ったか。PreResolveと同じ値を返すこと


	};





	class PipelineState_DxilLibrary :
		public PipelineStateSubobject
	{

	public:

		PipelineState_DxilLibrary();
		~PipelineState_DxilLibrary() {}


		void SetDXILLibrary(const D3D12_SHADER_BYTECODE& byteCode) {
			m_desc.DXILLibrary = byteCode;
		}
		void AddExport(
			LPCWSTR Name,
			LPCWSTR ExportToRename = nullptr,
			D3D12_EXPORT_FLAGS Flags = D3D12_EXPORT_FLAG_NONE);

		virtual uint32_t PreResolve() { return 1; }
		virtual uint32_t Resolve(D3D12_STATE_SUBOBJECT* pDst);



	private:
		D3D12_DXIL_LIBRARY_DESC  m_desc;
		std::vector< D3D12_EXPORT_DESC> m_vecExportDesc;
		StringContainer			m_stringContainer;

	};



	class PipelineState_HitGroup :
		public PipelineStateSubobject
	{

	public:


		PipelineState_HitGroup();
		~PipelineState_HitGroup();


		virtual uint32_t PreResolve() { return 1; }
		virtual uint32_t Resolve(D3D12_STATE_SUBOBJECT* pDst);


		void SetHitGroupType(D3D12_HIT_GROUP_TYPE type) { m_desc.Type = type; }

		void SetHitGroupExport(const wchar_t* str) { m_strHitGroupExport = str; }
		void SetAnyHitShaderImport(const wchar_t* str) { m_strAnyHitShaderExport = str; }
		void SetClosestHitShaderImport(const wchar_t* str) { m_strClosestHitShaderExport = str; }
		void SetIntersectionShaderImport(const wchar_t* str) { m_strIntersectionShaderExport = str; }


	private:
		D3D12_HIT_GROUP_DESC  m_desc;
		StringContainer			m_stringContainer;

		std::wstring	m_strHitGroupExport;
		std::wstring	m_strAnyHitShaderExport;
		std::wstring	m_strClosestHitShaderExport;
		std::wstring	m_strIntersectionShaderExport;

	};



	class PipelineState_RaytracingShaderConfig :
		public PipelineStateSubobject
	{
	public:
		PipelineState_RaytracingShaderConfig();
		virtual ~PipelineState_RaytracingShaderConfig() {}



		virtual uint32_t PreResolve() { return 1; }
		virtual uint32_t Resolve(D3D12_STATE_SUBOBJECT* pDst);


		void Config(uint32_t MaxPayloadSizeInBytes, uint32_t MaxAttributeSizeInBytes)
		{
			m_desc.MaxPayloadSizeInBytes = MaxPayloadSizeInBytes;
			m_desc.MaxAttributeSizeInBytes = MaxAttributeSizeInBytes;
		}

	private:

		D3D12_RAYTRACING_SHADER_CONFIG	m_desc;

	};


	/*
		@brief RootSignature系の基底クラス
	*/
	class PipelineState_RootSignature :
		public PipelineStateSubobject
	{
	public:

		PipelineState_RootSignature() { m_pResolvedSubobject = nullptr; }


		// 解決済みのサブオブジェクトを取得
		const D3D12_STATE_SUBOBJECT* GetResolvedSubobject() const { return m_pResolvedSubobject; }


	protected:

		D3D12_STATE_SUBOBJECT* m_pResolvedSubobject;

	private:


	};


	class PipelineState_LocalRootSignature :
		public PipelineState_RootSignature
	{
	public:
		PipelineState_LocalRootSignature();
		virtual ~PipelineState_LocalRootSignature();


		virtual uint32_t PreResolve() { return 1; }
		virtual uint32_t Resolve(D3D12_STATE_SUBOBJECT* pDst);


		void SetRootSignature(ID3D12RootSignature* rootsig) {
			m_desc.pLocalRootSignature = rootsig;
		}

	private:
		D3D12_LOCAL_ROOT_SIGNATURE	m_desc;

	};


	class PipelineState_GlobalRootSignature :
		public PipelineState_RootSignature
	{
	public:
		PipelineState_GlobalRootSignature();
		virtual ~PipelineState_GlobalRootSignature();


		virtual uint32_t PreResolve() { return 1; }
		virtual uint32_t Resolve(D3D12_STATE_SUBOBJECT* pDst);


		void SetRootSignature(ID3D12RootSignature* rootsig) {
			m_desc.pGlobalRootSignature = rootsig;
		}

	private:
		D3D12_GLOBAL_ROOT_SIGNATURE	m_desc;

	};



	/*
		@brief D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATIONをカプセル化。ほかのサブオブジェクトの解決済みアドレスを利用する
	*/
	class PipelineState_SubobjectToExportsAssociation :
		public PipelineStateSubobject
	{
	public:
		PipelineState_SubobjectToExportsAssociation();
		virtual ~PipelineState_SubobjectToExportsAssociation();


		virtual uint32_t PreResolve() { return 1; }
		virtual uint32_t Resolve(D3D12_STATE_SUBOBJECT* pDst);

		/*
			@brief RootSignatureサブオブジェクトを設定する
		*/
		void SetRootSignature(const PipelineState_RootSignature* rootSig) {
			m_pAssociateRootSig = rootSig;
		}

		void AddExport(const wchar_t* name) {
			m_vecStr.push_back( m_stringContainer.Copy(name) );
		}


	private:
		const PipelineState_RootSignature* m_pAssociateRootSig;

		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION	m_desc;
		std::vector<const wchar_t*>		m_vecStr;
		StringContainer					m_stringContainer;
	};




	class PipelineState_PipelineConfig:
		public PipelineStateSubobject
	{
	public:
		PipelineState_PipelineConfig() ;
		virtual ~PipelineState_PipelineConfig() {};


		virtual uint32_t PreResolve() { return 1; } // SUBOBJECT 構造体がいくつ必要か？を返す
		virtual uint32_t Resolve(D3D12_STATE_SUBOBJECT* pDst);	// いくつ格納を行ったか。PreResolveと同じ値を返すこと


		void Config(uint32_t MaxTraceRecursionDepth)
		{
			m_desc.MaxTraceRecursionDepth = MaxTraceRecursionDepth;
		}

	private:
		D3D12_RAYTRACING_PIPELINE_CONFIG m_desc;

	};



}





#endif
