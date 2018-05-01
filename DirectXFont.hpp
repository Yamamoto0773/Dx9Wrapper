#pragma once


#include "FontTexture.hpp"
#include "DX9ShareContainer.hpp"
#include <array>


namespace dx9 {

	// 文字列描画クラス
	class DirectXFont : resource::DX9ShareContainer {

		static const int CHARACTER_MAXCNT = 1024;	// 文字列の最大文字数

	private:
		HFONT					hFont;			// 設定中のフォントのハンドル
		AntialiasLevel			antialiasLv;

		TEXTMETRICW				tm;
	
		std::vector<std::unique_ptr<texture::FontTextureW>> texRes;

		std::array<wchar_t, CHARACTER_MAXCNT+1> workBuf;
		

		size_t fontSize;

		float fontColor[4];

	public:
		DirectXFont();
		~DirectXFont();


		virtual bool Create(
			const std::wstring&	fontName,
			size_t			fontSize,
			FontWeight		fontWeight = FontWeight::NORMAL,
			bool			isItalic = false,
			bool			isUnderLine = false,
			bool			isStrikeOut = false,
			AntialiasLevel	level = AntialiasLevel::_15STEPS
			);


		// 文字列テクスチャを明示的に作成
		bool StoreFontTex(const wchar_t* wstr);
	
		// ARGBの順
		void SetFontColor(DWORD &color);

		// 0-255の成分指定
		void SetFontColor(size_t r, size_t g, size_t b, size_t a=255);

		// 0.0f-1.0fの成分指定
		void SetFontColor(float r, float g, float b, float a);

		//////////////////////////////////////////////
		// 文字描画

		// シンプル描画
		bool Draw(
			float x,
			float y,			
			const wchar_t* s, ...
			);

		// 指定領域内へ文字描画
		bool DrawInRect(
			RectF &rect,
			TextAlign format,			
			const wchar_t* s, ...
			);

		// 文字送り
		// note:最後の文字まで描画する時は，drawChCntに負数を指定
		bool Draw(
			float x,
			float y,			
			size_t startCharCnt,
			int drawCharCnt,
			const wchar_t* s, ...
			);

		// 文字送りx指定領域内描画
		// note:最後の文字まで描画する時は，drawChCntに負数を指定
		bool DrawInRect(
			RectF &rect,
			TextAlign format,			
			size_t startCharCnt,
			int drawCharCnt,
			const wchar_t* s, ...
			);

		// 回転
		bool Draw(
			float x,
			float y,			
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const wchar_t* s, ...
			);

		// 回転x指定領域内へ文字描画
		bool DrawInRect(
			RectF &rect,
			TextAlign format,			
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const wchar_t* s, ...
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
			const wchar_t* s, ...
			);


		// フォントテクスチャを開放する (デストラクタで自動的に呼び出されます。明示的に呼び出す必要はありません。
		bool Delete(unsigned int code);
		bool DeleteAll();


	

		

	private:

		// 描画を管理する関数
		bool DrawFont(
			RectF &rect,
			TextAlign format,
			size_t startCharCnt,
			int drawCharCnt,
			size_t fontSize,
			float letterSpace,
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const wchar_t* s,
			va_list vlist
			);


	

		// 文字列strのoffset番目の文字から，長さlimitに1行で入る文字数を取得
		// 1行の長さは，lengthに書き込まれる
		// limit < 0.0fの場合，長さの指定を無視する
		int GetStrLength(const wchar_t* str, size_t offset, float limit, size_t &length);


	};






}