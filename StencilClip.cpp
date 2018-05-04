// StencilClip.cpp

// �X�e���V���N���b�s���O

#include "StencilClip.hpp"

namespace stencil {
	StencilClip::StencilClip() :
		mode_			( Mode::Masking ),
		writeColor_		( MaskColor::Fill ),
		refColor_		( MaskColor::Fill ),
		curZTest_		( 0 ),
		curZFunc_		( 0 )
	{
	}

	StencilClip::~StencilClip() {
	}

	//! �N���b�s���O�̈�쐬�J�n
	bool StencilClip::regionBegin( IDirect3DDevice9* device, bool isClear ) {
		if ( device == nullptr ) {
			return false;
		}

		// �X�e���V���o�b�t�@�̂ݏ����l�ŃN���A
		if (isClear) {
			device->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, static_cast<DWORD>(MaskColor::Trans));
		}

		// ������Z�e�X�g�p�����[�^��ۑ�
		device->GetRenderState( D3DRS_ZENABLE, &curZTest_ );
		device->GetRenderState( D3DRS_ZFUNC,   &curZFunc_ );

		// �X�e���V���o�b�t�@��L����
		// �J���[�̏������݂͑j�~�������̂�
		// Z�e�X�g�����ׂĎ��s�����邪�X�e���V���o�b�t�@�ւ̏������݂͋�����
		device->SetRenderState( D3DRS_ZENABLE, true );
		device->SetRenderState( D3DRS_ZFUNC, D3DCMP_NEVER );

		device->SetRenderState( D3DRS_STENCILENABLE, true );
		device->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
		device->SetRenderState( D3DRS_STENCILPASS,	D3DSTENCILOP_REPLACE );
		device->SetRenderState( D3DRS_STENCILZFAIL,	D3DSTENCILOP_REPLACE );
		device->SetRenderState( D3DRS_STENCILREF,	static_cast<DWORD>(writeColor_) );
		device->SetRenderState( D3DRS_STENCILMASK, 0xff );

		mode_ = Mode::Masking;

		return true;
	}

	//! �N���b�s���O�̈�쐬�I��
	bool StencilClip::regionEnd(IDirect3DDevice9* device) {
		if ( device == nullptr ) {
			return false;
		}

		// �X�e���V�����ꎞ������
		// Z�e�X�g��߂�
		device->SetRenderState( D3DRS_STENCILENABLE, false );
		device->SetRenderState( D3DRS_ZENABLE, curZTest_ );
		device->SetRenderState( D3DRS_ZFUNC, curZFunc_ );

		return true;
	}

	//! �N���b�s���O�`��J�n
	bool StencilClip::drawBegin(IDirect3DDevice9* device, bool isKeep) {
		if ( device == nullptr ) {
			return false;
		}

		// ���łɏ������܂�Ă���X�e���V���l�ɑ΂���
		// �}�X�N�F�łȂ����������e�X�g���i�ɂ���
		device->SetRenderState( D3DRS_STENCILENABLE, true );
		device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
		device->SetRenderState( D3DRS_STENCILPASS,	D3DSTENCILOP_KEEP );
		device->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		device->SetRenderState( D3DRS_STENCILREF, static_cast<DWORD>(refColor_) );
		device->SetRenderState( D3DRS_STENCILMASK, 0xff );

		mode_ = Mode::Draw;

		return true;
	}

	//! �N���b�s���O�`��I��
	bool StencilClip::drawEnd(IDirect3DDevice9* device ) {
		if ( device == nullptr ) {
			return false;
		}

		// �X�e���V���𖳌������o�b�t�@���N���A
		device->SetRenderState( D3DRS_STENCILENABLE, false );

		return true;
	}

	//! �������ݎ��}�X�N�J���[�̐ݒ�
	void StencilClip::setWriteMaskColor(IDirect3DDevice9* device, MaskColor color ) {
		writeColor_ = color;
		if ( device && mode_ == Mode::Masking ) {
			device->SetRenderState( D3DRS_STENCILREF, static_cast<DWORD>(writeColor_) );
		}
	}

	//! �`�掞�̃}�X�N�J���[�̎w��
	void StencilClip::setRefMaskColor(IDirect3DDevice9* device, MaskColor color ) {
		refColor_ = color;
		if ( device && mode_ == Mode::Draw ) {
			device->SetRenderState( D3DRS_STENCILREF, static_cast<DWORD>(refColor_) );
		}
	}

	Mode StencilClip::getCurrectMode() {
		return mode_;
	}

	MaskColor StencilClip::getRefMaskColor() {
		return refColor_;
	}

	MaskColor StencilClip::getWriteMaskColor() {
		return writeColor_;
	}

}