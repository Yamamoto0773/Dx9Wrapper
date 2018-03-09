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
#include<atlbase.h>
#include<string>

#include "LogManager.hpp"


class DXTextureManager {
protected:
	CComPtr<IDirect3DTexture9> d3dtex9;

	std::string name;

	static LogManager* log;

	unsigned long width;
	unsigned long height;
	bool isLocked;

public:
	// コピー不可
	DXTextureManager(const DXTextureManager&) = delete;
	DXTextureManager& operator=(const DXTextureManager&) = delete;
	// ムーブはOK
	DXTextureManager(DXTextureManager&&) = default;
	DXTextureManager& operator=(DXTextureManager&&) = default;


	// テクスチャ生成メソッドは，継承先で定義する


	// テクスチャの明示的削除
	void Delete();


	bool Lock(D3DLOCKED_RECT *rect);
	bool Unlock();
	void Clear();


	// テクスチャの幅を取得
	unsigned long GetWidth() const { return width; }
	// テクスチャの高さを取得
	unsigned long GetHeight() const { return height; }


	// テクスチャへ名前を格納
	void SetName(const std::string& name) { this->name = name; }
	// テクスチャ名を取得
	const std::string& GetName() const { return name; }


	// Direct3DTexture9ポインタを取得
	IDirect3DTexture9* GetPointer() const { return d3dtex9.p; };


	static void SetLogWriteDest(LogManager* dest);

private:
	

protected:
	DXTextureManager();
	virtual ~DXTextureManager();

	

};



class TextureFile : public DXTextureManager {
public :
	bool Create(IDirect3DDevice9* dev, const std::string& fileName);
};


class EmptyTexture : public DXTextureManager {
public :
	bool Create(IDirect3DDevice9* dev, size_t width, size_t height);
};
