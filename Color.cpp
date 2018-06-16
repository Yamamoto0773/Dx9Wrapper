#include "Color.hpp"


namespace dx9 {

	ColorRGB::ColorRGB(int r, int g, int b, int a) {
		set(r, g, b, a);
	}

	ColorRGB::ColorRGB(float r, float g, float b, float a) {
		set(r, g, b, a);
	}

	ColorRGB::ColorRGB(unsigned int argb) {
		set(argb);
	}


	ColorRGB::~ColorRGB() {}

	void ColorRGB::set(int r, int g, int b, int a) {
		color.r = fit(r, 0, 255);
		color.g = fit(g, 0, 255);
		color.b = fit(b, 0, 255);
		color.a = fit(a, 0, 255);
	}

	void ColorRGB::set(float r, float g, float b, float a) {
		color.r = (int)(fit(r, 0.0f, 1.0f)*255);
		color.g = (int)(fit(g, 0.0f, 1.0f)*255);
		color.b = (int)(fit(b, 0.0f, 1.0f)*255);
		color.a = (int)(fit(a, 0.0f, 1.0f)*255);
	}

	void ColorRGB::set(int argb) {
		set((argb>>16)&0xff, (argb>>8)&&0xff, (argb)&0xff, (argb>>24)&0xff);
	}

	unsigned int ColorRGB::getHex() {
		return (color.a<<24 | color.r<<16 | color.g<<8 | color.b);
	}

	ColorHSB ColorRGB::getColorHSB() {
		int max = (color.r > color.g) ? color.r : color.b;
		max = (max > color.b) ? max : color.b;

		int min = (color.r < color.g) ? color.r : color.g;
		min = (min < color.b) ? min : color.b;

	}

	Color_RGBA &ColorRGB::get() {
		return color;
	}

	Color_RGBAF & ColorRGB::getFloat() {
		return std::move(Color_RGBAF({ color.a/255.f, color.a/255.f, color.a/255.f, color.a/255.f }));
	}




	colorArrey_t ColorRGB::getRGBAFloat() {
		return std::array<float, 4>({ color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f });
	}

	ColorHSB::ColorHSB(int h, int s, int b, int a) {}

	ColorHSB::ColorHSB(float h, float s, float b, float a) {}

	ColorHSB::~ColorHSB() {}


	void ColorHSB::set(int h, int s, int b, int a) {
		if (h < 0) {
			h = abs(h)%360;
			color.h = 360 - h;
		} else {
			color.h = h%360;
		}
		
		color.s = fit(s, 0, 100);
		color.b = fit(b, 0, 100);
		color.a = fit(a, 0, 100);
	}

	void ColorHSB::set(float h, float s, float b, float a) {
		if (h < 0) {
			color.h = 1 - (h - floorf(h));
		} else {
			color.h = h - (int)h;
		}

		color.h = (int)(color.h*255);
		color.s = (int)(fit(s, 0.0f, 1.0f)*255);
		color.b = (int)(fit(b, 0.0f, 1.0f)*255);
		color.a = (int)(fit(a, 0.0f, 1.0f)*255);
	}
	
	
	ColorRGB ColorHSB::getColorRGB() {
		
		return ColorRGB();
	}

	Color_HSBA &ColorHSB::get() {
		return Color_HSBA();
	}

	Color_HSBAF &ColorHSB::getFloat() {
		return Color_HSBAF();
	}

	unsigned int ColorHSB::getHex() {
		return 0;
	}

	colorArrey_t ColorHSB::getRGBAFloat() {
		return std::array<float, 4>();
	}


}