#pragma once

// 2017/12/15
// (c) 2017, NanamiYamamoto

////////////////////////////////////////////////////
// DirectX9.0を用いた描画を管理するクラス
// 参考:
// http://www.charatsoft.com/develop/otogema/
// http://marupeke296.com/DXG_Init.html
// http://marupeke296.com/DXGSmp_No1_Init.html
// http://marupeke296.com/NGDV_No3_Sprite.html
// http://sylphylunar.seesaa.net/article/390331341.html
////////////////////////////////////////////////////



#include<d3dx9.h>


#include<memory>
#include<atlbase.h>
#include<vector>


#include "DirectXFont.hpp"
#include "DXTextANSI.h"


namespace dxFont {
	class DirectXFont;
	struct PointF;
	struct RectF;
}



namespace drawMng {

	static const int TEXTURERES_MAXCNT = 1024;	// 作成可能なリソース数
	static const int TEXTURE_MAXCNT = 1024;	// 登録可能なテクスチャ数

	// ブレンドモードを指定する時の列挙体
	enum class BLENDMODE {
		// 通常合成
		// 結果色 = 合成色x合成色.alpha + 背景色x(1-合成色.alpha)
		NORMAL = 0,

		// 加算合成 (光の表現などに用いる)
		// 結果色 = 合成色x合成色.alpha + 背景色x1
		ADD
	};


	// 描画時に座標を指定するときのオプション
	enum class DRAWTEX_COORD {
		TOP_L = 0,
		TOP_R,
		BOTTOM_R,
		BOTTOM_L,
		CENTER
	};


	struct Size {
		size_t w;
		size_t h;
	};

	struct UVCoord {
		float left;
		float top;
		float w;
		float h;
	};

	


	// 描画時の設定
	struct DrawFormat {
		IDirect3DTexture9 *tex;	// テクスチャへのポインタ
		struct Pos {
			float x, y;
		}pos;			// 描画位置(左上)
		struct Pos2 {
			float x, y;
		}origin;		// 原点
		Size size;			// テクスチャサイズ
		UVCoord uv;			// uv座標
		float rotRad;		// 回転角度(ラジアン)
		float xscale;		// x方向の倍率
		float yscale;		// y方向の倍率
		struct Color {
			float r;
			float g;
			float b;
			float a;
		}color;				// 色R,G,B,A(0.0f - 1.0f)
		BLENDMODE blendMode;	// ブレンドモード
	};



	class DXDrawManager : public std::enable_shared_from_this<DXDrawManager> {
		
		static std::shared_ptr<DXDrawManager> instance;

	private:
		CComPtr<IDirect3D9>			d3d9;
		CComPtr<IDirect3DDevice9>	d3ddev9;

		D3DCAPS9									d3dcaps9;

		// DirectXを初期化する時に使う構造体
		// どんな値を設定したか取っておいた方がいいので、メンバ変数とする。
		D3DPRESENT_PARAMETERS						d3dpresent;

		CComPtr<IDirect3DVertexBuffer9>		vertex;
		CComPtr<ID3DXEffect>				effect;		// シェーダ
		CComPtr<IDirect3DVertexDeclaration9>verDecl;	// 頂点宣言

	private:
		// 画像ファイル用のテクスチャ
		std::vector<std::unique_ptr<TextureFile>> texRes;
		// 切り抜き情報
		struct TexClip {
			bool isValid;
			size_t resourceID;
			Size size;
			UVCoord uv;
		}texClip[TEXTURE_MAXCNT];
		
	private:
		// 現在の状態を保持する変数
		BLENDMODE blendMode;
		bool isDrawStarted;
		bool isLost;
		bool isRightHand;
		unsigned long backGroundColor;
		


	private:
		// 描画キュー
		static std::vector<DrawFormat> drawQueue;

		static LogManager* log;


	public:
		~DXDrawManager();
	private:
		// コピー不可
		DXDrawManager(const DXDrawManager&) = delete;	
		DXDrawManager& operator=(const DXDrawManager&) = delete;
		DXDrawManager();
	protected:
		

	public:
		

		static bool CreateFull(
			HWND hwnd
			);

		// インスタンス化用の関数
		// ウィンドウver.
		static bool CreateWind(
			HWND hwnd,
			size_t screenW,
			size_t screenH
			);

		
		static DXDrawManager& GetInstance() { return *(instance.get()); };


		// ---------------------------------------
		// テクスチャの管理などを行う関数

		// 画像ファイルからテクスチャを登録
		bool AddTexture(
			size_t texID,
			const std::string& fileName
			);

		// 画像ファイルからトリミングしてテクスチャを登録
		bool AddTexture(
			size_t texID,
			const std::string& fileName,
			size_t x,
			size_t y,
			size_t w,
			size_t h
			);
	

		// テクスチャの削除
		bool DelTexture(size_t texID);

		// ---------------------------------------
		// 描画を行う関数

		// テクスチャを描画
		bool DrawAll();

		// 前回までの描画をクリア
		bool ClearBackGround();

		// 描画開始
		bool DrawBegin();

		// 描画終了
		bool DrawEnd();



		// 指定番号のテクスチャを描画対象に追加
		bool DrawTexture(
			size_t texID,
			float x,
			float y, 
			DRAWTEX_COORD coord,
			float alpha=1.0f
			);
		bool DrawTexture(
			size_t texID,
			float x,
			float y, 
			DRAWTEX_COORD coord,
			float xscale=1.0f,
			float yscale=1.0f, 
			float alpha=1.0f,
			int rotDeg=0
			);
		

		void AddDrawQueue(DrawFormat fmt) { drawQueue.push_back(fmt); };

		// ---------------------------------------
		// 描画の設定を行う関数

		// 座標の指定方法の切替　右手系/左手系
		void SetHand(bool isRight);

		// 描画をクリアする時の色(背景色)の指定 (各色成分は0-255の範囲で指定)
		void SetBackGroundColor(size_t r, size_t g, size_t b);
		// 描画をクリアする時の色(背景色)の指定 (rgbを24bitで指定)
		void SetBackGroundColor(unsigned long rgb);

		// 描画時のテクスチャの補完方法の指定 (ジャギーがかかる場合はこの関数で設定して下さい)
		// https://msdn.microsoft.com/ja-jp/library/cc324337.aspx
		void SetTextureFilter(D3DTEXTUREFILTERTYPE type);

		// ブレンドモードの設定
		void SetBlendMode(BLENDMODE mode);


		// ---------------------------------------
		// 拡張機能

		// フォントの作成
		std::unique_ptr<dxFont::DirectXFont> CreateDirectXFont(
			const wchar_t*	fontName,
			size_t			fontSize,
			dxFont::FontWeight	fontWeight = dxFont::FontWeight::NORMAL,
			bool			isItalic = false,
			bool			isUnderLine = false,
			bool			isStrikeOut = false,
			AntialiasLevel	level = AntialiasLevel::_15STEPS
			);



		// ---------------------------------------

		// ログの書き込み先の指定
		static void SetLogWriteDest(LogManager* dest);


		// ---------------------------------------
		// Direct3D9オブジェクトの生ポインタ取得
		IDirect3D9* _GetDirect3D9() { return d3d9.p; }
		// Direct3DDevice9オブジェクトの生ポインタ取得
		IDirect3DDevice9* _GetDirect3DDev9() const { return d3ddev9.p; }
		

	private:
		
	
		bool Create(
			HWND hwnd,
			bool isfull,
			size_t w,
			size_t h
			);

		void Clear();
		void Delete();

		int GetTexResourceID(const std::string& fileName);

	};

}

