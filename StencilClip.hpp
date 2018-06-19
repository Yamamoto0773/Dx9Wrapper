// StencilClip.hpp

// ステンシルクリッピングクラス
// 参考
// http://marupeke296.com/DXG_No54_StencilClipping.html

#ifndef IKD_DIX_STENCILCLIP_H
#define IKD_DIX_STENCILCLIP_H

#include <d3d9.h>
#include <atlbase.h>
#include <array>

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
		Draw,		//! 描画モード中
		Idle		//! be idle 
	};

	


	class StencilClip {

	public:
		StencilClip();
		virtual ~StencilClip();

		//! クリッピング領域作成開始
		bool regionBegin( IDirect3DDevice9* device, bool isClear=true );

		//! クリッピング領域作成終了
		bool regionEnd( IDirect3DDevice9* device );

		//! クリッピング描画開始
		bool drawBegin( IDirect3DDevice9* device);

		//! クリッピング 
		bool drawEnd(IDirect3DDevice9* device);

		// set mask color for rendering
		void setMaskingColor(IDirect3DDevice9* device, MaskColor color);

		void clear(IDirect3DDevice9 *device);
		
		Mode getCurrectMode();
		MaskColor getRefMaskColor();

	private:
		
		Mode				mode;			//!< モード
		MaskColor			writeColor;
		MaskColor			testColor;
		DWORD				curZTest;		//!< 既存のZテストの有無 
		DWORD				curZFunc;		//!< 既存のZテスト比較関数
	};



}

#endif