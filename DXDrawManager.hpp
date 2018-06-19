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
// http://marupeke296.com/DXG_No54_StencilClipping.html
// http://sylphylunar.seesaa.net/article/390331341.html
////////////////////////////////////////////////////



#include<d3dx9.h>


#include<memory>
#include<atlbase.h>
#include<vector>

#include "dx9.hpp"
#include "LogManager.hpp"
#include "RenderingManager.hpp"
#include "DX9ShareContainer.hpp"
#include "Figure.hpp"
#include "Singleton.hpp"
#include "NonCopyable.hpp"
#include "Texture.hpp"


namespace dx9 {

	class DXDrawManager : 
		private resource::DX9ShareContainer,
		public Singleton<DXDrawManager>,
		public Noncopyable {
		
		friend class Singleton<DXDrawManager>;

		dx9::renderer::RenderingManager *renderMng;

		RenderingTarget textureRT;
		
		static size_t topLayerPos;
	

	public:

		// DirectXインターフェース群の生成
		bool Create(HWND hwnd, Size size, MultiSampleLv level=MultiSampleLv::_4SAMPLES, bool isRightHand=false);


		// 描画開始
		bool DrawBegin(bool isClear);

		// 描画終了
		bool DrawEnd();


		bool ClearDrawingTarget();

		// ---------------------------------------
		// 図形の描画

		// 直線描画
		bool DrawLine(PointF &begin, PointF &end, Color & color, float lineWidth=1.0f);
		bool DrawLine(float begin_x, float begin_y, float end_x, float end_y, Color & color, float lineWidth=1.0f);

		// 長方形の輪郭の描画
		bool DrawRectFrame(RectF &rect, Color & color, float lineWidth=1.0f);
		bool DrawRectFrame(PointF &topLeft, PointF &bottomRight, Color & color, float lineWidth=1.0f);
		bool DrawRectFrame(float x, float y, float w, float h, Color & color, float lineWidth=1.0f);

		// 長方形の描画
		bool DrawRect(RectF &rect, Color & color);
		bool DrawRect(PointF &topLeft, PointF &bottomRight, Color & color);
		bool DrawRect(float x, float y, float w, float h, Color & color);


		// 円の輪郭の描画
		bool DrawCircleFrame(float x, float y, float w, float h, Color & color, float lineWidth=1.0f);
		// rectArea... circle-frame will be postioned inside to come contact with [rectArea].
		bool DrawCircleFrame(RectF &rectArea, Color & color, float lineWidth=1.0f); 

		// 円の描画
		bool DrawCircle(float x, float y, float w, float h, Color & color);
		// rectArea... circle will be postioned inside to come contact with [rectArea].
		bool DrawCircle(RectF &rectArea, Color & color);	




		// ---------------------------------------
		// 描画の設定を行う関数

		// 描画をクリアする時の色(背景色)の指定 (各色成分は0-255の範囲で指定)
		void SetBackGroundColor(size_t r, size_t g, size_t b);
		// 描画をクリアする時の色(背景色)の指定 (rgbを24bitで指定)
		void SetBackGroundColor(unsigned long rgb);

		// ブレンドモードの設定
		void SetBlendMode(BLENDMODE mode);

		// テクスチャサンプラの設定
		// note:テクスチャにジャギがかかる場合に設定してください
		void SetTextureSamplerState(TextureFilter mode);

		
		// ------------------------------------------
		// マスク描画を管理する関数

		// マスクの作成スタート
		bool CreateMaskBegin();

		// マスクの作成終了
		bool CreateMaskEnd();

		// 作成したマスクを適用
		bool SetMask();
		// 短形領域のマスクを適用
		bool SetRectMask(RectF maskArea);
		// 円形のマスクを適用
		bool SetCircleMask(RectF maskArea);
		// 円形のマスクを適用
		bool SetCircleMask(float x, float y, float w, float h);
		// マスク内容のクリア
		void ClearMask();

		// マスクを解除
		bool RemoveMask();

		// マスクの種類を変更
		void SetMaskType(MaskType type = MaskType::DrawableMask);
		MaskType GetMaskType();


		// ---------------------------------------
		// レンダリングターゲット関係

		// レンダリングターゲットの作成
		bool CreateRenderingTarget(RenderingTarget &rt, size_t w, size_t h);

		// レンダリングターゲットの切替
		bool SetRenderingTarget(const RenderingTarget &rt);

		// レンダリングターゲットからテクスチャ取得
		Texture GetTextureFromRT(const RenderingTarget & rt);


		// レンダリングターゲットをデフォルトに戻す
		bool ResetRenderingTarget();
	
		// ---------------------------------------

		// ログの書き込み先の指定
		void SetLogWriteDest(LogManager* dest);


		// ---------------------------------------
		// Direct3D9オブジェクトの生ポインタ取得
		const IDirect3D9* _GetDirect3D9() { return d3d9.p; }
		// Direct3DDevice9オブジェクトの生ポインタ取得
		const IDirect3DDevice9* _GetDirect3DDev9() const { return d3ddev9.p; }
		

	private:
		DXDrawManager();
		virtual ~DXDrawManager();
	
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

		bool DeviceLost();
		bool DeviceReset();
	};

}

