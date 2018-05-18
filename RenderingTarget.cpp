#include "RenderingManager.hpp"


namespace dx9 {

	namespace renderer {




		RenderingTarget_::RenderingTarget_() :
			isValid(false),
			index(0) {
		}

		RenderingTarget_::~RenderingTarget_() {
		}


		RenderingTarget RenderingTargetFactory::CreateRT(IDirect3DDevice9 * device, size_t w, size_t h) {
			if (!device) nullptr;

			int pos = searchEmptyRTPool();

			if (pos != -1) {

				RenderingTarget rt = std::unique_ptr<RenderingTarget_, RT_deleter_t>(new RenderingTarget_);

				rt.get_deleter() = [this](RenderingTarget_ *rt) {
					if (rt->isValid) {
						RTpool[rt->index].user--;

						if (RTpool[rt->index].user == 0) {

							// Release resource
							RTpool[rt->index].res.texSurface.Release();
							RTpool[rt->index].res.depthStencilBuffer.Release();
							RTpool[rt->index].res.texture.Delete();
							ZeroMemory(&RTpool[rt->index].res.viewPort, sizeof(RTpool[rt->index].res.viewPort));
							ZeroMemory(&RTpool[rt->index].surfaceDesc, sizeof(RTpool[rt->index].surfaceDesc));
						}
					}

					delete rt;
				};


				// create RT-resources
				if (!CreateRTContainer(device, w, h, RTpool[pos].res)) {
					return nullptr;
				}


				RTpool[pos].user = 1;	// increment number of users
				RTpool[pos].res.texSurface->GetDesc(&RTpool[pos].surfaceDesc);

				rt->index = pos;		// set index for the RT-resource
				rt->isValid = true;

				return std::move(rt);
			}

			return nullptr;				// number of RT-resources has amounted to its max. 
		}



		void RenderingTargetFactory::ReleaseAll() {

			for (auto &it : RTpool) {
				if (it.user > 0) {

					// Release All RT-resources
					it.res.texSurface.Release();
					it.res.depthStencilBuffer.Release();
					it.res.texture.Delete();
					ZeroMemory(&it.res.viewPort, sizeof(it.res.viewPort));

					//logmng->tlnwrite("Release RT");
				}
			}

		}

		bool RenderingTargetFactory::Restore(IDirect3DDevice9 * device) {
			if (!device) return false;

			for (auto &it : RTpool) {
				// Create All RT-resources
				if (it.user > 0) {
					if (!CreateRTContainer(device, it.surfaceDesc.Width, it.surfaceDesc.Height, it.res))
						return false;
				}
			}

			return true;
		}


		const RTContainer::Container* RenderingTargetFactory::GetContainer(const RenderingTarget_t &rt) {
			if (!rt->isValid) return nullptr;

			return &RTpool[rt->index].res;
		}


		int RenderingTargetFactory::searchEmptyRTPool() {

			for (int i = 0; i < RTRESOURCE_MAXCNT; i++) {
				if (RTpool[i].user == 0) return i;
			}

			return -1;
		}


		bool RenderingTargetFactory::CreateRTContainer(IDirect3DDevice9 *device, size_t w, size_t h, RTContainer::Container &container) {
			if (!device) return false;
			
			HRESULT hr;

			{
				IDirect3DTexture9 *rawPtr;
				if (FAILED(hr = device->CreateTexture(
					w,
					h,
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
					w,
					h,
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
				container.viewPort.Width = w;
				container.viewPort.Height = h;
				container.viewPort.X = 0;
				container.viewPort.Y = 0;
				container.viewPort.MinZ = 0.0f;
				container.viewPort.MaxZ = 1.0f;
			}
			
			return true;
		}


	}

}