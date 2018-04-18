#include "DirectXFigure.hpp"

#include <math.h>

namespace dx9 {

	DirectXFigure::DirectXFigure() : emptyTex(std::make_unique<EmptyTexture>()) {
	}

	DirectXFigure::~DirectXFigure() {
	}


	bool DirectXFigure::Create() {
		if (emptyTex->GetPointer()) {
			return true;
		}

		bool r = emptyTex->Create(d3ddev9, 1, 1);
		if (!r) {
			return false;
		}

		D3DLOCKED_RECT rect;
		emptyTex->Lock(&rect);
		*(static_cast<DWORD*>(rect.pBits)) = 0xffffffff;	// 完全不透明の白に書き換え

		return true;
	}


	bool DirectXFigure::DrawLine(PointF &begin, PointF &end, DWORD color, float lineWidth) {
		if (lineWidth < 0.0f) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::Line line;
		
		line.SetPos(begin, end);
		line.SetColor(color);
		line.SetLineWidth(lineWidth);

		return line.Draw(d3ddev9, effect, emptyTex.get(), &projMat, blendMode, GetTopLayerPos());
	}

	bool DirectXFigure::DrawLine(float begin_x, float begin_y, float end_x, float end_y, DWORD color, float lineWidth) {
		if (!isDrawable()) {
			return false;
		}
		
		PointF begin = {begin_x, begin_y};
		PointF end = {end_x, end_y};
		return DrawLine(begin, end, color, lineWidth);
	}


	bool DirectXFigure::DrawRectFrame(PointF &topLeft, PointF &bottomRight, DWORD color, float lineWidth) {
		if (lineWidth < 0.0f) {
			return false;
		}
		if (topLeft.x > bottomRight.x || topLeft.y > bottomRight.y) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::RectFrame rectframe;

		rectframe.SetPos(topLeft, bottomRight);
		rectframe.SetColor(color);
		rectframe.SetLineWidth(lineWidth);

		return rectframe.Draw(d3ddev9, effect, emptyTex.get(), &projMat, blendMode, GetTopLayerPos());
	}

	bool DirectXFigure::DrawRectFrame(float x, float y, float w, float h, DWORD color, float lineWidth) {
		if (lineWidth < 0.0f) {
			return false;
		}
		if (w < 0.0f || h < 0.0f) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::RectFrame rectframe;

		rectframe.SetPos(x, y, w, h);
		rectframe.SetColor(color);
		rectframe.SetLineWidth(lineWidth);

		return rectframe.Draw(d3ddev9, effect, emptyTex.get(), &projMat, blendMode, GetTopLayerPos());
	}

	bool DirectXFigure::DrawRectFrame(RectF &rect, DWORD color, float lineWidth) {
		if (lineWidth < 0.0f) {
			return false;
		}
		if (rect.left > rect.right || rect.top > rect.bottom) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::RectFrame rectframe;

		rectframe.SetPos(rect);
		rectframe.SetColor(color);
		rectframe.SetLineWidth(lineWidth);

		return rectframe.Draw(d3ddev9, effect, emptyTex.get(), &projMat, blendMode, GetTopLayerPos());
	}


	bool DirectXFigure::DrawRect(RectF & rect, DWORD color) {

		if (rect.left > rect.right || rect.top > rect.bottom) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::Rect rectframe;

		rectframe.SetPos(rect);
		rectframe.SetColor(color);
		
		return rectframe.Draw(d3ddev9, effect, emptyTex.get(), &projMat, blendMode, GetTopLayerPos());

	}

	bool DirectXFigure::DrawRect(PointF & topLeft, PointF & bottomRight, DWORD color) {
		
		if (topLeft.x > bottomRight.x || topLeft.y > bottomRight.y) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::Rect rectframe;

		rectframe.SetPos(topLeft, bottomRight);
		rectframe.SetColor(color);

		return rectframe.Draw(d3ddev9, effect, emptyTex.get(), &projMat, blendMode, GetTopLayerPos());

	}

	bool DirectXFigure::DrawRect(float x, float y, float w, float h, DWORD color) {
		if (w < 0.0f || h < 0.0f) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::Rect rectframe;

		rectframe.SetPos(x, y, w, h);
		rectframe.SetColor(color);
	
		return rectframe.Draw(d3ddev9, effect, emptyTex.get(), &projMat, blendMode, GetTopLayerPos());

	}


	

	bool DirectXFigure::DrawCircleFrame(float x, float y, float w, float h, DWORD color, float lineWidth) {
		return false;
	}

	bool DirectXFigure::DrawCircle(float x, float y, float w, DWORD color, float lineWidth) {
		return false;
	}
}