#pragma once

#include <array>
#include <Windows.h>

namespace dx9 {

	// src��min����max�͈͓̔��Ɏ��߂�
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

	struct rgba_t {
		int r;
		int g;
		int b;
		int a;
	};

	struct hsba_t {
		int h;
		int s;
		int b;
		int a;
	};

	struct rgba_float_t {
		float r;
		float g;
		float b;
		float a;
	};

	struct hsba_float_t {
		float h;
		float s;
		float b;
		float a;
	};

	using color_array_t = std::array<float, 4>;



	class Color {

	public:
		// html�J���[�R�[�h�̐擪�ɃA���t�@�l��ǉ������l��Ԃ��܂�
		// 0xAARRGGBB (�e������8bit)
		virtual DWORD getHex() = 0;

		// rgba�̊e������0.0-1.0f�ŕ\�����z���Ԃ��܂�
		virtual color_array_t getRGBAFloat() = 0;


	};



	class ColorRGB : public Color {
		
		rgba_t color;

	public:

		// �e������0-255�Ŏw�肵�č쐬
		ColorRGB(int r, int g, int b, int a = 255);
		// �e������0.0-1.0�Ŏw�肵�č쐬
		ColorRGB(float r, float g, float b, float a = 1.0f);
		// ARGB�̕��т�32bit�Ŏw�肵�č쐬
		ColorRGB(DWORD argb);

		~ColorRGB();

		// �e������0-255�Ŏw�肵�č쐬
		void set(int r, int g, int b, int a = 255);
		// �e������0.0-1.0�Ŏw�肵�č쐬
		void set(float r, float g, float b, float a = 1.0f);
		// ARGB�̕��т�32bit�Ŏw��
		void set(int argb);

		// html�J���[�R�[�h�̐擪�ɃA���t�@�l��ǉ������l��Ԃ��܂�
		// 0xAARRGGBB (�e������8bit)
		DWORD getHex() override;


		ColorHSB getColorHSB();

		rgba_t get();
		rgba_float_t getFloat();

		color_array_t getRGBAFloat() override;

	};




	class ColorHSB : public Color {

		hsba_t color;
		
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
		hsba_t get();
		hsba_float_t getFloat();

		DWORD getHex() override;

		color_array_t getRGBAFloat() override;
	};

}