#include "DXTextureBase.hpp"

#define Successful (1)
#define Failed (0)

#define WRITELOG(x, ...) { if (log != nullptr) { log->tlnwrite(x, __VA_ARGS__); } }

#include <d3dx9.h>

namespace dx9 {

	namespace texture {

		LogManager* DXTextureBase::log;

		void deleter(IDirect3DTexture9* p) {
			if (p) {
				p->Release();
				p = nullptr;
			}
		}

		DXTextureBase::DXTextureBase() : 
			d3dtex9(nullptr, deleter) // d3dtex9
		{
		}

		DXTextureBase::~DXTextureBase() {
		}

		DXTextureBase::DXTextureBase(const DXTextureBase &t) {
			CopyFrom(t);
		}

		DXTextureBase & DXTextureBase::operator=(const DXTextureBase &t) {
			CopyFrom(t);
			return *this;
		}



		bool DXTextureBase::Create(IDirect3DDevice9* dev, const std::wstring& fileName) {
			if (dev == nullptr) {
				return false;
			}

			D3DXIMAGE_INFO imgInfo;
			IDirect3DTexture9 *ptr;

			HRESULT ret = D3DXCreateTextureFromFileExW(
				dev,
				fileName.c_str(),
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				1,
				0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_FILTER_POINT,
				D3DX_FILTER_NONE,
				0,
				&imgInfo,
				NULL,
				&ptr
				);

			if (FAILED(ret)) {
				WRITELOG("failed to create texture from \"%ls\"", fileName.c_str());
				return false;
			}

			// オブジェクトをセット
			attach(ptr);

			width = imgInfo.Width;
			height = imgInfo.Height;


			return true;
		}


		bool DXTextureBase::Create(IDirect3DDevice9 * dev, size_t _w, size_t _h) {
			IDirect3DTexture9 *ptr;

			if (FAILED(dev->CreateTexture(
				(UINT)_w,
				(UINT)_h,
				1,
				0,
				D3DFMT_A8R8G8B8,
				D3DPOOL_MANAGED,
				&ptr,
				NULL
				))
				) {
				WRITELOG("failed to create empty texture");
				return false;
			}

			attach(ptr);
			this->width = _w;
			this->height = _h;

			return true;
		}

		bool DXTextureBase::Create(IDirect3DTexture9 * tex) {
			if (!tex) return false;

			D3DSURFACE_DESC desc;
			tex->GetLevelDesc(0, &desc);

			width = desc.Width;
			height = desc.Height;
			
			attach(tex);
			this->isLocked = false;

			return true;
		}


		void DXTextureBase::CopyFrom(const DXTextureBase &src) {
			this->d3dtex9 =	src.d3dtex9;
			this->name = src.name;	
			this->width = src.width;
			this->height = src.height;
			this->isLocked = src.isLocked;
		}

		bool DXTextureBase::operator!() {
			return !d3dtex9;
		}


		void DXTextureBase::SetLogWriteDest(LogManager* dest) {
			if (dest == nullptr) {
				return;
			}

			log = dest;
		}


		bool DXTextureBase::Lock(D3DLOCKED_RECT *rect) {
			if (isLocked) {
				return true;
			}

			if (FAILED(d3dtex9->LockRect(0, rect, NULL, 0))) {
				return false;
			}

			isLocked = true;
			return true;
		}

		bool DXTextureBase::Unlock() {
			if (!isLocked) {
				return true;
			}

			if (FAILED(d3dtex9->UnlockRect(0))) {
				return false;
			}

			return true;
		}


		void DXTextureBase::Delete() {
			d3dtex9.reset();
			name.clear();

			width = 0;
			height = 0;
			isLocked = false;

		}


		void DXTextureBase::attach(IDirect3DTexture9 * p) {
			d3dtex9.reset(p, deleter);
		}

	}

}

