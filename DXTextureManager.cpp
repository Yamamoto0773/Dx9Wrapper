#include "DXTextureManager.hpp"

#define Successful (1)
#define Failed (0)

DXTextureManager *DXTextureManager::Create(IDirect3DDevice9* dev, const std::string& fileName) {
	static DXTextureManager *ptr = new DXTextureManager();

	if (ptr->CreateD3DTex9(dev, fileName) == Failed) {
		delete ptr;
		ptr = nullptr;
	}

	return ptr;
}

void DXTextureManager::Delete() {
	Clear();
}

void DXTextureManager::SetLogWriteDest(LogManager* dest) {
	if (dest == nullptr) {
		return;
	}

	log = dest;
}

DXTextureManager::DXTextureManager() {
}

DXTextureManager::~DXTextureManager() {
}

bool DXTextureManager::CreateD3DTex9(IDirect3DDevice9* dev, const std::string& fileName) {
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
		log->tlnwrite("画像\"%s\"ロード失敗", fileName);
		return false;
	}

	// オブジェクトをセット
	d3dtex9.reset(ptr);

	this->fileName = fileName;

	return true;
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
	d3dtex9.reset();
	fileName.clear();
	width = 0;
	height = 0;
	isLocked = false;
}

