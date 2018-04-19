#pragma once

#include <Windows.h>
#include "template.hpp"

namespace dx9 {

	inline DWORD getRGBA(size_t r, size_t g, size_t b, size_t a) {
		r &= 0xff, g &= 0xff, b &= 0xff, a &= 0xff;
		return ( (r<<24)|(g<<16)|(b<<8)|a );
	}


	struct RectF {
		float left;
		float top;
		float right;
		float bottom;
	};

	struct PointF {
		float x, y;
	};

	struct Size {
		size_t w;
		size_t h;
	};

	struct UVCoord {
		float left;
		float top;
		float w;
		float h;
	};


	// テクスチャフィルタの指定
	// 参照:https://msdn.microsoft.com/ja-jp/library/cc324337.aspx
	enum class TextureFilter : int {
		NONE = 0,		// ミップマップ処理を無効
		POINT,			// 点フィルタリング
		LINEAR,			// バイリニア補完フィルタリング 目的のピクセルを囲む 2 × 2 領域のテクセルの重み付き平均を使う。
		ANISOTROPIC,	// 異方性テクスチャ フィルタリング。テクスチャ ポリゴンとスクリーン平面間の角度の相違により起こる歪みを補正する。
		PYRAMIDALQUAD,	// 4 サンプルのテント フィルタ。
		GAUSSIANQUAD	// 4 サンプルのガウス フィルタ。
	};


	// 描画時に座標を指定するときのオプション
	enum class DrawTexCoord {
		TOP_L = 0,
		TOP_R,
		BOTTOM_R,
		BOTTOM_L,
		CENTER
	};
	using FontRotOrigin = DrawTexCoord;


	struct CharInfo {
		int sizeW;			// 文字の幅
		int sizeH;			// 文字の高さ
		int AreaW;			// 文字の幅　(空白あり
		int AreaH;			// 文字の高さ(空白あり
		int originX;		// 文字の原点のx座標
		int originY;		// 文字の原点のy座標
	};


	// 文字の位置に関する指定
	enum class TEXTALIGN : UINT {
		LEFT		= 0b00000001,	// 左寄せ
		CENTERX		= 0b00000010,	// 中央寄せ
		RIGHT		= 0b00000100,	// 右寄せ
		CENTERXY	= 0b00001000,	// 中心寄せ　※1行のみ
		NONE		= 0b00000000,	// 指定なし　(描画領域は無視され、折り返しされません)
	};


	// 文字の拡大縮小に関する指定
	enum class TEXTSCALE : UINT {
		AUTOX	= 0b00010000,	// 描画領域に合わせて横方向に縮小 (はみ出す場合のみ) ※1行のみ
		AUTOXY	= 0b00100000,	// 描画領域に合わせて縦横比を維持したまま縮小 (はみ出す場合のみ) ※1行のみ
		NONE	= 0b00000000,	// 指定なし
	};


	inline TEXTALIGN getAlign(DWORD format) {
		return static_cast<TEXTALIGN>(format&0x0F);
	}

	inline TEXTSCALE getScale(DWORD format) {
		return static_cast<TEXTSCALE>(format&0xF0);
	}



	enum class AntialiasLevel : UINT {
		NONE = 1,
		_3STEPS = 4,
		_15STEPS = 5,
		_65STEPS = 6
	};

	
}
