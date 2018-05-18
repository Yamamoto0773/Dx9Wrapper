#pragma once



// (c) 2018, Nanami Yamamoto




#include <d3d9.h>
#include <atlbase.h>
#include <array>
#include <memory>

#include "RenderingTarget.hpp"
#include "StencilClip.hpp"
#include "LogManager.hpp"


namespace dx9 {

	namespace renderer {


		// This class manage rendering such as depth-stencil buffer, rendering target.
		class RenderingManager : public Singleton<RenderingManager>, public stencil::StencilClip {

				
		public:
			friend class Singleton<RenderingManager>;

			// Create Rendering Target whose size [w]x[h]
			bool CreateRenderingTarget(IDirect3DDevice9 *device, RenderingTarget &rt, size_t w, size_t h);

			// Change current Rendering Target to [rt]. * "Clear" method is NOT called in this function*
			bool SetRenderingTarget(IDirect3DDevice9 *device, RenderingTarget &rt);
			
			// Change Rendering Target to default one.
			bool SetRenderingTarget(IDirect3DDevice9 *device);

			// Release all Rendering Target.
			bool OnLostDevice(IDirect3DDevice9 *device);

			// Restore Rendering Target. note:This fanction will failed when device lost is being caused.
			bool OnResetDevice(IDirect3DDevice9 *device);

			// Get a texture from [rt].
			texture::DXTextureBase GetTexture(RenderingTarget &rt);

		protected:
			RenderingManager() {};
			~RenderingManager() {};

		private:
			
			RTContainer::Container		defaultRTcon;

			// maintain current Rendering Target created by user
			RenderingTarget *currentUsrRT;


			bool GetDefaultRT(IDirect3DDevice9 *device, RTContainer::Container &rt);


		};

	}

}