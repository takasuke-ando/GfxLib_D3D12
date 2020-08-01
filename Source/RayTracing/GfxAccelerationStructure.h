#ifndef __INCLUDE_GFXACCELERATIONSTRUCTURE_H__
#define __INCLUDE_GFXACCELERATIONSTRUCTURE_H__



#include "../Resource/GfxResource.h"


namespace GfxLib
{

	class RtGeometry;
	class GraphicsCommandList;
	class CommandList;
	class Buffer;


	/*
	
		@brief
			ASの基底クラス
			直接使うことはない
	
	*/
	class AccelerationStructure :
		public Resource
	{
		typedef Resource SuperClass;
	public:
		AccelerationStructure();
		virtual ~AccelerationStructure();


		/*
			@brief	
				BLASを初期化。
				リソースの確保のみを行う

				構築処理は描画タイムラインで処理を行うため
				Build()を別途呼ぶ必要がある
		
		*/
		//bool	InitializeBottomLevel(const RtGeometry* geometrys,size_t numGeom);
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE );


		size_t  GetScratchDataSizeInBytes() const { return m_prebuildInfo.ScratchDataSizeInBytes; }


	protected:

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO	m_prebuildInfo;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS	m_asInputs;



		//Top Level
		//std::vector<const AccelerationStructure*>	m_vec

	};
		

	/*
		@brief
			BLASをカプセル化
			初期化時にはメモリが確保されるだけで中身は構築されない
			Initialize後に、スクラッチバッファを確保しておき
			描画タイムラインでBuildを呼び出すこと
	*/
	class BottomLevelAccelerationStructure :
		public AccelerationStructure
	{
		typedef AccelerationStructure SuperClass;
	public:

		BottomLevelAccelerationStructure();
		~BottomLevelAccelerationStructure();

		/***************************************************************
			@brief
				BLASを初期化。
				リソースの確保のみを行う

				構築処理は描画タイムラインで処理を行うため
				Build()を別途呼ぶ必要がある

		*/
		bool	Initialize(const RtGeometry* geometrys, size_t numGeom);
		bool	Initialize(const D3D12_RAYTRACING_GEOMETRY_DESC* geometrys, size_t numGeom);

		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);



		void	Build(CommandList& cmdList, Buffer* scratchBuffer);

		const	DirectX::XMMATRIX& GetTransform() const { return m_Transform; }
		void	SetTransform(const DirectX::XMMATRIX& mtx )  { m_Transform = mtx; }

	private:

		/***************************************************************
			@brief	初期化処理共通部分
			@par	[説明]
				Initializeの後半共通部分
			@param
		*/
		bool	_Initialize( size_t numGeom);

		//Bottom level
		std::vector<const RtGeometry*>	m_vecGeometry;				//!<	参照するレイトレーシングジオメトリ
		std::vector< D3D12_RAYTRACING_GEOMETRY_DESC> m_vecGeomDesc;	//!<	CPUタイムラインでしか参照されない（はず）

		DirectX::XMMATRIX	m_Transform;

	};


	/*
		@brief
			TLASをカプセル化
			初期化時にはメモリが確保されるだけで中身は構築されない
			Initialize後に、スクラッチバッファを確保しておき
			描画タイムラインでBuildを呼び出すこと
	*/
	class TopLevelAccelerationStructure :
		public AccelerationStructure
	{
		typedef AccelerationStructure SuperClass;
	public:

		TopLevelAccelerationStructure();
		~TopLevelAccelerationStructure();

		/*
			@brief
				BLASを初期化。
				リソースの確保のみを行う

				構築処理は描画タイムラインで処理を行うため
				Build()を別途呼ぶ必要がある

				blasが指し示すBottomLevelAccelerationStructureの実体の所有権は
				移らないため、呼び出し側で管理する必要がある

		*/
		bool	Initialize(const BottomLevelAccelerationStructure* const * blas, size_t numBlas);
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		void	Build(CommandList &cmdList, Buffer* scratchBuffer);


	private:
		//Bottom level
		std::vector<const BottomLevelAccelerationStructure*>	m_vecBLAS;	//!< 参照するレイトレーシングジオメトリ
		//std::vector< D3D12_RAYTRACING_INSTANCE_DESC >			m_vecGeomDesc;	//!<	CPUタイムラインでしか参照されない（はず）

	};

}


#endif //__INCLUDE_GFXACCELERATIONSTRUCTURE_H__
