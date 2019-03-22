#pragma once

#include "TextureBase.hpp"
#include "DX9ShareContainer.hpp"
#include "Texture.hpp"
#include "Singleton.hpp"
#include <vector>
#include <array>


// テクスチャの作成と描画を管理するクラス
// (c) 2018, Yamamoto Nanami


namespace dx9 {


	using tex_vector = std::vector<std::shared_ptr<texture::TextureBase>>;

	class TextureManager :
		private resource::DX9ShareContainer,
		public Singleton<TextureManager> {

		friend class Singleton<TextureManager>;


	private:
		TextureManager();
		~TextureManager();


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
			const texture::TextureBase &texbase
		);


		// ---------------------------------------
		// Setting of rendering

		void SetTextureAdjust(TextureAdjust pos) { textureAdjust = pos; }
		TextureAdjust GetTextureAdjust() { return textureAdjust; }

		void SetDrawTexCoord(DrawTexCoord coord) { drawTexCoord = coord; }
		DrawTexCoord GetDrawTexCoord() { return drawTexCoord; }

		void SetColorFilter(const Color &color, BLENDMODE blendmode);
		
		std::pair<ColorRGB, BLENDMODE> GetColorFilterRGB();
		std::pair<ColorHSB, BLENDMODE> GetColorFilterHSB();

		void RemoveColorFilter();

		// ---------------------------------------
		// 描画を行う関数

		bool DrawTexture(
			const Texture &tex,
			float x,
			float y,
			float scale_x = 1.0f,
			float scale_y = 1.0f,
			float alpha = 1.0f,
			int rotDeg = 0,
			bool isClip = false
		);

		bool DrawTexture(
			const Texture &tex,
			RectF posArea,
			float alpha = 1.0f,
			int rotDeg = 0,
			bool isClip = false
		);

		bool DrawTexture(
			const Texture &tex,
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
			const Texture &tex,
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