#define _CRT_SECURE_NO_WARNINGS


#define WRITELOG(x) { if (log != nullptr) { log->tlnwrite(x); } }


#include "FontTexture.hpp"

#include <strsafe.h>
#include <locale.h>

#include <d3dx9.h>

namespace dx9 {


	namespace texture {

		FontTextureW::FontTextureW() {
			// ロケールを日本に設定
			setlocale(LC_CTYPE, "jpn");
			ZeroMemory(&charInfo, sizeof(CharInfo));
		}


		FontTextureW::~FontTextureW() {

		}


		bool FontTextureW::Create(IDirect3DDevice9* dev, HDC hdc, wchar_t _ch, AntialiasLevel level, TEXTMETRICW *tm) {
			using namespace std;

			Delete();

			// ビットマップの階調を取得
			int grad;
			switch (level) {
				case AntialiasLevel::_3STEPS:
					grad = 4;
					break;
				case AntialiasLevel::_15STEPS:
					grad = 16;
					break;
				case AntialiasLevel::_65STEPS:
					grad = 64;
					break;
				default:
					return false;
			}


			//// 文字テクスチャ作成 ////
			if (tm == nullptr) {
				GetTextMetricsW(hdc, tm);
			}
			const MAT2 mat = {{ 0,1 },{ 0,0 },{ 0,0 },{ 0,1 }};


			GLYPHMETRICS gm;
			DWORD size = 0;

			// 文字コード取得
			UINT code = (UINT)_ch;


			///// 例外文字の処理 ////
			bool space = false;
			switch (code) {
				case (UINT)L' ':
				case (UINT)L'　':
					space = true;
					break;
				case (UINT)L'\n':
					return true;
			}


			// 文字のグリフビットマップを取得
			if ((size = GetGlyphOutlineW(hdc, code, static_cast<UINT>(level), &gm, 0, NULL, &mat)) == GDI_ERROR) {
				return false;
			}
			BYTE *pMono = new BYTE[size];
			GetGlyphOutlineW(hdc, code, static_cast<UINT>(level), &gm, size, pMono, &mat);

			// 文字情報を保存
			charInfo.sizeW			= (gm.gmBlackBoxX + 3) / 4 * 4;
			charInfo.sizeH			= gm.gmBlackBoxY;
			charInfo.AreaW			= gm.gmCellIncX;
			charInfo.AreaH			= tm->tmHeight;
			charInfo.originX		= gm.gmptGlyphOrigin.x;
			charInfo.originY		= gm.gmptGlyphOrigin.y - tm->tmAscent;		// 左上原点


			this->ch = _ch;


			// 空テクスチャ作成
			IDirect3DTexture9 *ptr;

			if (FAILED(dev->CreateTexture(
				charInfo.sizeW,
				charInfo.sizeH,
				1,
				0,
				D3DFMT_A8R8G8B8,
				D3DPOOL_MANAGED,
				&ptr,
				NULL
				))
				) {
				WRITELOG("failed to empty texture");
				return false;
			}

			attach(ptr);
			width = charInfo.sizeW;
			height = charInfo.sizeH;


			if (!space) {
				// スペースでない場合
				// テクスチャにフォントビットマップ情報を書き込み
				D3DLOCKED_RECT lockedRect;
				Lock(&lockedRect);							// ロック
				DWORD *texBuf = (DWORD*)lockedRect.pBits;   // テクスチャメモリへのポインタ


				for (int y = 0; y < charInfo.sizeH; y++) {
					for (int x = 0; x < charInfo.sizeW; x++) {
						DWORD alpha = pMono[y * charInfo.sizeW + x] * 255 / grad;
						texBuf[y * charInfo.sizeW + x] = (alpha << 24) & 0xffffffff;
					}
				}


				Unlock();  // アンロック
			}

			delete[] pMono;

			return true;
		}










		FontTextureA::FontTextureA() {
			// ロケールを日本に設定
			setlocale(LC_CTYPE, "jpn");
			ZeroMemory(&charInfo, sizeof(CharInfo));
		}


		FontTextureA::~FontTextureA() {

		}


		bool FontTextureA::Create(IDirect3DDevice9* dev, HDC hdc, char _ch, AntialiasLevel level, TEXTMETRICA *tm) {
			using namespace std;

			Delete();

			// ビットマップの階調を取得
			int grad;
			switch (level) {
				case AntialiasLevel::_3STEPS:
					grad = 4;
					break;
				case AntialiasLevel::_15STEPS:
					grad = 16;
					break;
				case AntialiasLevel::_65STEPS:
					grad = 64;
					break;
				default:
					return false;
			}


			//// 文字テクスチャ作成 ////
			if (tm == nullptr) {
				GetTextMetricsA(hdc, tm);
			}
			const MAT2 mat = {{ 0,1 },{ 0,0 },{ 0,0 },{ 0,1 }};


			GLYPHMETRICS gm;
			DWORD size = 0;

			// 文字コード取得
			UINT code = (UINT)_ch;


			///// 例外文字の処理 ////
			bool space = false;
			switch (code) {
				case (UINT)L' ':
				case (UINT)L'　':
					space = true;
					break;
				case (UINT)L'\n':
					return true;
			}


			// 文字のグリフビットマップを取得
			if ((size = GetGlyphOutlineA(hdc, code, static_cast<UINT>(level), &gm, 0, NULL, &mat)) == GDI_ERROR) {
				return false;
			}
			BYTE *pMono = new BYTE[size];
			GetGlyphOutlineW(hdc, code, static_cast<UINT>(level), &gm, size, pMono, &mat);

			// 文字情報を保存
			charInfo.sizeW			= (gm.gmBlackBoxX + 3) / 4 * 4;
			charInfo.sizeH			= gm.gmBlackBoxY;
			charInfo.AreaW			= gm.gmCellIncX;
			charInfo.AreaH			= tm->tmHeight;
			charInfo.originX		= gm.gmptGlyphOrigin.x;
			charInfo.originY		= gm.gmptGlyphOrigin.y - tm->tmAscent;		// 左上原点


			this->ch = _ch;


			// 空テクスチャ作成
			IDirect3DTexture9 *ptr;

			if (FAILED(dev->CreateTexture(
				charInfo.sizeW,
				charInfo.sizeH,
				1,
				0,
				D3DFMT_A8R8G8B8,
				D3DPOOL_MANAGED,
				&ptr,
				NULL
				))
				) {
				WRITELOG("failed to empty texture");
				return false;
			}

			attach(ptr);
			width = charInfo.sizeW;
			height = charInfo.sizeH;


			if (!space) {
				// スペースでない場合
				// テクスチャにフォントビットマップ情報を書き込み
				D3DLOCKED_RECT lockedRect;
				Lock(&lockedRect);							// ロック
				DWORD *texBuf = (DWORD*)lockedRect.pBits;   // テクスチャメモリへのポインタ

				for (int y = 0; y < charInfo.sizeH; y++) {
					for (int x = 0; x < charInfo.sizeW; x++) {
						DWORD alpha = pMono[y * charInfo.sizeW + x] * 255 / grad;
						texBuf[y * charInfo.sizeW + x] = (alpha << 24) & 0xffffffff;
					}
				}

				Unlock();  // アンロック
			}

			delete[] pMono;

			return true;
		}

	}


}
