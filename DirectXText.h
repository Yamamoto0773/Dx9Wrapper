#ifndef _DIRECTXFONT_H
#define _DIRECTXFONT_H

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


//////////////////////////////////
// DirectX9.0を用いて文字を描画するクラスです．
// 決まった文字列を表示する分にはそれなりに使えます．
// (c) 2018, Yamamoto Nanami
/////////////////////////////////



#include <windows.h>
#include <tchar.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <stdio.h>

#include "DirectXTextClass.h"

#define DFONT_MAXSTRING 128
#define DFONT_MAXCHARACTER 256

class DirectXText {
private:
	LPDIRECT3DDEVICE9		lpDev;
	IDirect3DVertexBuffer9	*lpVertexBuffer;
	ID3DXEffect				*lpEffect;			// シェーダ管理用
	IDirect3DVertexDeclaration9	*lpDecl;		// 頂点宣言作成

	int						iDrawWidth;
	int						iDrawHeight;

	CHARDATA				*pCharData[DFONT_MAXSTRING];
	int						iFontSize[DFONT_MAXSTRING];

	WCHAR					*pSetString[DFONT_MAXSTRING];

	IDirect3DTexture9		*lpFontTex[DFONT_MAXSTRING][DFONT_MAXCHARACTER];		// 文字のテクスチャ


public:
	DirectXText();
	virtual ~DirectXText();

	//　DirectXText初期化
	BOOL Init(LPDIRECT3DDEVICE9 dev, int drawWidth, int drawHeight);
	// 描画する文字の登録
	BOOL SetString(int strID, FONTSTATUS *status, const char * s, ...);
	// 文字描画
	BOOL Draw(int strID, int x, int y, int fontSize, int charInterval, DWORD color);
	// 指定領域内へ文字描画
	BOOL DrawInRect(int strID, RECT *rect, int fontSize, int charInterval, DWORD format, DWORD color);
	// フォントテクスチャを開放する (デストラクタで自動的に呼び出されます。明示的に呼び出す必要はありません。
	BOOL Clear(int strID);

	
	// FONTSTATUS構造体に値を格納する　(関数を使わずに直接構造体に値を格納しても構いません)
	BOOL SetFontStatus(FONTSTATUS *status, int fontSize, WCHAR *fontName, int fontWeight, bool italic);
	BOOL SetFontStatusEX(FONTSTATUS *status, int fontSize, WCHAR *fontName, int fontWeight, bool italic, bool underLine, bool struckOut, const int antiAliasing = GGO_GRAY4_BITMAP);

	// RGBAをDWORD型に変換する関数。色を4成分で指定したいときに使います
	DWORD ConvertFromRGBA(int red, int green, int blue, int alpha = 255);

private:
	// 与えられた描画領域とフォーマットから、最終的な描画座標を計算
	int	CalcTextPosition(int strID, RECT *rect, float inScale, int charInterval, DWORD format, const WCHAR *s, POINT *pt, float *outScale);
	// 文字列を最適化する (戻り値は最適化後の文字数)
	int OptimizeString(WCHAR *dst, const WCHAR *src);


};


#endif // !_DIRECTXFONT_H

