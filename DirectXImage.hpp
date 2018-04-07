#pragma once

#include "DXDrawManager.hpp"
#include "DXTextureManager.hpp"
#include <array>


namespace dx9 {

	static const int TEXTURE_MAXCNT = 1024;	// 登録可能なテクスチャ数

	class DirectXImage : public DXDrawManager {
	public:
		DirectXImage() {};
		~DirectXImage() {};

	private:
		// 画像ファイル用のテクスチャ
		std::array<std::unique_ptr<TextureFile>, TEXTURE_MAXCNT> texRes;
		
		

	public:

		// ---------------------------------------
		// テクスチャの管理などを行う関数

		// 画像ファイルからテクスチャを登録
		bool AddTexture(
			size_t texID,
			const std::wstring& fileName
			);

		// 画像ファイルからトリミングしてテクスチャを登録
		bool AddTexture(
			size_t texID,
			const std::wstring& fileName,
			size_t x,
			size_t y,
			size_t w,
			size_t h
			);


		// テクスチャの削除
		bool DelTexture(size_t texID);


		// ---------------------------------------
		// 描画を行う関数
		// 指定番号のテクスチャを描画対象に追加
		bool Draw(
			size_t texID,
			float x,
			float y,
			DrawTexCoord coord,
			float alpha=1.0f
			);
		bool Draw(
			size_t texID,
			float x,
			float y,
			DrawTexCoord coord,
			float xscale=1.0f,
			float yscale=1.0f,
			float alpha=1.0f,
			int rotDeg=0
			);



	private:

		int GetTexResourceID(const std::wstring& fileName);

	};

}