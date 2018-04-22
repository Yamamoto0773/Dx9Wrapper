#include "DXTextureManager.hpp"

#define Successful (1)
#define Failed (0)

#define WRITELOG(x) { if (log != nullptr) { log->tlnwrite(x); } }


namespace dx9 {

	namespace texture {

		LogManager* DXTextureBase::log;

		DXTextureBase::DXTextureBase() {
		}

		DXTextureBase::~DXTextureBase() {
		}


		void DXTextureBase::CopyTo(DXTextureBase &dst) {
			dst.d3dtex9 = this->d3dtex9;
			dst.name = this->name;
			dst.width = this->width;
			dst.height = this->height;
			dst.isLocked = this->isLocked;
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





		void TextureFile::Delete() {
			d3dtex9.Release();
			name.clear();

			width = 0;
			height = 0;
			isLocked = false;

			ZeroMemory(&texClip, sizeof(texClip));

		}


		bool TextureFile::Create(IDirect3DDevice9* dev, const std::wstring& fileName) {
			if (dev == nullptr) {
				return false;
			}

			D3DXIMAGE_INFO imgInfo;
			ZeroMemory(&imgInfo, sizeof(D3DXIMAGE_INFO));
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
				WRITELOG("failed to create texture from \"%s\"", fileName);
				return false;
			}

			// オブジェクトをセット
			d3dtex9.Attach(ptr);

			width = imgInfo.Width;
			height = imgInfo.Height;


			return true;
		}






		bool EmptyTexture::Create(IDirect3DDevice9 * dev, size_t width, size_t height) {
			IDirect3DTexture9 *ptr;

			if (FAILED(dev->CreateTexture(
				width,
				height,
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

			d3dtex9.Attach(ptr);
			this->width = width;
			this->height = height;

			return true;
		}


		void EmptyTexture::Delete() {
			d3dtex9.Release();
			name.clear();

			width = 0;
			height = 0;
			isLocked = false;

		}

	}

}

