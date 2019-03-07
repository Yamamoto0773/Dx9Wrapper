#pragma once

// (c) 2018, Nanami Yamamoto



#include <atlbase.h>
#include <array>
#include <memory>

#include "Singleton.hpp"
#include "NonCopyable.hpp"

#include "DXTextureBase.hpp"
#include "RTContainer.hpp"

namespace dx9 {

	namespace renderer {
		class RenderingTarget_class;
		class RenderingTargetFactory;

		constexpr unsigned RTRESOURCE_MAXCNT = 10;


		// This class is used to reference Rendering Target maintained in its factory.
		// User can manage ostensibly RT-resource with this class.
		class RenderingTarget_class : Noncopyable {
			friend class RenderingTargetFactory;

		private:
			bool isValid;
			unsigned index;
			RenderingTarget_class();
			~RenderingTarget_class();
		};



		

		// This class manage to create and destroy Rendering Target.
		class RenderingTargetFactory : public Singleton<RenderingTargetFactory> {
			friend class Singleton<RenderingTargetFactory>;

		public:
			
			using RenderingTarget_sptr = std::shared_ptr<renderer::RenderingTarget_class>;
			using RTpool_t = std::array<RTContainer, RTRESOURCE_MAXCNT>;

		public:

			// Create Rendering Target 
			RenderingTarget_sptr CreateRT(IDirect3DDevice9 *device, size_t w, size_t h);

			// Release all RT-resources created by user.
			void ReleaseAll();

			// Restore all RT-resources created by user.
			bool Restore(IDirect3DDevice9 *device);

			// Get RT-resources specified with [rt]. If [rt] is invalid, return nullptr.
			const RTContainer::ContainerPimpl* GetContainer(const RenderingTarget_sptr &rt);


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
				RTContainer::ContainerPimpl	&container
				);


			// maintain RT-resource and number of user
			std::array<RTContainer, RTRESOURCE_MAXCNT> RTpool;


		};

		
	}


	using RenderingTarget = renderer::RenderingTargetFactory::RenderingTarget_sptr;
	
}
