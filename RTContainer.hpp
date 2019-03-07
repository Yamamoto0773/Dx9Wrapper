#pragma once

#include <memory>

namespace dx9 {
	namespace renderer {

		struct RTContainerPimpl;

		struct RTContainer {
			RTContainer();
			~RTContainer();

			std::unique_ptr<RTContainerPimpl> pimpl;

			struct ContainerPimpl;
			std::unique_ptr<ContainerPimpl> pimplRes;
			
		};

	}
}