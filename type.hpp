#pragma once

#include <functional>

#include <memory>

namespace dx9 {

	namespace type {

		
		using D3D9_t			= void;
		using D3DDev9_t			= void;
		using D3DVtxBuf9_t		= void;
		using D3DEffect_t		= void;
		using D3DVtxDecl9_t		= void;
		using D3DTex9_t			= void;
		using D3DSurface9_t		= void;


		using D3DViewPort9_t	= void;

		using HWND_t = void*;
		using HFONT_t = void*;
		using HDC_t = void*;
		
		using TextMetricA_t = void;
		using TextMetricW_t = void;
		
		using D3DCaps9_t = void;

		using D3DMatrix_t = void; 

		using D3DPresentParam_t = void;

		using LockedRect_vptr = void*;

		using D3DSurfaceDesc_t = void;

		using D3DVec2_t	= void;


		

		using d3dinterface_deleter_t = std::function<void(void *)>;
		using vptr_deleter_t = std::function<void(void*)>;


		template<typename T>
		using vcomptr_unique_t = std::unique_ptr<T, d3dinterface_deleter_t>; 

		template<typename T>
		using vptr_unique_t = std::unique_ptr<T, vptr_deleter_t>;

		template<typename T>
		using vcomptr_shared_t = std::shared_ptr<T>;
		
		template<typename T>
		using vptr_shared_t = std::shared_ptr<T>;

		

	}

}