// StencilClip.cpp

// ステンシルクリッピング

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

	//! クリッピング領域作成開始
	bool StencilClip::regionBegin( IDirect3DDevice9* device, bool isClear ) {
		if ( device == nullptr ) {
			return false;
		}

		// ステンシルバッファのみ初期値でクリア
		if (isClear) {
			device->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, static_cast<DWORD>(MaskColor::Trans));
		}

		// 既存のZテストパラメータを保存
		device->GetRenderState( D3DRS_ZENABLE, &curZTest_ );
		device->GetRenderState( D3DRS_ZFUNC,   &curZFunc_ );

		// ステンシルバッファを有効化
		// カラーの書き込みは阻止したいので
		// Zテストをすべて失敗させるがステンシルバッファへの書き込みは許可する
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

	//! クリッピング領域作成終了
	bool StencilClip::regionEnd(IDirect3DDevice9* device) {
		if ( device == nullptr ) {
			return false;
		}

		// ステンシルを一時無効化
		// Zテストを戻す
		device->SetRenderState( D3DRS_STENCILENABLE, false );
		device->SetRenderState( D3DRS_ZENABLE, curZTest_ );
		device->SetRenderState( D3DRS_ZFUNC, curZFunc_ );

		return true;
	}

	//! クリッピング描画開始
	bool StencilClip::drawBegin(IDirect3DDevice9* device, bool isKeep) {
		if ( device == nullptr ) {
			return false;
		}

		// すでに書き込まれているステンシル値に対して
		// マスク色でない部分だけテスト合格にする
		device->SetRenderState( D3DRS_STENCILENABLE, true );
		device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
		device->SetRenderState( D3DRS_STENCILPASS,	D3DSTENCILOP_KEEP );
		device->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		device->SetRenderState( D3DRS_STENCILREF, static_cast<DWORD>(refColor_) );
		device->SetRenderState( D3DRS_STENCILMASK, 0xff );

		mode_ = Mode::Draw;

		return true;
	}

	//! クリッピング描画終了
	bool StencilClip::drawEnd(IDirect3DDevice9* device ) {
		if ( device == nullptr ) {
			return false;
		}

		// ステンシルを無効化しバッファをクリア
		device->SetRenderState( D3DRS_STENCILENABLE, false );

		return true;
	}

	//! 書き込み時マスクカラーの設定
	void StencilClip::setWriteMaskColor(IDirect3DDevice9* device, MaskColor color ) {
		writeColor_ = color;
		if ( device && mode_ == Mode::Masking ) {
			device->SetRenderState( D3DRS_STENCILREF, static_cast<DWORD>(writeColor_) );
		}
	}

	//! 描画時のマスクカラーの指定
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