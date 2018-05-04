// StencilClip.hpp

// �X�e���V���N���b�s���O�N���X
// �قƂ�ǁC�܂�؂�����̃\�[�X�R�[�h�ł�
// http://marupeke296.com/DXG_No54_StencilClipping.html

#ifndef IKD_DIX_STENCILCLIP_H
#define IKD_DIX_STENCILCLIP_H

#include <d3d9.h>

namespace stencil {
	// �}�X�N�F
	enum class MaskColor : UINT {
		Trans = 0x00,	//! �����F
		Fill  = 0x01,	//! �h��Ԃ�
		None  = 0xff  //! �����J���[
	};

	// ���[�h
	enum class Mode : UINT {
		Masking,	//! �}�X�N�������݃��[�h��
		Draw		//! �`�惂�[�h��
	};

	class StencilClip {
	public:
		

	public:
		StencilClip();
		virtual ~StencilClip();

		//! �N���b�s���O�̈�쐬�J�n
		virtual bool regionBegin( IDirect3DDevice9* device, bool isClear=true );

		//! �N���b�s���O�̈�쐬�I��
		virtual bool regionEnd( IDirect3DDevice9* device );

		//! �N���b�s���O�`��J�n
		virtual bool drawBegin( IDirect3DDevice9* device, bool isKeep=true);

		//! �N���b�s���O 
		virtual bool drawEnd(IDirect3DDevice9* device);

		//! �������ݎ��}�X�N�J���[�̐ݒ�
		void setWriteMaskColor(IDirect3DDevice9* device, MaskColor color );

		//! �`�掞�̃}�X�N�J���[�̎w��
		void setRefMaskColor(IDirect3DDevice9* device, MaskColor color );

		Mode getCurrectMode();
		MaskColor getRefMaskColor();
		MaskColor getWriteMaskColor();

	protected:
		Mode				mode_;			//!< ���[�h
		MaskColor			writeColor_;	//!< �}�X�N�������ݎ��̐F
		MaskColor			refColor_;		//!< �`�掞�̃}�X�L���O�F
		DWORD				curZTest_;		//!< ������Z�e�X�g�̗L�� 
		DWORD				curZFunc_;		//!< ������Z�e�X�g��r�֐�
	};
}

#endif