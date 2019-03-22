#pragma once

#include "TextureBase.hpp"


// テクスチャを通常の変数のように扱えるクラスです．
// (c) 2018, Yamamoto Nanami



namespace dx9 {

	class TextureManager;

	namespace texture {
		
		struct TexClip {
			Size size;
			UVCoord uv;
		};
		
		
	}

	class Texture {
		friend class TextureManager;

	private:
		std::shared_ptr<texture::TextureBase> tex;
		texture::TexClip clipInfo;

		void set(std::shared_ptr<texture::TextureBase> _tex, texture::TexClip &_clipInfo) {
			this->tex = _tex;
			this->clipInfo = _clipInfo;
		}

	public:
		Texture();
		~Texture();

		// 有効なテクスチャか返す
		bool operator!() const;

		Size getSize() const { return clipInfo.size; };
	};

}

