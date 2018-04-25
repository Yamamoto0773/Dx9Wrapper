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

#include "dx9.hpp"
#include "DXTextureManager.hpp"
#include "LogManager.hpp"



namespace dx9 {

	class DXDrawManager : private resource::DX9ShareContainer {

		texture::DXTextureManager texMng;

		static size_t topLayerPos;
	
	private:
		// コピー不可
		DXDrawManager(const DXDrawManager&) = delete;	
		DXDrawManager& operator=(const DXDrawManager&) = delete;
		

	public:

		DXDrawManager();
		virtual ~DXDrawManager();

		// DirectXインターフェース群の生成
		bool Create(HWND hwnd, Size size, MultiSampleLv level=MultiSampleLv::_4SAMPLES, bool isRightHand=false);

		// 描画開始
		bool DrawBegin(bool isClear);

		// 描画終了
		bool DrawEnd();




		// ---------------------------------------
		// テクスチャの管理などを行う関数

		// 画像ファイルからテクスチャを作成
		bool CreateFromFile(
			Texture &tex,
			const std::wstring& fileName
			);

		// 画像ファイルからトリミングしてテクスチャを作成
		bool CreateFromFile(
			Texture &tex,
			const std::wstring& fileName,
			size_t x,
			size_t y,
			size_t w,
			size_t h
			);

		// 空のテクスチャを作成
		bool CreateEmptyTex(
			Texture &tex,
			size_t w,
			size_t h
			);

		// 使用されていないテクスチャを削除
		// 戻り値:解放したテクスチャの数
		int CleanTexPool();


		// ---------------------------------------
		// 描画を行う関数
		bool DrawTexture(
			Texture &tex,
			float x,
			float y,
			DrawTexCoord coord=DrawTexCoord::TOP_L,
			float alpha=1.0f,
			float xscale=1.0f,
			float yscale=1.0f,
			int rotDeg=0
			);


		// ---------------------------------------
		// 描画の設定を行う関数

		// 描画をクリアする時の色(背景色)の指定 (各色成分は0-255の範囲で指定)
		void SetBackGroundColor(size_t r, size_t g, size_t b);
		// 描画をクリアする時の色(背景色)の指定 (rgbを24bitで指定)
		void SetBackGroundColor(unsigned long rgb);

		// ブレンドモードの設定
		void SetBlendMode(BLENDMODE mode);

		void SetTexFilter(TextureFilter mode);

		// ---------------------------------------

		// ログの書き込み先の指定
		void SetLogWriteDest(LogManager* dest);


		// ---------------------------------------
		// Direct3D9オブジェクトの生ポインタ取得
		const IDirect3D9* _GetDirect3D9() { return d3d9.p; }
		// Direct3DDevice9オブジェクトの生ポインタ取得
		const IDirect3DDevice9* _GetDirect3DDev9() const { return d3ddev9.p; }
		

	private:
		
	
		bool Create(
			HWND hwnd,
			bool isfull,
			size_t w,
			size_t h,
			MultiSampleLv level,
			bool isRightHand
			);

		void Clear();
		
		void Delete();

		bool DeviceReset();
	};

}

