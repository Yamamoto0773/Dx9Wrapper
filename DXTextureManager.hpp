#pragma once

#include "DXTextureBase.hpp"
#include "DX9ShareContainer.hpp"
#include "Texture.hpp"
#include "Singleton.hpp"
#include <vector>
#include <array>


// テクスチャの作成と描画を管理するクラス
// (c) 2018, Yamamoto Nanami


namespace dx9 {


	namespace texture {

		using tex_vector = std::vector<std::shared_ptr<texture::DXTextureBase>>;

		class DXTextureManager :
			private resource::DX9ShareContainer,
			public Singleton<DXTextureManager> {

			friend class Singleton<DXTextureManager>;

		
		private:
			DXTextureManager();
			~DXTextureManager();


			// 画像ファイル用のテクスチャプール
			tex_vector texPool;

			DrawTexCoord drawTexCoord;
			
			std::array<float, 3> filterColor;
			BLENDMODE filterBlendMode;

			TextureAdjust textureAdjust;


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
				ClipArea clipArea
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
			// Setting of rendering


			void SetTextureAdjust(TextureAdjust pos) { textureAdjust = pos; }
			
			void SetDrawTexCoord(DrawTexCoord coord) { drawTexCoord = coord; }

			void SetColorFilter(DWORD color, BLENDMODE blendmode);
			void SetColorFilter(size_t r, size_t g, size_t b, BLENDMODE blendmode);
			void RemoveColorFilter();

			std::pair<DWORD, BLENDMODE> GetTexFilter();


			// ---------------------------------------
			// 描画を行う関数
	
			bool DrawTexture(
				Texture &tex,
				float x,
				float y,
				float scale_x = 1.0f,
				float scale_y = 1.0f,
				float alpha = 1.0f,
				int rotDeg = 0,
				bool isClip = false
			);

			bool DrawTexture(
				Texture &tex,
				RectF posArea,
				float alpha = 1.0f,
				int rotDeg = 0,
				bool isClip = false
			);
			
			bool DrawTexture(
				Texture &tex,
				RectF posArea,
				ClipArea clipArea,
				float alpha = 1.0f,
				int rotDeg = 0,
				bool isClip = false		// if true, pixel shader discard (don't render) transparent pixels.
			);
		
			// 使用されていないテクスチャを削除
			// 戻り値:解放したテクスチャの数
			int CleanTexPool();
			

		private:
			bool DrawTexture_main(
				Texture &tex,
				float x,
				float y,
				float uv_left,
				float uv_top,
				float uv_w,
				float uv_h,
				float scale_x,
				float scale_y,
				float alpha,
				int rotDeg,
				bool isClip
			);
			

			

			tex_vector::iterator FindTexture(const std::wstring& fileName);
			tex_vector::iterator FindEmptyPool();

		};

	}

}