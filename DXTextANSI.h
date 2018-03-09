#ifndef _DXTEXTANSI_H
#define _DXTEXTANSI_H

//////////////////////////////////
// DirectX9.0を用いて英数字を描画するクラスです．
// 最初にフォントのビットマップデータを生成してしまうので，
// 変化する文字を高速に描画できます．(頑張ってシェーダを使いました)
// (c) 2018, Yamamoto Nanami
/////////////////////////////////


#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <windows.h>
#include <tchar.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <stdio.h>
#include <strsafe.h>
#include <locale.h>


#include "DirectXTextClass.h"

#define CHARACTERKIND		(0x7E - 0x20 +1)
#define MAXCHARACTER		1024


#define SAFE_FREE(x)		{ if(x){free(x); x=NULL;} }
#define SAFE_RELEASE(x)		{ if(x){x->Release(); x=NULL;} }


class DXTextANSI {
private:
	LPDIRECT3DDEVICE9		lpDev;

	IDirect3DVertexBuffer9	*lpVertexBuffer;
	ID3DXEffect				*lpEffect;			// シェーダ管理用
	IDirect3DVertexDeclaration9	*lpDecl;		// 頂点宣言作成

	int						iDrawWidth;
	int						iDrawHeight;

	int						iFontSize;

	int						test;

	
	IDirect3DTexture9		*lpFontTex[CHARACTERKIND];

	CharInfo				mCharData[CHARACTERKIND];

public:
	DXTextANSI();
	~DXTextANSI();

	// DXTextANSI初期化　※描画前に必ず呼び出す
	BOOL	Init(LPDIRECT3DDEVICE9 dev, int drawWidth, int drawHeight);
	// テクスチャ生成　ANSI文字のテクスチャを作成　※描画前に呼び出す
	BOOL	Create(int fontSize, int fontWeight, WCHAR *fontName, bool italic);
	// 文字描画
	BOOL	Draw(int x, int y, int fontSize, int charInterval, DWORD color, const char *s, ...);
	// 与えられた領域内へ文字描画
	BOOL	DrawInRect(RECT *rect, int fontSize, int charInterval, DWORD format, DWORD color, const char *s, ...);
	// フォントテクスチャを開放する　(デストラクタで自動的に呼び出されます。明示的に呼び出す必要はありません
	BOOL	Clear();

	//　RGBAをDWORD型に変換する関数。色を4成分で指定したいときに使います
	DWORD	ConvertFromRGBA(int red, int green, int blue, int alpha = 255);

private:

	// 与えられた描画領域とフォーマットから、最終的な描画座標を計算
	int	CalcTextPosition(RECT *rect, float inScale, int charInterval, DWORD format, const char *s, POINT *pt, float *outScale);
	// 実際に描画を担当する関数
	int		DrawTEXT(RECT *rect, int fontSize, int charInterval, DWORD format, DWORD color, const char *s, va_list arg);
	// 文字列を最適化する (戻り値は最適化後の文字数)
	int		OptimizeString(char *dst, const char *src);

};


#endif // !_DIRECTXFONT_H