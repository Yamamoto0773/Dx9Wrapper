#pragma once

#include "DXTextureBase.hpp"


// テクスチャを通常の変数のように扱えるクラスです．
// (c) 2018, Yamamoto Nanami



namespace dx9 {

	class DXTextureManager;

	namespace texture {
		
		struct TexClip {
			Size size;
			UVCoord uv;
		};
		
		
	}

	class Texture {
		friend class DXTextureManager;

	private:
		std::shared_ptr<texture::DXTextureBase> tex;
		texture::TexClip clipInfo;

		void set(std::shared_ptr<texture::DXTextureBase> tex, texture::TexClip &clipInfo) {
			this->tex = tex;
			this->clipInfo = clipInfo;
		}

	public:
		Texture();
		~Texture();

		// 有効なテクスチャか返す
		bool operator!();

		Size getSize() { return clipInfo.size; };
	};

}

