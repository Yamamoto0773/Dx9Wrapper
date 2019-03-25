#pragma once

// 2017/12/16
// (c) NanamiYamamoto

//////////////////////////////////////////////
// DirectX9.0を用いたテクスチャを管理するクラス
// 参考:
// http://www.charatsoft.com/develop/otogema/
// http://marupeke296.com/DXG_CreateTextureMethod.html
//////////////////////////////////////////////


#include <memory>
#include <atlbase.h>
#include <string>
#include <functional>

#include "LogManager.hpp"
#include "dx9.hpp"

#include "dx9interface.hpp"


namespace dx9 {

	namespace texture {

		class TextureBase {
		protected:
			std::shared_ptr<IDirect3DTexture9> d3dtex9;

			std::wstring name;

			static LogManager* log;

			size_t width;
			size_t height;
			bool isLocked;



			bool Lock(D3DLOCKED_RECT *rect);
			bool Unlock();
		
			
			void attach(IDirect3DTexture9* p);

		public:
			TextureBase();
			virtual ~TextureBase();

			TextureBase(const TextureBase&);
			TextureBase& operator=(const TextureBase&);
			// ムーブはOK
			TextureBase(TextureBase&&) = default;
			TextureBase& operator=(TextureBase&&) = default;

			// ファイルからテクスチャ作成
			bool Create(IDirect3DDevice9* dev, const std::wstring& fileName);
			// 空テクスチャ作成
			bool Create(IDirect3DDevice9* dev, size_t width, size_t height);
			// Create texture from IDirect3DTexture9. ownership of [tex] move.
			bool Create(IDirect3DTexture9 *tex);

			void CopyFrom(const TextureBase &src);


			bool operator!(); 


			// テクスチャの明示的削除
			virtual void Delete();

			// テクスチャの幅を取得
			size_t GetWidth() const { return width; }
			// テクスチャの高さを取得
			size_t GetHeight() const { return height; }


			// テクスチャへ名前を格納
			void SetName(const std::wstring& _name) { this->name = _name; }
			// テクスチャ名を取得
			const std::wstring& GetName() const { return name; }


			// Direct3DTexture9ポインタを取得
			IDirect3DTexture9* GetPointer() const;

			std::shared_ptr<IDirect3DTexture9> GetSharedPtr() const;


			static void SetLogWriteDest(LogManager* dest);

		};


	
	}



}
