#pragma once

#include <Windows.h>
#include "template.hpp"
#include "resource.h"

namespace dx9 {

	inline DWORD getRGBA(size_t r, size_t g, size_t b, size_t a) {
		r &= 0xff, g &= 0xff, b &= 0xff, a &= 0xff;
		return static_cast<DWORD>((r<<24)|(g<<16)|(b<<8)|a);
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
	struct Point {
		int x, y;
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

	struct ClipArea {
		size_t upleft_x;
		size_t upleft_y;
		size_t size_w;
		size_t size_h;
	};


	// ブレンドモードを指定する時の列挙体
	enum class BLENDMODE {
		// 通常合成
		// 結果色 = 合成色x合成色.alpha + 背景色x(1-合成色.alpha)
		NORMAL = 0,

		// 加算合成 (光の表現などに用いる)
		// 結果色 = 合成色x合成色.alpha + 背景色x1
		ADD
	};


	// フルシーンマルチサンプリングのレベル
	enum class MultiSampleLv : DWORD {
		NONE = 0,
		_2SAMPLES = 2,
		_4SAMPLES = 4,
		_6SAMPLES = 6,
		_8SAMPLES = 8,
		_12SAMPLES = 12,
		_16SAMPLES = 16
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


	enum class TextureAdjust {
		NONE = 0u,
		ASPECT_UNFIXED,	// アスペクト比を維持しない
		ASPECT_FIXED,		// アスペクト比を維持
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
	enum class TextAlign : UINT {
		LEFT		= 0b00000001,	// 左寄せ
		CENTERX		= 0b00000010,	// 中央寄せ
		RIGHT		= 0b00000100,	// 右寄せ
		CENTERXY	= 0b00001000,	// 中心寄せ　
		NONE		= 0b00000000,	// 指定なし　(描画領域は無視され、折り返しされません)
	};


	enum class AntialiasLevel : UINT {
		_3STEPS = 4,
		_15STEPS = 5,
		_65STEPS = 6
	};


	enum class MaskType : UINT {
		DrawableMask = 0u,	// this mask which express drawble area
		NotDrawableMask		// this mask which express NOT drawable area
	};


	namespace shader {

		enum class ShaderPass : UINT {
			Color = 0,
			Tex,
			Mul_Tex_Color,
			Mul_ColorAlpha_UVTexAlpha,
			Mul_ColorAlpha_InvTexAlpha,
			Mul_UVTex_color,
			Mul_ColorAlpha_InvUVTexAlpha,
			RectFrame, 
			Add_UVTex_color
		};

	}
	
}
