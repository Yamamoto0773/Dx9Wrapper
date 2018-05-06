﻿#pragma once

#include "FontTexture.hpp"
#include "DX9ShareContainer.hpp"
#include <array>

// ASCII文字描画クラス


namespace dx9 {

	class DirectXFontAscii : resource::DX9ShareContainer {

		static const int CHARACTER_MAXCNT = 1024;	// 文字列の最大文字数


	private:
		HFONT					hFont;			// 設定中のフォントのハンドル
		AntialiasLevel			antialiasLv;

		std::array<char, CHARACTER_MAXCNT+1> workBuf;

		TEXTMETRICA				tm;
	

		std::vector< std::unique_ptr<texture::FontTextureA> > texRes;

		size_t fontSize;
		std::array<float, 4> fontColor;

	public:
		DirectXFontAscii();
		~DirectXFontAscii();

		virtual bool Create(
			const char*		fontName,
			size_t			fontSize,
			FontWeight		fontWeight = FontWeight::NORMAL,
			bool			isItalic = false,
			bool			isUnderLine = false,
			bool			isStrikeOut = false,
			AntialiasLevel	level = AntialiasLevel::_15STEPS
			);


		// ARGBの順
		void SetFontColor(DWORD &color);

		// 0-255の成分指定
		void SetFontColor(size_t r, size_t g, size_t b, size_t a=255);

		
		//////////////////////////////////////////////
		// 文字描画

		// シンプル描画
		bool Draw(
			float x,
			float y,		
			const char* s, ...
			);

		// 指定領域内へ文字描画
		bool DrawInRect(
			RectF &rect,
			TextAlign format,			
			const char* s, ...
			);

		// 文字送り
		// note:最後の文字まで描画する時は，drawChCntに負数を指定
		bool Draw(
			float x,
			float y,			
			size_t startCharCnt,
			int drawCharCnt,
			const char* s, ...
			);

		// 文字送りx指定領域内描画
		// note:最後の文字まで描画する時は，drawChCntに負数を指定
		bool DrawInRect(
			RectF &rect,
			TextAlign format,			
			size_t startCharCnt,
			int drawCharCnt,
			const char* s, ...
			);

		// 回転
		bool Draw(
			float x,
			float y,			
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const char* s, ...
			);

		// 回転x指定領域内へ文字描画
		bool DrawInRect(
			RectF &rect,
			TextAlign format,			
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const char* s, ...
			);


		// カスタム描画
		bool DrawInRect(
			RectF &rect,
			TextAlign format,			
			size_t startCharCnt,
			int drawCharCnt,
			size_t fontSize,
			float letterSpace,
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const char* s, ...
			);


		

	private:


		// 描画を管理する関数
		virtual bool DrawFont(
			RectF &rect,
			TextAlign format,		
			size_t startCharCnt,
			int drawCharCnt,
			size_t fontSize,
			float letterSpace,
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const char* s,
			va_list vlist
			);


		// フォントテクスチャを開放する
		bool DeleteAll();


		// 文字列strのoffset番目の文字から，長さlimitに1行で入る文字数を取得
		// 1行の長さは，lengthに書き込まれる
		// limit < 0.0fの場合，長さの指定を無視する
		int GetStrLength(const char* str, size_t offset, float limit, size_t &length);



	};






};
