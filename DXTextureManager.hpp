#pragma once

#include "DXTextureBase.hpp"
#include "DX9ShareContainer.hpp"
#include "Texture.hpp"
#include <vector>


// テクスチャの作成と描画を管理するクラス
// (c) 2018, Yamamoto Nanami


namespace dx9 {


	namespace texture {

		using tex_vector = std::vector<std::shared_ptr<texture::DXTextureBase>>;

		class DXTextureManager : private resource::DX9ShareContainer {
		public:
			DXTextureManager() {};
			~DXTextureManager() {};

		private:

			// 画像ファイル用のテクスチャプール
			tex_vector texPool;


		public:

			// ---------------------------------------
			// テクスチャの管理などを行う関数

			// 画像ファイルからテクスチャを作成
			bool CreateFromFile(
				Texture &tex,
				const std::wstring& fileName
				);

			// 画像ファイルからトリミングしてテクスチャを作成
			bool CreateFromFile(
				Texture &tex,
				const std::wstring& fileName,
				size_t x,
				size_t y,
				size_t w,
				size_t h
				);

			// 空のテクスチャを作成
			bool CreateEmptyTex(
				Texture &tex,
				size_t w,
				size_t h
				);

			
			// Create texture from IDirct3DTexture9
			// ownership of [texbase] is moved
			bool CreateFromD3DTex9(
				Texture &tex,
				const DXTextureBase &texbase
				);


			// ---------------------------------------
			// 描画を行う関数

			// If [isClip] is true, clip data in [tex] is ignored.
			bool DrawTexture(
				Texture &tex,
				float x,
				float y,
				DrawTexCoord coord=DrawTexCoord::TOP_L,
				DWORD color=0xffffffff,
				float xscale=1.0f,
				float yscale=1.0f,
				int rotDeg=0,
				bool isClip = false		
				);

		
			// 使用されていないテクスチャを削除
			// 戻り値:解放したテクスチャの数
			int CleanTexPool();
			

		private:

			 tex_vector::iterator FindTexture(const std::wstring& fileName);
			 tex_vector::iterator FindEmptyPool();

		};

	}

}