#pragma once

#include <Windows.h>

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

	typedef DrawTexCoord FontRotOrigin;

}
