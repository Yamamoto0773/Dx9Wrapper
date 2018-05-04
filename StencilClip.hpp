// StencilClip.hpp

// ステンシルクリッピングクラス
// ほとんど，まるぺけさんのソースコードです
// http://marupeke296.com/DXG_No54_StencilClipping.html

#ifndef IKD_DIX_STENCILCLIP_H
#define IKD_DIX_STENCILCLIP_H

#include <d3d9.h>

namespace stencil {
	// マスク色
	enum class MaskColor : UINT {
		Trans = 0x00,	//! 透明色
		Fill  = 0x01,	//! 塗りつぶし
		None  = 0xff  //! 無効カラー
	};

	// モード
	enum class Mode : UINT {
		Masking,	//! マスク書き込みモード中
		Draw		//! 描画モード中
	};

	class StencilClip {
	public:
		

	public:
		StencilClip();
		virtual ~StencilClip();

		//! クリッピング領域作成開始
		virtual bool regionBegin( IDirect3DDevice9* device, bool isClear=true );

		//! クリッピング領域作成終了
		virtual bool regionEnd( IDirect3DDevice9* device );

		//! クリッピング描画開始
		virtual bool drawBegin( IDirect3DDevice9* device, bool isKeep=true);

		//! クリッピング 
		virtual bool drawEnd(IDirect3DDevice9* device);

		//! 書き込み時マスクカラーの設定
		void setWriteMaskColor(IDirect3DDevice9* device, MaskColor color );

		//! 描画時のマスクカラーの指定
		void setRefMaskColor(IDirect3DDevice9* device, MaskColor color );

		Mode getCurrectMode();
		MaskColor getRefMaskColor();
		MaskColor getWriteMaskColor();

	protected:
		Mode				mode_;			//!< モード
		MaskColor			writeColor_;	//!< マスク書き込み時の色
		MaskColor			refColor_;		//!< 描画時のマスキング色
		DWORD				curZTest_;		//!< 既存のZテストの有無 
		DWORD				curZFunc_;		//!< 既存のZテスト比較関数
	};
}

#endif