#include "DX9ShareContainer.hpp"

namespace dx9 {

	namespace resource {

		bool									DX9ShareContainer::isResCreated = false;
		size_t									DX9ShareContainer::topLayerPos = 0;
		LogManager*								DX9ShareContainer::log;
		CComPtr<IDirect3D9>						DX9ShareContainer::d3d9;
		CComPtr<IDirect3DDevice9>				DX9ShareContainer::d3ddev9;
		D3DCAPS9								DX9ShareContainer::d3dcaps9;
		D3DPRESENT_PARAMETERS					DX9ShareContainer::d3dpresent;
		CComPtr<IDirect3DVertexBuffer9>			DX9ShareContainer::vertex_rect;
		CComPtr<IDirect3DVertexBuffer9>			DX9ShareContainer::vertex_circle;
		CComPtr<ID3DXEffect>					DX9ShareContainer::effect;		// シェーダ
		CComPtr<IDirect3DVertexDeclaration9>	DX9ShareContainer::verDecl;	// 頂点宣言
		D3DXMATRIX								DX9ShareContainer::projMat;
		BLENDMODE								DX9ShareContainer::blendMode = BLENDMODE::NORMAL;
		bool									DX9ShareContainer::isDrawStarted = false;
		bool									DX9ShareContainer::isLost = false;
		bool									DX9ShareContainer::isRightHand = false;
		unsigned long							DX9ShareContainer::backGroundColor = 0xffffff;
		TextureFilter							DX9ShareContainer::texFilter = TextureFilter::LINEAR;
	}

}

