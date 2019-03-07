#pragma once



// (c) 2018, Nanami Yamamoto



#include <atlbase.h>
#include <array>
#include <memory>

#include "RenderingTarget.hpp"
#include "StencilClip.hpp"


namespace dx9 {

	namespace renderer {


		// This class manage rendering such as depth-stencil buffer, rendering target.
		class RenderingManager : public Singleton<RenderingManager>, public stencil::StencilClip {

				
		public:
			friend class Singleton<RenderingManager>;

			// Create Rendering Target whose size [w]x[h]
			bool CreateRenderingTarget(IDirect3DDevice9 *device, RenderingTarget &rt, size_t w, size_t h);

			// Change current Rendering Target to [rt]. * "Clear" method is NOT called in this function*
			bool SetRenderingTarget(IDirect3DDevice9 *device, const RenderingTarget &rt);
			
			// Change Rendering Target to default one.
			bool SetRenderingTarget(IDirect3DDevice9 *device);

			// Release all Rendering Target.
			bool OnLostDevice(IDirect3DDevice9 *device);

			// Restore Rendering Target. note:This fanction will failed when device lost is being caused.
			bool OnResetDevice(IDirect3DDevice9 *device);

			// Get a texture from [rt].
			const texture::DXTextureBase* GetTexture(const RenderingTarget &rt);

			// Get a current RT. If it is default one, this function return nullptr.
			const RenderingTarget GetCurrentRT() { return currentUsrRT.lock(); };

			const RTContainer::ContainerPimpl *GetRTContainer(const RenderingTarget &rt) { return RenderingTargetFactory::GetInstance().GetContainer(rt); };

		protected:
			RenderingManager();
			~RenderingManager();

		private:
			
			std::unique_ptr<RTContainer::ContainerPimpl>	defaultRTcon;

			// maintain current Rendering Target created by user
			std::weak_ptr<renderer::RenderingTarget_class> currentUsrRT;


			bool GetDefaultRT(IDirect3DDevice9 *device, RTContainer::ContainerPimpl &rt);


		};

	}

}