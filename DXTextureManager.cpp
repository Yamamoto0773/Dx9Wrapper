#include "DXTextureManager.hpp"

#define Successful (1)
#define Failed (0)

#define WRITELOG(x) { if (log != nullptr) { log->tlnwrite(x); } }




LogManager* DXTextureManager::log;

DXTextureManager::DXTextureManager() {
}

DXTextureManager::~DXTextureManager() {
	Delete();
}


void DXTextureManager::Delete() {
	d3dtex9.Release();
	name.clear();
}

void DXTextureManager::SetLogWriteDest(LogManager* dest) {
	if (dest == nullptr) {
		return;
	}

	log = dest;
}


bool DXTextureManager::Lock(D3DLOCKED_RECT *rect) {
	if (isLocked) {
		return true;
	}

	if (FAILED(d3dtex9->LockRect(0, rect, NULL, 0))) {
		return false;
	}

	isLocked = true;
	return true;
}

bool DXTextureManager::Unlock() {
	if (!isLocked) {
		return true;
	}

	if (FAILED(d3dtex9->UnlockRect(0))) {
		return false;
	}

	return true;
}

void DXTextureManager::Clear() {
	width = 0;
	height = 0;
	isLocked = false;
}






bool TextureFile::Create(IDirect3DDevice9* dev, const std::string& fileName) {
	if (dev == nullptr) {
		return false;
	}

	Clear();

	D3DXIMAGE_INFO imgInfo;
	ZeroMemory( &imgInfo,sizeof(D3DXIMAGE_INFO) );
	IDirect3DTexture9 *ptr;

	HRESULT ret = D3DXCreateTextureFromFileExA(
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

	if( FAILED(ret) ) {
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

	if( FAILED(dev->CreateTexture(
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
		WRITELOG("failed to empty texture");
		return false;
	}

	d3dtex9.Attach(ptr);
	this->width = width;
	this->height = height;

	return true;
}
