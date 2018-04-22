#pragma once


#include "FontTexture.hpp"
#include "DXDrawManager.hpp"


namespace dx9 {

	static const int CHARACTER_MAXCNT = 512;	// 文字列の最大文字数



	// 文字列描画クラス
	class DirectXFont : private DXDrawManager {

	private:
		HFONT					hFont;			// 設定中のフォントのハンドル
		AntialiasLevel			antialiasLv;

		TEXTMETRICW				tm;
		TextureFilter			texFilter;

	
		std::vector< std::unique_ptr<texture::FontTextureW> > texRes;

		size_t fontSize;

	public:
		DirectXFont();
		~DirectXFont();


		virtual bool Create(
			const wchar_t*	fontName,
			size_t			fontSize,
			FontWeight		fontWeight = FontWeight::NORMAL,
			bool			isItalic = false,
			bool			isUnderLine = false,
			bool			isStrikeOut = false,
			AntialiasLevel	level = AntialiasLevel::_15STEPS
			);


		// 文字列テクスチャを明示的に作成
		bool StoreFontTex(const std::wstring& wstr);
	

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
			const wchar_t* s, ...
			);

		// 指定領域内へ文字描画
		bool DrawInRect(
			RectF &rect,
			DWORD format,
			DWORD color,
			const wchar_t* s, ...
			);

		// 文字送り
		// note:最後の文字まで描画する時は，drawChCntに負数を指定
		bool Draw(
			float x,
			float y,
			DWORD color,
			size_t startCharCnt,
			int drawCharCnt,
			const wchar_t* s, ...
			);

		// 文字送りx指定領域内描画
		// note:最後の文字まで描画する時は，drawChCntに負数を指定
		bool DrawInRect(
			RectF &rect,
			DWORD format,
			DWORD color,
			size_t startCharCnt,
			int drawCharCnt,
			const wchar_t* s, ...
			);

		// 回転
		bool Draw(
			float x,
			float y,
			DWORD color,
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const wchar_t* s, ...
			);

		// 回転x指定領域内へ文字描画
		bool DrawInRect(
			RectF &rect,
			DWORD format,
			DWORD color,
			float rotateDeg,
			FontRotOrigin rotOrigin,
			const wchar_t* s, ...
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
			const wchar_t* s, ...
			);


		// フォントテクスチャを開放する (デストラクタで自動的に呼び出されます。明示的に呼び出す必要はありません。
		bool Delete(unsigned int code);
		bool DeleteAll();


	

		

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
			const wchar_t* s,
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
			const std::wstring &str,
			RectF &rect,
			float inScale,
			int letterSpace,
			DWORD format,
			std::vector<PointF> &pt,
			RectF &strArea,
			float &outScale
			);

	};






}