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
#include<list>

#include "dx9.hpp"
#include "LogManager.hpp"



namespace dx9 {

	// ブレンドモードを指定する時の列挙体
	enum class BLENDMODE {
		// 通常合成
		// 結果色 = 合成色x合成色.alpha + 背景色x(1-合成色.alpha)
		NORMAL = 0,

		// 加算合成 (光の表現などに用いる)
		// 結果色 = 合成色x合成色.alpha + 背景色x1
		ADD
	};


	// フルシーンマルチサンプリングのレベル
	enum class MultiSampleLv : DWORD {
		NONE = 0,
		_2SAMPLES = 2,
		_4SAMPLES = 4,
		_6SAMPLES = 6,
		_8SAMPLES = 8,
		_12SAMPLES = 12,
		_16SAMPLES = 16
	};


	

	namespace shader {

		enum class ShaderPass : UINT {
			Color = 0,
			Tex,
			Mul_Tex_Color,
			Mul_ColorAlpha_TexAlpha,
			Mul_ColorAlpha_InvTexAlpha,
			Mul_UVTex_color,
			Mul_ColorAlpha_InvUVTexAlpha
		};

	}



	class DXDrawManager {

		static bool isResCreated;
		static size_t topLayerPos;
		
	protected:
		static CComPtr<IDirect3D9>			d3d9;
		static CComPtr<IDirect3DDevice9>	d3ddev9;

		static D3DCAPS9						d3dcaps9;

		// DirectXを初期化する時に使う構造体
		// どんな値を設定したか取っておいた方がいいので、メンバ変数とする。
		static D3DPRESENT_PARAMETERS				d3dpresent;

		static CComPtr<IDirect3DVertexBuffer9>		vertex;		// 頂点バッファ
	
		static CComPtr<ID3DXEffect>					effect;		// シェーダ

		static CComPtr<IDirect3DVertexDeclaration9>	verDecl;	// 頂点宣言

	

		// 現在の状態を保持する変数
		static D3DXMATRIX projMat;

		static BLENDMODE blendMode;
		static bool isDrawStarted;
		static bool isLost;
		static bool isRightHand;
		static unsigned long backGroundColor;
		
		static LogManager* log;
	
	private:
		// コピー不可
		DXDrawManager(const DXDrawManager&) = delete;	
		DXDrawManager& operator=(const DXDrawManager&) = delete;
		
	protected:
		DXDrawManager();
		virtual ~DXDrawManager();

	public:
		
		// ウィンドウ生成用の関数
		// フルスクリーンver.
		bool CreateFull(
			HWND hwnd,
			MultiSampleLv level=MultiSampleLv::_4SAMPLES,
			bool isRightHand=false
			);

		// ウィンドウ生成用の関数
		// ウィンドウver.
		bool CreateWind(
			HWND hwnd,
			size_t screenW,
			size_t screenH,
			MultiSampleLv level=MultiSampleLv::_4SAMPLES,
			bool isRightHand=false
			);

		
		bool isDrawable() { return (isDrawStarted && (!isLost) && isResCreated); };


		// ---------------------------------------
		// 描画を行う関数

		// 前回までの描画をクリア
		bool ClearBackGround();

		// 描画開始
		bool DrawBegin();

		// 描画終了
		bool DrawEnd();



		// ---------------------------------------
		// 描画の設定を行う関数

		// 描画をクリアする時の色(背景色)の指定 (各色成分は0-255の範囲で指定)
		void SetBackGroundColor(size_t r, size_t g, size_t b);
		// 描画をクリアする時の色(背景色)の指定 (rgbを24bitで指定)
		void SetBackGroundColor(unsigned long rgb);

		// ブレンドモードの設定
		void SetBlendMode(BLENDMODE mode);

		// ---------------------------------------

		// ログの書き込み先の指定
		static void SetLogWriteDest(LogManager* dest);


		// ---------------------------------------
		// Direct3D9オブジェクトの生ポインタ取得
		const IDirect3D9* _GetDirect3D9() { return d3d9.p; }
		// Direct3DDevice9オブジェクトの生ポインタ取得
		const IDirect3DDevice9* _GetDirect3DDev9() const { return d3ddev9.p; }
		

	protected:

		// 描画の前後位置を管理する関数
		float GetTopLayerPos() { return topLayerPos; };

		// 1つ上のレイヤーに変更
		void ChangeLayer() { topLayerPos++; };


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
	};

}

