#include "DXTextureManager.hpp"

#define Successful (1)
#define Failed (0)

DXTextureManager *DXTextureManager::Create(IDirect3DDevice9 *dev, const char *fileName) {
	static DXTextureManager *ptr = new DXTextureManager();

	if (ptr->Create(dev, fileName) == Failed) {
		delete ptr;
		ptr = nullptr;
	}

	return ptr;
}

DXTextureManager::DXTextureManager() {
}

DXTextureManager::~DXTextureManager() {
}

bool DXTextureManager::CreateD3DTex9(IDirect3DDevice9 *dev, const char *fileName) {
	Clear();

	D3DXIMAGE_INFO imgInfo;
	ZeroMemory( &imgInfo,sizeof(D3DXIMAGE_INFO) );
	IDirect3DTexture9 *ptr;

	HRESULT ret = D3DXCreateTextureFromFileExA(
		dev,
		fileName,
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
		DEBUG( "[%s] 画像ロード失敗\n",file );
		return FALSE;
	}

	return true;
}

void DXTextureManager::Lock() {
}

void DXTextureManager::Unlock() {
}

void DXTextureManager::Clear() {
	d3dtex9.reset();
	width = 0;
	height = 0;
	errMsg.clear();
	errMsg.reserve(128);
}

void DXTextureManager::WriteErrMsg(const char * msg, ...) {
}
