#ifndef __INCLUDE_GFXFENCE_H__
#define __INCLUDE_GFXFENCE_H__



/*

	�t�F���X���J�v�Z����
	GPU�|CPU�Ԃœ�������肽���ꍇ�Ɏg�p����

	GPU�Ƃ̓������Ƃ�^��2�ʂ�̎g����������

	�|�[�����O���[�h�̏ꍇ
	Sync�֐��͑����ɐ����Ԃ�

	�ҋ@���[�h�̏ꍇ
	Sync�͓������s����܂ŁA�ҋ@����
	Signal ��Sync�͑΂ɂȂ��ČĂяo����Ȃ��Ƃ����Ȃ�
	���̃��[�h�̏ꍇWindows�̓����I�u�W�F�N�g���g�p���邽�߁A��ʂɍ쐬���邱�Ƃ͐�������Ȃ�

*/



namespace GfxLib
{
	class CoreSystem;

	class Fence
	{
		friend class CoreSystem;
	public:
		Fence();
		~Fence();


		/*
			�t�F���X�̏��������s��

			pollingMode : false�ɂ���ƁA�ҋ@�I�u�W�F�N�g���쐬����
		*/
		bool	Initialize(CoreSystem*,bool pollingMode);
		void	Finalize();


		/*
			�������s��
			�t�F���X��ʉߍς݂̏ꍇ�ATRUE���Ԃ�
			�|�[�����O���[�h�̏ꍇ�A�����ɐ��䂪�߂�
		*/
		bool	Sync();

		bool	IsPollingMode() const { return m_event == NULL; }

	private:

		void	_Insert(CoreSystem *);

		D3DPtr<ID3D12Fence>	m_fence;
		bool				m_bWaiting;
		HANDLE				m_event;		//!<	winAPI �ҋ@�C�x���g
		uint64_t			m_waitingFenceValue;

	};






}






#endif // !__INCLUDE_GFXFENCE_H__
