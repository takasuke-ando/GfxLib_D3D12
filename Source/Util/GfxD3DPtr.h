#pragma	once



namespace GfxLib
{


	template< class D3DType >
	class	D3DPtr
	{

		typedef D3DPtr<D3DType>	ThisClass;

	public:
		D3DPtr( D3DType* p = NULL, bool bAddRef= TRUE )
		{
			m_ptr = p;
			if ( m_ptr && bAddRef )
			{
				m_ptr->AddRef();
			}
		}
		~D3DPtr( )
		{
			Release();
		}


		void	Release()
		{
			if ( m_ptr )
			{
				m_ptr->Release();
				m_ptr = NULL;
			}
		}


		const ThisClass&	operator = ( D3DType * p )
		{
			if ( m_ptr == p )	return *this;

			Release();

			m_ptr = p ;
			if ( m_ptr )
			{
				m_ptr->AddRef();
			}
			return *this;
		}

		operator D3DType* ()	const	
		{
			return m_ptr;
		}
		
		D3DType*	operator ->() const
		{
			return m_ptr;
		}

		//	D3DŠÖ”‚Ìˆø”‚É“n‚·Œ`‚Åó‚¯æ‚é
		D3DType**	InitialAccept()
		{
			GFX_ASSERT( m_ptr ==NULL , "Initial Call only!" );
			return &m_ptr;
		}

	private:
		D3DType*	m_ptr;


	};


}






