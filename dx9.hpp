#pragma once


#include "template.hpp"
#include "resource.h"
#include "Color.hpp"

namespace dx9 {

	
	struct RectF {
		RectF(float left, float top, float right, float bottom) : 
			left(left), top(top), right(right), bottom(bottom) {}

		RectF() {};
		~RectF() {};

		float left;
		float top;
		float right;
		float bottom;
	};


	struct PointF {
		PointF() {};
		~PointF() {};
		PointF(float x, float y) : x(x), y(y) {};
		float x, y;
	};
	struct Point {
		Point() {};
		~Point() {};
		Point(int x, int y) : x(x), y(y) {};
		int x, y;
	};

	struct Size {
		Size() {};
		~Size() {};
		Size(size_t w, size_t h) : w(w), h(h) {};
		size_t w;
		size_t h;
	};

	typedef PointF Vec2;

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
		NONE					= 0b000,	// 自動で拡大縮小しない
		ASPECT_UNFIXED			= 0b001,	// アスペクト比を維持しない
		ASPECT_FIXED			= 0b010,	// アスペクト比を維持
		ASPECT_UNFIXED_REDUCEONLY	= 0b101,	// アスペクト比を維持しない，縮小のみ
		ASPECT_FIXED_REDUCEONLY		= 0b110,	// アスペクト比を維持，縮小のみ
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
			Add_UVTex_color,
			CircleFrame
		};

	}
	
}
