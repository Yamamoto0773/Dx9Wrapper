#include "DirectXFigure.hpp"

#include <math.h>

namespace dx9 {

	DirectXFigure::DirectXFigure() :
		emptyTex(std::make_unique<EmptyTexture>()), 
		texFilter(TextureFilter::LINEAR) {
	}

	DirectXFigure::~DirectXFigure() {
	}


	bool DirectXFigure::Create() {
		if (emptyTex->GetPointer()) {
			return true;
		}

		return emptyTex->Create(d3ddev9, 1, 1);;
	}


	bool DirectXFigure::DrawLine(PointF begin, PointF end, DWORD color, float lineWidth) {
		if (lineWidth < 0.0f) {
			return false;
		}

		figure::Line line;
		
		line.SetPos(begin, end);
		line.SetColor(color);
		line.SetLineWidth(lineWidth);

		return line.Draw(d3ddev9, effect, emptyTex.get(), &projMat, blendMode, GetTopLayerPos());
	}

	bool DirectXFigure::DrawRectFrame(RectF rect, DWORD color, float lineWidth) {
		return false;
	}

	bool DirectXFigure::DrawRectFrame(float x, float y, float w, float h, DWORD color, float lineWidth) {
		return false;
	}

	bool DirectXFigure::DrawRect(RectF rect, DWORD color) {
		return false;
	}

	bool DirectXFigure::DrawRect(float x, float y, float w, float h, DWORD color) {
		return false;
	}

	bool DirectXFigure::DrawCircleFrame(float x, float y, float w, float h, DWORD color, float lineWidth) {
		return false;
	}

	bool DirectXFigure::DrawCircle(float x, float y, float w, DWORD color, float lineWidth) {
		return false;
	}
}