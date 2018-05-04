#pragma once

#include <d3dx9.h>
#include <atlbase.h>

#include "LogManager.hpp"
#include "dx9.hpp"



namespace dx9 {

	namespace resource {

		constexpr unsigned CIRCLE_VERTEXCNT = 128; // the number of vertex for circle vertex buffer

		class DX9ShareContainer {
		protected:

			static bool isResCreated;
			static size_t topLayerPos;

			static CComPtr<IDirect3D9>			d3d9;
			static CComPtr<IDirect3DDevice9>	d3ddev9;

			static D3DCAPS9						d3dcaps9;

			// DirectXを初期化する時に使う構造体
			// どんな値を設定したか取っておいた方がいいので、メンバ変数とする。
			static D3DPRESENT_PARAMETERS				d3dpresent;

			static CComPtr<IDirect3DVertexBuffer9>		vertex_rect;		// 頂点バッファ
			static CComPtr<IDirect3DVertexBuffer9>		vertex_circle;

			static CComPtr<ID3DXEffect>					effect;		// シェーダ

			static CComPtr<IDirect3DVertexDeclaration9>	verDecl;	// 頂点宣言


			// 現在の状態を保持する変数
			static D3DXMATRIX projMat;

			static BLENDMODE blendMode;
			static bool isDrawStarted;
			static bool isLost;
			static bool isRightHand;
			static unsigned long backGroundColor;
			static TextureFilter texFilter;

			static LogManager* log;

			// 現在描画可能か返す
			bool isDrawable() { return (isDrawStarted && (!isLost) && isResCreated); };

		public:

		};

	}


}
