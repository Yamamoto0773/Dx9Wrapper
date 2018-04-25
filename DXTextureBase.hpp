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

	namespace texture {

		class DXTextureBase {
		protected:
			CComPtr<IDirect3DTexture9> d3dtex9;

			std::wstring name;

			static LogManager* log;

			size_t width;
			size_t height;
			bool isLocked;


		public:
			DXTextureBase();
			virtual ~DXTextureBase();

			// コピー不可
			DXTextureBase(const DXTextureBase&) = delete;
			DXTextureBase& operator=(const DXTextureBase&) = delete;
			// ムーブはOK
			DXTextureBase(DXTextureBase&&) = default;
			DXTextureBase& operator=(DXTextureBase&&) = default;

			// ファイルからテクスチャ作成
			bool Create(IDirect3DDevice9* dev, const std::wstring& fileName);
			// 空テクスチャ作成
			bool Create(IDirect3DDevice9* dev, size_t width, size_t height);

			void CopyTo(DXTextureBase &dst);


			// テクスチャの明示的削除
			virtual void Delete();


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

		};


	
	}



}
