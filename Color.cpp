#include "Color.hpp"


namespace dx9 {

	ColorRGB::ColorRGB(int r, int g, int b, int a) {
		set(r, g, b, a);
	}

	ColorRGB::ColorRGB(float r, float g, float b, float a) {
		set(r, g, b, a);
	}

	ColorRGB::ColorRGB(DWORD argb) {
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

	DWORD ColorRGB::getHex() {
		return (color.a<<24 | color.r<<16 | color.g<<8 | color.b);
	}

	ColorHSB ColorRGB::getColorHSB() {
		int h, s, b;

		// get max and min value in RGB
		std::array<int, 3> col = {color.r, color.g, color.b};
		int max = 0, min = 255;
		for (int i=0; i<3; i++) {
			if (col[i] > max) max = col[i];
			if (col[i] < min) min = col[i];
		}

		// calc hue
		if (max == min) {
			h = 0;
		}
		else {
			if (max == color.r) {
				h = (int)(60*(color.g - color.b)/(float)(max - min));
			}
			else if (max == color.g) {
				h = (int)(60*(color.b - color.r)/(float)(max - min) + 120);
			}
			else if (max == color.b) {
				h = (int)(60*(color.r - color.g)/(float)(max - min) + 240);
			}


			if (h < 0) h += 360;
		}

		// calc saturation
		s = (int)(100.f*(max - min)/max);

		// calc brightness
		b = (int)(max*100/255.f);


		return ColorHSB(h, s, b, (int)(color.a*100/255.f));
	}

	rgba_t ColorRGB::get() {
		return color;
	}

	rgba_float_t ColorRGB::getFloat() {
		rgba_float_t col = {
			color.a/255.f,
			color.g/255.f,
			color.b/255.f,
			color.a/255.f
		};

		return col;
	}

	color_array_t ColorRGB::getRGBAFloat() {
		color_array_t col = {
			color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f
		};

		return col;
	}






	ColorHSB::ColorHSB(int h, int s, int b, int a) {
		set(h, s, b, a);
	}

	ColorHSB::ColorHSB(float h, float s, float b, float a) {
		set(h, s, b, a);
	}

	ColorHSB::~ColorHSB() {}


	void ColorHSB::set(int h, int s, int b, int a) {
		if (h < 0) {
			h = abs(h)%360;
			color.h = 360 - h;
		}
		else {
			color.h = h%360;
		}

		color.s = fit(s, 0, 100);
		color.b = fit(b, 0, 100);
		color.a = fit(a, 0, 100);
	}

	void ColorHSB::set(float h, float s, float b, float a) {
		if (h < 0) {
			h = 1 - (h - floorf(h));
		}
		else {
			h = h - (int)h;
		}

		color.h = (int)(color.h*360);
		color.s = (int)(fit(s, 0.0f, 1.0f)*100);
		color.b = (int)(fit(b, 0.0f, 1.0f)*100);
		color.a = (int)(fit(a, 0.0f, 1.0f)*100);
	}


	ColorRGB ColorHSB::getColorRGB() {
		auto ary = getRGBAFloat();
		return ColorRGB(ary[0], ary[1], ary[2], ary[3]);
	}

	hsba_t ColorHSB::get() {
		return color;
	}

	hsba_float_t ColorHSB::getFloat() {
		hsba_float_t col = {
			color.h/360.f,
			color.s/100.f,
			color.b/100.f,
			color.a/100.f
		};

		return col;
	}

	DWORD ColorHSB::getHex() {
		auto col = getColorRGB();
		return col.getHex();
	}

	color_array_t ColorHSB::getRGBAFloat() {
		float max = (float)color.b*255/100;
		float min = max - color.s*max/100;
		float r, g, b;

		if (0 <= color.h && color.h < 60) {
			r = max;
			g = ((color.h/60.f)*(max - min) + min);
			b = min;
		}
		else if (60 <= color.h && color.h < 120) {
			r = ((120 - color.h)/60.f*(max - min) + min);
			g = max;
			b = min;
		}
		else if (120 <= color.h && color.h < 180) {
			r = min;
			g = max;
			b = ((color.h - 120)/60.f*(max - min) + min);
		}
		else if (180 <= color.h && color.h < 240) {
			r = min;
			g = ((240 - color.h)/60.f*(max - min) + min);
			b = max;
		}
		else if (240 <= color.h && color.h < 300) {
			r = ((color.h - 240)/60.f*(max - min) + min);
			g = min;
			b = max;
		}
		else {
			r = max;
			g = min;
			b = ((360 - color.h)/60.f*(max - min) + min);
		}


		color_array_t ary = {
			r/255.f, g/255.f, b/255.f, color.a/100.f
		};

		return ary;
	}

	


}