#pragma once

#include "dx9.hpp"

#include <array>


namespace dx9 {

	class Color;
	class ColorRGB;
	class ColorHSB;

	struct Color_RGBA {
		int r;
		int g;
		int b;
		int a;
	};

	struct Color_HSBA {
		int h;
		int s;
		int b;
		int a;
	};

	struct Color_RGBAF {
		float r;
		float g;
		float b;
		float a;
	};

	struct Color_HSBAF {
		float h;
		float s;
		float b;
		float a;
	};

	using colorArrey_t = std::array<float, 4>;

	class Color {

		// htmlカラーコードの先頭にアルファ値を追加した値を返します
		// 0xAARRGGBB (各成分は8bit)
		virtual unsigned int getHex() = 0;

		// rgbaの各成分を0.0-1.0fで表した配列を返します
		virtual std::array<float, 4> getRGBAFloat() = 0;
	};



	class ColorRGB : Color {
		
		Color_RGBA color;

	public:

		// 各成分を0-255で指定して作成
		ColorRGB(int r, int g, int b, int a = 255);
		// 各成分を0.0-1.0で指定して作成
		ColorRGB(float r, float g, float b, float a = 1.0f);
		// ARGBの並びで32bitで指定
		ColorRGB(unsigned int argb);

		~ColorRGB();

		// 各成分を0-255で指定して作成
		void set(int r, int g, int b, int a = 255);
		// 各成分を0.0-1.0で指定して作成
		void set(float r, float g, float b, float a = 1.0f);
		// ARGBの並びで32bitで指定
		void set(int argb);

		// 
		unsigned int getHex() override;


		ColorHSB getColorHSB();

		Color_RGBA &get();
		Color_RGBAF &getFloat();

		colorArrey_t getRGBAFloat() override;

	};




	class ColorHSB : Color {

		Color_HSBA color;
		
	public:

		// 色相(h):0-359, 彩度(s):0-100, 明度(b):0-100, 透明度(a):0-100で指定
		ColorHSB(int h, int s, int b, int a = 100);
		// 各成分を0.0-1.0で指定
		ColorHSB(float h, float s, float b, float a = 1.0f);


		~ColorHSB();


		// 色相(h):0-360, 彩度(s):0-100, 明度(b):0-100, 透明度(a):0-100で指定
		void set(int h, int s, int b, int a = 255);
		// 各成分を0.0-1.0で指定
		void set(float h, float s, float b, float a = 1.0f);
	
		ColorRGB getColorRGB();
		Color_HSBA &get();
		Color_HSBAF &getFloat();



		unsigned int getHex() override;

		colorArrey_t getRGBAFloat() override;


	};

}