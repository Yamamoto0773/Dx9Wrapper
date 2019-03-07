#pragma once

#include "DX9ShareContainer.hpp"

namespace dx9 {

	namespace font {

		class DirectXFontBase : public resource::DX9ShareContainer {

		protected:
			HFONT					hFont;			// 設定中のフォントのハンドル
			AntialiasLevel			antialiasLv;

			size_t fontSize;

			std::array<float, 4> fontColor;

			int letterSpace;

			float charTravelAngle_rad; // travel direction of character (rad)

			FontRotOrigin fontRotOrigin;
			TextAlign textAlign;
			TextureAdjust strAdjust;

			DirectXFontBase();
			~DirectXFontBase();

			void Clear();
			
		public:

			// ARGBの順
			void SetFontColor(const Color &color);


			// 字間の指定 pixel単位で指定
			void SetLetterSpace(int size) { letterSpace = size; };

			// 文字が進む方向の指定 degreeで指定
			void SetCharTravelDirection(int deg);
			// 文字が進む方向の指定 radianで指定
			void SetCharTravelDirection(float rad);

			// 文字寄せの設定
			void SetAlignment(dx9::TextAlign align) { textAlign = align; };

			// 文字回転の中心を設定
			void SetRotateOrigin(FontRotOrigin origin) { fontRotOrigin = origin; };

			// 文字列の拡大縮小に関する設定
			// note:フラグがNONE以外の場合は，一行しか描画されません
			void SetStringAdjust(TextureAdjust adjust);

		};

	}
	
}