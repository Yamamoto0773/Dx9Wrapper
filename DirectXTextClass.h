#pragma once

//////////////////////////////////
// 文字描画クラスで使う定義をまとめたやつです
// (c) 2018, Yamamoto Nanami
/////////////////////////////////


#include <windows.h>


typedef struct _Vtx {
	float x, y, z;
	float u, v;
}Vtx;


typedef struct _FONTSTATUS {
	int		iFontSize;		// 文字の大きさ
	int		iFontWeight;	// 文字の太さ
	WCHAR	wcFontName[32];	// フォント名
	int		iAntiAliasing;	// アンチエイリアスのレベル
	bool	bItalic;		// イタリック体フォント
	bool	bUnderline;		// 下線付きフォント
	bool	bStruckOut;		// 打ち消し線付きフォント
}FONTSTATUS;


typedef struct _CHARDATA {
	int iWidth;			// 文字の幅
	int iHeight;		// 文字の高さ
	int iAreaWidth;		// 文字の幅　(空白あり
	int iAreaHeight;	// 文字の高さ(空白あり
	int iOriginX;		// 文字の原点のx座標
	int iOriginY;		// 文字の原点のy座標
}CHARDATA;


// 文字の位置に関する指定
typedef enum _TEXTALIGN {
	TEXTALIGN_LEFT		= 0b00000001,	// 左寄せ
	TEXTALIGN_CENTERX	= 0b00000010,	// 中央寄せ
	TEXTALIGN_RIGHT		= 0b00000100,	// 右寄せ
	TEXTALIGN_CENTERXY	= 0b00001000,	// 中心寄せ　※1行のみ
	TEXTALIGN_NONE		= 0b00000000,	// 指定なし　(描画領域は無視され、折り返しされません)
}TEXTALIGN;


// 文字の拡大縮小に関する指定
typedef enum _TEXTSCALE {
	TEXTSCALE_AUTOX		= 0b00010000,	// 描画領域に合わせて横方向に縮小 (はみ出す場合のみ) ※1行のみ
	TEXTSCALE_AUTOXY	= 0b00100000,	// 描画領域に合わせて縦横比を維持したまま縮小 (はみ出す場合のみ) ※1行のみ
	TEXTSCALE_NONE		= 0b00000000,	// 指定なし
}TEXTSCALE;