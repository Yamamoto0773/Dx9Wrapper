#pragma once

// (c) 2018, Nanami Yamamoto




#include <d3d9.h>
#include <atlbase.h>
#include <array>
#include <memory>
#include <functional>

#include "Singleton.hpp"
#include "NonCopyable.hpp"

#include "DXTextureBase.hpp"
#include "LogManager.hpp"

namespace dx9 {

	namespace renderer {
		class RenderingTarget_;
		class RenderingTargetFactory;


		constexpr unsigned RTRESOURCE_MAXCNT = 10;



		struct RTContainer {
			unsigned user;
			D3DSURFACE_DESC surfaceDesc;
			struct Container {
				texture::DXTextureBase texture;
				CComPtr<IDirect3DSurface9> texSurface, depthStencilBuffer;
				D3DVIEWPORT9 viewPort;
			}res;
		};


		// This class is used to reference Rendering Target maintained in its factory.
		// User can manage ostensibly RT-resource with this class.
		class RenderingTarget_ : Noncopyable {
			friend class RenderingTargetFactory;

		private:
			RenderingTarget_();
			~RenderingTarget_();
			bool isValid;
			unsigned index;
		};



		// This class manage to create and destroy Rendering Target.
		class RenderingTargetFactory : public Singleton<RenderingTargetFactory> {
			friend class Singleton<RenderingTargetFactory>;

		public:
			
			using RT_deleter_t = std::function<void(RenderingTarget_*)>;
			using RenderingTarget_t = std::unique_ptr<RenderingTarget_, RT_deleter_t>;
			using RTpool_t = std::array<RTContainer, RTRESOURCE_MAXCNT>;

		public:

			// Create Rendering Target 
			RenderingTarget_t CreateRT(IDirect3DDevice9 *device, size_t w, size_t h);

			// Release all RT-resources created by user.
			void ReleaseAll();

			// Restore all RT-resources created by user.
			bool Restore(IDirect3DDevice9 *device);

			// Get RT-resources specified with [rt]. If [rt] is invalid, return nullptr.
			const RTContainer::Container* GetContainer(const RenderingTarget_t &rt);


		protected:
			RenderingTargetFactory() {};
			~RenderingTargetFactory() {};



		private:
			// search empty element in RTpool, and return index number indicated it.
			// If not find it, return -1.
			int searchEmptyRTPool();

			
			bool CreateRTContainer(
				IDirect3DDevice9			*device,
				size_t						w,
				size_t						h,
				RTContainer::Container		&container
				);


			// maintain RT-resource and number of user
			std::array<RTContainer, RTRESOURCE_MAXCNT> RTpool;

		};

	
	}

	using RenderingTarget = renderer::RenderingTargetFactory::RenderingTarget_t;
}