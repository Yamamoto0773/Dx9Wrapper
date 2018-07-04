#pragma once


#include <d3dx9.h>


namespace dx9 {

	namespace type {

		struct d3dinterface_deleter {
			void operator()(void* ptr) {
				if (ptr) {
					static_cast<IUnknown*>(ptr)->Release();
					ptr = nullptr;
				}
			}
		};

		struct vptr_deleter {
			void operator()(void* ptr) {
				if (ptr) {
					delete ptr;
					ptr = nullptr;
				}
			}
		};


	}
}


