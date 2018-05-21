#include "RenderingManager.hpp"


namespace dx9 {

	namespace renderer {

		bool RenderingManager::CreateRenderingTarget(IDirect3DDevice9 * device, RenderingTarget & rt, size_t w, size_t h) {
			auto &fac = RenderingTargetFactory::GetInstance();

			rt = fac.CreateRT(device, w, h);

			if (rt == nullptr) {
				return false;
			}

			return true;
		}


		bool RenderingManager::SetRenderingTarget(IDirect3DDevice9 * device, const RenderingTarget & rt) {
			if (!device) return nullptr;

			const auto *destRTcon = RenderingTargetFactory::GetInstance().GetContainer(rt);

			if (destRTcon == nullptr)
				return false;

			if (currentUsrRT.expired() || currentUsrRT.lock() != rt) {
				
				if (currentUsrRT.expired()) {
					// preserve data for default RT

					if (!GetDefaultRT(device, defaultRTcon))
						return false;
				}

				// change Rendering Target
				device->SetRenderTarget(0, destRTcon->texSurface);
				device->SetDepthStencilSurface(destRTcon->depthStencilBuffer);
				device->SetViewport(&destRTcon->viewPort);

				currentUsrRT = rt;		// get reference

			}
			
			return true;
		}

		bool RenderingManager::SetRenderingTarget(IDirect3DDevice9 * device) {
			if (!device) return nullptr;
			
			if (!currentUsrRT.expired()) {

				// change Rendering Target
				if (FAILED(device->SetRenderTarget(0, defaultRTcon.texSurface))) {
					return false;
				}
					
				device->SetDepthStencilSurface(defaultRTcon.depthStencilBuffer);
				device->SetViewport(&defaultRTcon.viewPort);


				defaultRTcon.texSurface.Release();
				defaultRTcon.depthStencilBuffer.Release();
				ZeroMemory(&defaultRTcon, sizeof(defaultRTcon));

				currentUsrRT.reset();
			}

			return true;
		}


		bool RenderingManager::OnLostDevice(IDirect3DDevice9 *device) {
			auto *fac = &RenderingTargetFactory::GetInstance();

			
			if (!currentUsrRT.expired()) {
				// If current RT is users one, change it to default.
				SetRenderingTarget(device);
			}

			// Release all user RT
			fac->ReleaseAll();

			currentUsrRT.reset();

			return true;
		}


		bool RenderingManager::OnResetDevice(IDirect3DDevice9 * device) {
			return RenderingTargetFactory::GetInstance().Restore(device);
		}


		const texture::DXTextureBase *RenderingManager::GetTexture(const RenderingTarget & rt) {

			const auto *con = RenderingTargetFactory::GetInstance().GetContainer(rt);

			if (con == nullptr)
				return nullptr;

			return &con->texture;
		}

	

		bool RenderingManager::GetDefaultRT(IDirect3DDevice9 * device, RTContainer::Container &rt) {
			if (!device) return false;

			{
				IDirect3DSurface9 *rawPtr;

				if (FAILED(device->GetRenderTarget(0, &rawPtr)))
					return false;
				defaultRTcon.texSurface.Attach(rawPtr);

				if (FAILED(device->GetDepthStencilSurface(&rawPtr)))
					return false;
				defaultRTcon.depthStencilBuffer.Attach(rawPtr);

				if (FAILED(device->GetViewport(&defaultRTcon.viewPort)))
					return false;

			}


			return true;
		}


	}

}