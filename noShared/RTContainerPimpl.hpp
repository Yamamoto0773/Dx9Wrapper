#pragma once

#include <d3dx9.h>

#include <atlbase.h>
#include <memory>

#include "NonCopyable.hpp"
#include "TextureBase.hpp"


namespace dx9 {
	namespace renderer {

		struct RTContainerPimpl : public Noncopyable {
			unsigned user;
			D3DSURFACE_DESC surfaceDesc;
		};

		struct RTContainer::ContainerPimpl {
			dx9::texture::TextureBase texture;
			CComPtr<IDirect3DSurface9> texSurface, depthStencilBuffer;
			D3DVIEWPORT9 viewPort;
		};

	}
}
