#include "RenderingManager.hpp"

#include <d3dx9.h>
#include "RTContainerPimpl.hpp"

namespace dx9 {

	namespace renderer {




		RenderingTarget_class::RenderingTarget_class() :
			isValid(false),
			index(0) {
		}

		RenderingTarget_class::~RenderingTarget_class() {
		}


		RenderingTarget RenderingTargetFactory::CreateRT(IDirect3DDevice9 * device, size_t w, size_t h) {
			if (!device) nullptr;

			int pos = searchEmptyRTPool();

			if (pos != -1) {

				auto deleter = [this](RenderingTarget_class *rt) {
					if (rt->isValid) {
						RTpool[rt->index].pimpl->user--;

						if (RTpool[rt->index].pimpl->user == 0) {

							// Release resource
							RTpool[rt->index].pimplRes->texSurface.Release();
							RTpool[rt->index].pimplRes->depthStencilBuffer.Release();
							RTpool[rt->index].pimplRes->texture.Delete();
							ZeroMemory(&RTpool[rt->index].pimplRes->viewPort, sizeof(RTpool[rt->index].pimplRes->viewPort));
							ZeroMemory(&RTpool[rt->index].pimpl->surfaceDesc, sizeof(RTpool[rt->index].pimpl->surfaceDesc));
						}
					}

					delete rt;
				};

				RenderingTarget rt = std::shared_ptr<RenderingTarget_class>(new RenderingTarget_class, deleter);


				// create RT-resources
				if (!CreateRTContainer(device, w, h, *RTpool[pos].pimplRes.get())) {
					return nullptr;
				}


				RTpool[pos].pimpl->user = 1;	// increment number of users
				RTpool[pos].pimplRes->texSurface->GetDesc(&RTpool[pos].pimpl->surfaceDesc);

				rt->index = pos;		// set index for the RT-resource
				rt->isValid = true;

				return std::move(rt);
			}

			return nullptr;				// number of RT-resources has amounted to its max. 
		}



		void RenderingTargetFactory::ReleaseAll() {

			for (auto &it : RTpool) {
				if (it.pimpl->user > 0) {

					// Release All RT-resources
					it.pimplRes->texSurface.Release();
					it.pimplRes->depthStencilBuffer.Release();
					it.pimplRes->texture.Delete();
					ZeroMemory(&it.pimplRes->viewPort, sizeof(it.pimplRes->viewPort));

					//logmng->tlnwrite("Release RT");
				}
			}

		}

		bool RenderingTargetFactory::Restore(IDirect3DDevice9 * device) {
			if (!device) return false;

			for (auto &it : RTpool) {
				// Create All RT-resources
				if (it.pimpl->user > 0) {
					if (!CreateRTContainer(device, it.pimpl->surfaceDesc.Width, it.pimpl->surfaceDesc.Height, *it.pimplRes.get()))
						return false;
				}
			}

			return true;
		}


		const RTContainer::ContainerPimpl* RenderingTargetFactory::GetContainer(const RenderingTarget_sptr &rt) {
			if (!rt->isValid) return nullptr;

			return RTpool[rt->index].pimplRes.get();
		}


		int RenderingTargetFactory::searchEmptyRTPool() {

			for (int i = 0; i < RTRESOURCE_MAXCNT; i++) {
				if (RTpool[i].pimpl->user == 0) return i;
			}

			return -1;
		}


		bool RenderingTargetFactory::CreateRTContainer(IDirect3DDevice9 *device, size_t w, size_t h, RTContainer::ContainerPimpl &container) {
			if (!device) return false;
			
			HRESULT hr;

			{
				IDirect3DTexture9 *rawPtr;
				if (FAILED(hr = device->CreateTexture(
					(UINT)w,
					(UINT)h,
					1,
					D3DUSAGE_RENDERTARGET,
					D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT,
					&rawPtr,
					NULL
					))) {
					return false;
				}
				container.texture.Create(rawPtr);
			}
			{
				IDirect3DSurface9 *rawPtr;
				if (FAILED(hr = device->CreateDepthStencilSurface(
					(UINT)w,
					(UINT)h,
					D3DFMT_D24S8,
					D3DMULTISAMPLE_NONE,
					0,
					FALSE,
					&rawPtr,
					NULL
					))) {
					return false;
				}
				container.depthStencilBuffer.Attach(rawPtr);


				if (FAILED(
					container.texture.GetPointer()->GetSurfaceLevel(0, &rawPtr)
					)) {
					return false;
				}
				container.texSurface.Attach(rawPtr);


				// set data for view-port
				container.viewPort.Width = (DWORD)w;
				container.viewPort.Height = (DWORD)h;
				container.viewPort.X = 0;
				container.viewPort.Y = 0;
				container.viewPort.MinZ = 0.0f;
				container.viewPort.MaxZ = 1.0f;
			}
			
			return true;
		}


	}

}