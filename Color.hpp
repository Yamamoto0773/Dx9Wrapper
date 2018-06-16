#pragma once

#include <array>
#include <Windows.h>

namespace dx9 {

	// srcをminからmaxの範囲内に収める
	template <typename T>
	T fit(T src, T min, T max) {
		if (src < min)
			src = min;
		else if (src > max)
			src = max;

		return src;
	}


	class Color;
	class ColorRGB;
	class ColorHSB;

	struct colorRGBA_t {
		int r;
		int g;
		int b;
		int a;
	};

	struct colorHSBA_t {
		int h;
		int s;
		int b;
		int a;
	};

	struct colorRGBAF_t {
		float r;
		float g;
		float b;
		float a;
	};

	struct colorHSBAF_t {
		float h;
		float s;
		float b;
		float a;
	};

	using colorArray_t = std::array<float, 4>;



	class Color {

	public:
		// htmlカラーコードの先頭にアルファ値を追加した値を返します
		// 0xAARRGGBB (各成分は8bit)
		virtual DWORD getHex() = 0;

		// rgbaの各成分を0.0-1.0fで表した配列を返します
		virtual colorArray_t getRGBAFloat() = 0;


	};



	class ColorRGB : public Color {
		
		colorRGBA_t color;

	public:

		// 各成分を0-255で指定して作成
		ColorRGB(int r, int g, int b, int a = 255);
		// 各成分を0.0-1.0で指定して作成
		ColorRGB(float r, float g, float b, float a = 1.0f);
		// ARGBの並びで32bitで指定して作成
		ColorRGB(DWORD argb);

		~ColorRGB();

		// 各成分を0-255で指定して作成
		void set(int r, int g, int b, int a = 255);
		// 各成分を0.0-1.0で指定して作成
		void set(float r, float g, float b, float a = 1.0f);
		// ARGBの並びで32bitで指定
		void set(int argb);

		// htmlカラーコードの先頭にアルファ値を追加した値を返します
		// 0xAARRGGBB (各成分は8bit)
		DWORD getHex() override;


		ColorHSB getColorHSB();

		colorRGBA_t get();
		colorRGBAF_t getFloat();

		colorArray_t getRGBAFloat() override;

	};




	class ColorHSB : public Color {

		colorHSBA_t color;
		
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
		colorHSBA_t get();
		colorHSBAF_t getFloat();

		DWORD getHex() override;

		colorArray_t getRGBAFloat() override;
	};

}