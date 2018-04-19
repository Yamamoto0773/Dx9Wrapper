#pragma once

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
#include <string>
#include <vector>

#include "DXTextureManager.hpp"


namespace dx9 {

	enum class FontWeight : LONG {
		DONTCAARE = 0,
		THIN = 100,
		ULTRALIGHT = 200,
		LIGHT = 300,
		NORMAL = 400,
		MEDIUM = 500,
		SEMIBOLD = 600,
		BOLD = 700,
		ULTRABOLD = 800,
		HEAVY = 900,
	};


	// フォントビットマップテクスチャを作成するクラス
	class FontTextureW : public DXTextureManager {

	private:
		CharInfo	charInfo;
		wchar_t		ch;

	public:
		FontTextureW();
		~FontTextureW();
		
		
		// 指定したフォントの1文字分のテクスチャを作成
		// note:現在デバイスに設定されているフォントが使用される
		bool Create(
			IDirect3DDevice9* dev,
			HDC hdc,
			wchar_t ch,
			AntialiasLevel level=AntialiasLevel::_15STEPS,
			TEXTMETRICW* tm = nullptr
			);

		const CharInfo& _chInfo() { return charInfo; };
		const wchar_t& _ch() { return ch; };

		void Delete() {
			d3dtex9.Release();
			name.clear();

			width = 0;
			height = 0;
			isLocked = false;

			ZeroMemory(&charInfo, sizeof(charInfo));
		}

	};



	// フォントビットマップテクスチャを作成するクラス
	class FontTextureA : public DXTextureManager {

	private:
		CharInfo	charInfo;
		char		ch;

	public:
		FontTextureA();
		~FontTextureA();


		// 指定したフォントの1文字分のテクスチャを作成
		// note:現在デバイスに設定されているフォントが使用される
		bool Create(
			IDirect3DDevice9* dev,
			HDC hdc,
			char ch,
			AntialiasLevel level=AntialiasLevel::_15STEPS,
			TEXTMETRICA* tm = nullptr
			);

		const CharInfo& _chInfo() { return charInfo; };
		const char& _ch() { return ch; };

		void Delete() {
			d3dtex9.Release();
			name.clear();

			width = 0;
			height = 0;
			isLocked = false;

			ZeroMemory(&charInfo, sizeof(charInfo));
		}

	};



	

}
