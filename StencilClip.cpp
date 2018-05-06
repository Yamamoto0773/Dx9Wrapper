// StencilClip.cpp

// �X�e���V���N���b�s���O

#include "StencilClip.hpp"

namespace stencil {
	StencilClip::StencilClip() :
		mode			( Mode::Masking ),
		writeColor		(MaskColor::Fill),
		testColor		(MaskColor::Fill),
		curZTest		( 0 ),
		curZFunc		( 0 )
	{
	}

	StencilClip::~StencilClip() {
	}

	

	//! �N���b�s���O�̈�쐬�J�n
	bool StencilClip::regionBegin( IDirect3DDevice9* device, bool isClear ) {
		if ( !device ) {
			return false;
		}

		// �X�e���V���o�b�t�@�̂ݏ����l�ŃN���A
		if (isClear) {
			device->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, static_cast<DWORD>(MaskColor::Trans));
		}

		// ������Z�e�X�g�p�����[�^��ۑ�
		device->GetRenderState( D3DRS_ZENABLE, &curZTest );
		device->GetRenderState( D3DRS_ZFUNC,   &curZFunc );

		// �X�e���V���o�b�t�@��L����
		// �J���[�̏������݂͑j�~�������̂�
		// Z�e�X�g�����ׂĎ��s�����邪�X�e���V���o�b�t�@�ւ̏������݂͋�����
		device->SetRenderState( D3DRS_ZENABLE, true );
		device->SetRenderState( D3DRS_ZFUNC, D3DCMP_NEVER );

		device->SetRenderState( D3DRS_STENCILENABLE, true );
		device->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
		device->SetRenderState( D3DRS_STENCILPASS,	D3DSTENCILOP_REPLACE );
		device->SetRenderState( D3DRS_STENCILZFAIL,	D3DSTENCILOP_REPLACE );
		device->SetRenderState( D3DRS_STENCILREF,	static_cast<DWORD>(writeColor) );
		device->SetRenderState( D3DRS_STENCILMASK, 0xff );

		mode = Mode::Masking;

		return true;
	}

	
	//! �N���b�s���O�̈�쐬�I��
	bool StencilClip::regionEnd(IDirect3DDevice9* device) {
		if ( !device ) {
			return false;
		}

		// �X�e���V�����ꎞ������
		// Z�e�X�g��߂�
		device->SetRenderState( D3DRS_STENCILENABLE, false );
		device->SetRenderState( D3DRS_ZENABLE, curZTest );
		device->SetRenderState( D3DRS_ZFUNC, curZFunc );

		mode = Mode::Idle;

		return true;
	}

	//! �N���b�s���O�`��J�n
	bool StencilClip::drawBegin(IDirect3DDevice9* device) {
		if ( !device ) {
			return false;
		}

		// ���łɏ������܂�Ă���X�e���V���l�ɑ΂���
		// �}�X�N�F�łȂ����������e�X�g���i�ɂ���
		device->SetRenderState( D3DRS_STENCILENABLE, true );
		device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
		device->SetRenderState( D3DRS_STENCILPASS,	D3DSTENCILOP_KEEP );
		device->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		device->SetRenderState( D3DRS_STENCILREF, static_cast<DWORD>(testColor) );
		device->SetRenderState( D3DRS_STENCILMASK, 0xff );

		mode = Mode::Draw;

		return true;
	}

	//! �N���b�s���O�`��I��
	bool StencilClip::drawEnd(IDirect3DDevice9* device ) {
		if ( !device ) {
			return false;
		}

		// �X�e���V���𖳌���
		device->SetRenderState( D3DRS_STENCILENABLE, false );

		mode = Mode::Idle;

		return true;
	}


	void StencilClip::setMaskingColor(IDirect3DDevice9* device, MaskColor color ) {
		testColor = color;

		if ( device && mode == Mode::Draw ) {
			device->SetRenderState( D3DRS_STENCILREF, static_cast<DWORD>(testColor) );
		}
	}


	Mode StencilClip::getCurrectMode() {
		return mode;
	}

	MaskColor StencilClip::getRefMaskColor() {
		return testColor;
	}



	MaskManager::MaskManager() :
		currentMaskType(MaskType::Main)
	{
	}

	MaskManager::~MaskManager() {
	}

	bool MaskManager::create(IDirect3DDevice9 * dev) {
		if (!dev) return false;

		// create a reserve stencil buffer
		IDirect3DSurface9 *ptr;
		IDirect3DSurface9 *ptr_create;
		D3DSURFACE_DESC desc;

		dev->GetDepthStencilSurface(&ptr);
		ptr->GetDesc(&desc);

		if (FAILED(dev->CreateDepthStencilSurface(
			desc.Width,
			desc.Height,
			desc.Format,
			desc.MultiSampleType,
			desc.MultiSampleQuality,
			false,
			&ptr_create,
			NULL
			))) {
			return false;
		}

		ptr->Release();

		reserveStencilBuf.Attach(ptr_create);

		currentMask = &stencilClip[static_cast<size_t>(currentMaskType)];

		return true;
	}

	bool MaskManager::regionBegin(IDirect3DDevice9 * device, bool isClear) {
		if (!currentMask) return false;
		return currentMask->regionBegin(device, isClear);
	}

	bool MaskManager::regionEnd(IDirect3DDevice9 * device) {
		if (!currentMask) return false;
		return currentMask->regionEnd(device);
	}

	bool MaskManager::drawBegin(IDirect3DDevice9 * device) {
		if (!currentMask) return false;
		return currentMask->drawBegin(device);
	}

	bool MaskManager::drawEnd(IDirect3DDevice9 * device) {
		if (!currentMask) return false;
		return currentMask->drawEnd(device);
	}

	void MaskManager::setMaskingColor(IDirect3DDevice9 * device, MaskColor color) {
		if (!currentMask) return ;
		currentMask->setMaskingColor(device, color);
	}

	Mode MaskManager::getCurrectMode() {
		if (!currentMask) return Mode::Idle;
		return currentMask->getCurrectMode();
	}

	MaskColor MaskManager::getRefMaskColor() {
		if (!currentMask) return MaskColor::None;
		return currentMask->getRefMaskColor();
	}


	bool MaskManager::changeMask(IDirect3DDevice9 *dev, MaskType type) {
		if (!dev) return false;
		if (!reserveStencilBuf) return false;
			
		if (currentMaskType != type) {

			// reserve the main stencil buffer
			if (currentMaskType == MaskType::Main) {
				IDirect3DSurface9 *ptr;
				dev->GetDepthStencilSurface(&ptr);
				
				// update
				mainStencilBuf.Release();
				mainStencilBuf.Attach(ptr);
			}

			// swap ownership
			IDirect3DSurface9 *ptr1, *ptr2;
			ptr1 = reserveStencilBuf.Detach();
			ptr2 = mainStencilBuf.Detach();
			reserveStencilBuf.Attach(ptr2);
			mainStencilBuf.Attach(ptr1);

			dev->SetDepthStencilSurface(mainStencilBuf);

	
			currentMask = &stencilClip[static_cast<size_t>(currentMaskType)];
		

			currentMaskType = type;
		}

		return true;
	}


}