// StencilClip.hpp

// �X�e���V���N���b�s���O�N���X
// �Q�l
// http://marupeke296.com/DXG_No54_StencilClipping.html

#ifndef IKD_DIX_STENCILCLIP_H
#define IKD_DIX_STENCILCLIP_H

#include <d3d9.h>
#include <atlbase.h>
#include <array>

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
		Draw,		//! �`�惂�[�h��
		Idle		//! be idle 
	};

	


	class StencilClip {

	public:
		StencilClip();
		virtual ~StencilClip();

		//! �N���b�s���O�̈�쐬�J�n
		bool regionBegin( IDirect3DDevice9* device, bool isClear=true );

		//! �N���b�s���O�̈�쐬�I��
		bool regionEnd( IDirect3DDevice9* device );

		//! �N���b�s���O�`��J�n
		bool drawBegin( IDirect3DDevice9* device);

		//! �N���b�s���O 
		bool drawEnd(IDirect3DDevice9* device);

		// set mask color for rendering
		void setMaskingColor(IDirect3DDevice9* device, MaskColor color);

		void clear(IDirect3DDevice9 *device);
		
		Mode getCurrectMode();
		MaskColor getRefMaskColor();

	private:
		
		Mode				mode;			//!< ���[�h
		MaskColor			writeColor;
		MaskColor			testColor;
		DWORD				curZTest;		//!< ������Z�e�X�g�̗L�� 
		DWORD				curZFunc;		//!< ������Z�e�X�g��r�֐�
	};



}

#endif