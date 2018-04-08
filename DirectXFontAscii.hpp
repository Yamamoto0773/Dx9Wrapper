#pragma once

#include "FontTexture.hpp"
#include "DXDrawManager.hpp"

// ASCII文字描画クラス


namespace dx9 {

	class DirectXFontAscii : public DXDrawManager {

		static const int CHARACTER_MAXCNT = 512;	// 文字列の最大文字数


	private:
		HFONT					hFont;			// 設定中のフォントのハンドル
		AntialiasLevel			antialiasLv;

		TEXTMETRICA				tm;
		TextureFilter			texFilter;
		
	

		std::vector< std::unique_ptr<FontTextureA> > texRes;

		size_t fontSize;

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


		// 描画時のテクスチャの補完方法の指定 (ジャギーがかかる場合はこの関数で設定して下さい)
		// https://msdn.microsoft.com/ja-jp/library/cc324337.aspx
		void SetFontTextureFilter(TextureFilter filter) { texFilter = filter; };



		//////////////////////////////////////////////
		// 文字描画

		// シンプル描画
		bool Draw(
			float x,
			float y,
			DWORD color,
			const char* s, ...
			);

		// 指定領域内へ文字描画
		bool DrawInRect(
			RectF &rect,
			DWORD format,
			DWORD color,
			const char* s, ...
			);

		// 文字送り
		// note:最後の文字まで描画する時は，drawChCntに負数を指定
		bool Draw(
			float x,
			float y,
			DWORD color,
			size_t startCharCnt,
			int drawCharCnt,
			const char* s, ...
			);

		// 文字送りx指定領域内描画
		// note:最後の文字まで描画する時は，drawChCntに負数を指定
		bool DrawInRect(
			RectF &rect,
			DWORD format,
			DWORD color,
			size_t startCharCnt,
			int drawCharCnt,
			const char* s, ...
			);

		// 回転
		bool Draw(
			float x,
			float y,
			DWORD color,
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const char* s, ...
			);

		// 回転x指定領域内へ文字描画
		bool DrawInRect(
			RectF &rect,
			DWORD format,
			DWORD color,
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const char* s, ...
			);


		// カスタム描画
		bool DrawInRect(
			RectF &rect,
			DWORD format,
			DWORD color,
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
			DWORD format,
			DWORD color,
			size_t startCharCnt,
			int drawCharCnt,
			size_t fontSize,
			float letterSpace,
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const char* s,
			va_list vlist
			);

	

		// 与えられた描画領域とフォーマットから、最終的な描画座標を計算
		// [in] 文字列テクスチャの参照
		// [in] 描画する短形領域
		// [in] 拡大倍率
		// [in] 文字の間隔
		// [in] 配置フォーマット
		// [out] 文字の描画位置
		// [out] 文字列の描画範囲
		// [out] 拡大倍率 (inScale*outScaleが最終的な拡大倍率になります)
		int	CalcTextPosition(
			const std::string &str,
			RectF &rect,
			float inScale,
			int letterSpace,
			DWORD format,
			std::vector<PointF> &pt,
			RectF &strArea,
			float &outScale
			);


		// フォントテクスチャを開放する
		bool DeleteAll();



	};






};
