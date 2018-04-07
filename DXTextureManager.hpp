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
#include "dx9.hpp"


namespace dx9 {

	class DXTextureManager {
	protected:
		CComPtr<IDirect3DTexture9> d3dtex9;

		std::wstring name;

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


		void CopyTo(DXTextureManager &dst);


		// テクスチャの明示的削除
		virtual void Delete() = 0;


		bool Lock(D3DLOCKED_RECT *rect);
		bool Unlock();

		// テクスチャの幅を取得
		unsigned long GetWidth() const { return width; }
		// テクスチャの高さを取得
		unsigned long GetHeight() const { return height; }


		// テクスチャへ名前を格納
		void SetName(const std::wstring& name) { this->name = name; }
		// テクスチャ名を取得
		const std::wstring& GetName() const { return name; }


		// Direct3DTexture9ポインタを取得
		IDirect3DTexture9* GetPointer() const { return d3dtex9.p; };

		CComPtr<IDirect3DTexture9> GetComPtr() const { return d3dtex9; }


		static void SetLogWriteDest(LogManager* dest);

	private:


	protected:
		DXTextureManager();
		virtual ~DXTextureManager();



	};





	struct TexClip {
		Size size;
		UVCoord uv;
	};

	class TextureFile : public DXTextureManager {
		// 切り抜き情報
		TexClip texClip;


	public:
		TextureFile() {};
		~TextureFile() {};


		const TexClip& GetClipInfo() { return texClip; };
		void SetClipSize(Size &size) { texClip.size = size; };
		void SetClipUV(UVCoord &uv) { texClip.uv = uv; };


		bool Create(IDirect3DDevice9* dev, const std::wstring& fileName);

		void Delete() override;


	};



	class EmptyTexture : public DXTextureManager {
	public:
		bool Create(IDirect3DDevice9* dev, size_t width, size_t height);

		void Delete() override;

	};


}
