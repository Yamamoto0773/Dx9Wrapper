#include "RTContainer.hpp"

#include "RTContainerPimpl.hpp"

namespace dx9 {
	namespace renderer {

		RTContainer::RTContainer() : 
			pimpl(std::make_unique<RTContainerPimpl>()),
			pimplRes(std::make_unique<ContainerPimpl>()) {
		}

		RTContainer::~RTContainer() {}

	}
}