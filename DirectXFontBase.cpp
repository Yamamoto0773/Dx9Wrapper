#include "DirectXFontBase.hpp"

#define _USE_MATH_DEFINES
#include <math.h>  

namespace dx9 {
	DirectXFontBase::DirectXFontBase() {
		hFont = nullptr;
	}

	DirectXFontBase::~DirectXFontBase() {}

	void DirectXFontBase::SetFontColor(Color &color) {
		fontColor = color.getRGBAFloat();
	}

	void DirectXFontBase::SetCharTravelDirection(int deg) {
		charTravelAngle_rad = deg*(float)M_PI/180.0f;
	}

	void DirectXFontBase::SetCharTravelDirection(float rad) {
		charTravelAngle_rad = rad;
	}

}