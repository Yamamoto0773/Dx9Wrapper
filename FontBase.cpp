#include "FontBase.hpp"

#define _USE_MATH_DEFINES
#include <math.h>  

namespace dx9 {

	namespace font {

		FontBase::FontBase() {
			hFont = nullptr;
		}

		FontBase::~FontBase() {}

		void FontBase::Clear() {
			fontSize = 0;
			antialiasLv = AntialiasLevel::_15STEPS;
			fontColor = { 0.0f, 0.0f, 0.0f, 1.0f };

			letterSpace = 0;

			charTravelAngle_rad = 0.0f;

			fontRotOrigin = FontRotOrigin::TOP_L;
			textAlign = TextAlign::LEFT;
			strAdjust = TextureAdjust::NONE;
		}

		void FontBase::SetFontColor(const Color &color) {
			fontColor = color.getRGBAFloat();
		}

		void FontBase::SetCharTravelDirection(int deg) {
			charTravelAngle_rad = deg*(float)M_PI/180.0f;
		}

		void FontBase::SetCharTravelDirection(float rad) {
			charTravelAngle_rad = rad;
		}

		void FontBase::SetStringAdjust(TextureAdjust adjust) {
			strAdjust = adjust;
		}

	}
}