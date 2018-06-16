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

		// html�J���[�R�[�h�̐擪�ɃA���t�@�l��ǉ������l��Ԃ��܂�
		// 0xAARRGGBB (�e������8bit)
		virtual unsigned int getHex() = 0;

		// rgba�̊e������0.0-1.0f�ŕ\�����z���Ԃ��܂�
		virtual std::array<float, 4> getRGBAFloat() = 0;
	};



	class ColorRGB : Color {
		
		Color_RGBA color;

	public:

		// �e������0-255�Ŏw�肵�č쐬
		ColorRGB(int r, int g, int b, int a = 255);
		// �e������0.0-1.0�Ŏw�肵�č쐬
		ColorRGB(float r, float g, float b, float a = 1.0f);
		// ARGB�̕��т�32bit�Ŏw��
		ColorRGB(unsigned int argb);

		~ColorRGB();

		// �e������0-255�Ŏw�肵�č쐬
		void set(int r, int g, int b, int a = 255);
		// �e������0.0-1.0�Ŏw�肵�č쐬
		void set(float r, float g, float b, float a = 1.0f);
		// ARGB�̕��т�32bit�Ŏw��
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

		// �F��(h):0-359, �ʓx(s):0-100, ���x(b):0-100, �����x(a):0-100�Ŏw��
		ColorHSB(int h, int s, int b, int a = 100);
		// �e������0.0-1.0�Ŏw��
		ColorHSB(float h, float s, float b, float a = 1.0f);


		~ColorHSB();


		// �F��(h):0-360, �ʓx(s):0-100, ���x(b):0-100, �����x(a):0-100�Ŏw��
		void set(int h, int s, int b, int a = 255);
		// �e������0.0-1.0�Ŏw��
		void set(float h, float s, float b, float a = 1.0f);
	
		ColorRGB getColorRGB();
		Color_HSBA &get();
		Color_HSBAF &getFloat();



		unsigned int getHex() override;

		colorArrey_t getRGBAFloat() override;


	};

}