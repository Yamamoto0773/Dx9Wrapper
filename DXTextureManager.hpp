#pragma once

// 2017/12/16
// (c) NanamiYamamoto

//////////////////////////////////////////////
// DirectX9.0を用いたテクスチャを管理するクラス
// 参考:
// http://www.charatsoft.com/develop/otogema/
// http://marupeke296.com/DXG_CreateTextureMethod.html
//////////////////////////////////////////////



#include<d3dx9.h>

#include<memory>
#include<string>

#include "LogManager.hpp"


class DXTextureManager {
private:
	std::unique_ptr<IDirect3DTexture9> d3dtex9;

	std::string fileName;

	LogManager* log;

	unsigned long width;
	unsigned long height;
	bool isLocked;

public:

	// テクスチャ生成
	static DXTextureManager* Create(IDirect3DDevice9* dev, const std::string& fileName);

	// テクスチャの削除
	void Delete();


	// テクスチャの幅を取得
	unsigned long GetWidth() const { return width; }
	// テクスチャの高さを取得
	unsigned long GetHeight() const { return height; }

	// 画像ファイル名を取得
	const std::string& GetFileName() const { return fileName; }

	// Direct3DTexture9ポインタを取得
	const IDirect3DTexture9* GetPointer() const { return d3dtex9.get(); };


	void SetLogWriteDest(LogManager* dest);

private:
	DXTextureManager(const DXTextureManager&) = delete;	// コピー不可
	DXTextureManager& operator=(const DXTextureManager&) = delete; // 代入不可
	DXTextureManager();
	~DXTextureManager();

	bool CreateD3DTex9(IDirect3DDevice9* dev, const std::string& fileName);


	bool Lock(D3DLOCKED_RECT *rect);
	bool Unlock();
	void Clear();

};