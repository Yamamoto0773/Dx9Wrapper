﻿#pragma once


//////////////////////////////////
// DirectX9.0を用いて文字を描画するクラスです．
// (c) 2018, Yamamoto Nanami
/////////////////////////////////



#include <string>
#include <vector>

#include "TextureBase.hpp"


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


	namespace texture {

		// フォントビットマップテクスチャを作成するクラス
		class FontTextureW : public TextureBase {

		private:
			CharInfo	charInfo;
			wchar_t		ch;

		public:
			FontTextureW();
			virtual ~FontTextureW();


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
				d3dtex9.reset();
				name.clear();

				width = 0;
				height = 0;
				isLocked = false;

				ZeroMemory(&charInfo, sizeof(charInfo));
			}

		};



		// フォントビットマップテクスチャを作成するクラス
		class FontTextureA : public TextureBase {

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
				d3dtex9.reset();
				name.clear();

				width = 0;
				height = 0;
				isLocked = false;

				ZeroMemory(&charInfo, sizeof(charInfo));
			}

		};

	}

	

}
