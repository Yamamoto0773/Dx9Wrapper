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


#include "dx9.hpp"
#include "LogManager.hpp"
#include "RenderingManager.hpp"
#include "DX9ShareContainer.hpp"
#include "Figure.hpp"
#include "Singleton.hpp"
#include "NonCopyable.hpp"
#include "Texture.hpp"
#include "Window.hpp"


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
		bool Create(const WindowPimpl* const window, Size size, MultiSampleLv level=MultiSampleLv::_4SAMPLES, bool isRightHand=false);


		// 描画開始
		bool DrawBegin(bool isClear);

		// 描画終了
		bool DrawEnd();


		bool ClearRenderingTarget();

		// ---------------------------------------
		// 図形の描画

		// 直線描画
		bool DrawLine(const PointF& begin, const PointF& end, const Color & color, float lineWidth=1.0f);
		bool DrawLine(float begin_x, float begin_y, float end_x, float end_y, const Color & color, float lineWidth=1.0f);

		// 長方形の輪郭の描画
		bool DrawRectFrame(const RectF& rect, const Color & color, float lineWidth=1.0f);
		bool DrawRectFrame(const PointF& topLeft, const PointF& bottomRight, const Color & color, float lineWidth=1.0f);
		bool DrawRectFrame(float x, float y, float w, float h, const Color & color, float lineWidth=1.0f);

		// 長方形の描画
		bool DrawRect(const RectF& rect, const Color & color);
		bool DrawRect(const PointF& topLeft, const PointF& bottomRight, const Color & color);
		bool DrawRect(float x, float y, float w, float h, const Color & color);


		// 円の輪郭の描画
		bool DrawCircleFrame(float x, float y, float w, float h, const Color & color, float lineWidth=1.0f);
		// 円の輪郭の描画
		// reatAreaで指定した矩形領域内にピッタリ収まる円の輪郭を描画します
		bool DrawCircleFrame(const RectF& rectArea, const Color & color, float lineWidth=1.0f); 

		// 円の描画
		bool DrawCircle(float x, float y, float w, float h, const Color & color);
		// reatAreaで指定した矩形領域内にピッタリ収まる円を描画します
		bool DrawCircle(const RectF& rectArea, const Color & color);	




		// ---------------------------------------
		// 描画の設定を行う関数

		// 描画をクリアする時の色(背景色)の指定
		void SetClearColor(const Color& color);

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
		bool SetRectMask(const RectF& maskArea);
		// 円形のマスクを適用
		bool SetCircleMask(const RectF& maskArea);
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
			const WindowPimpl * const window,
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

