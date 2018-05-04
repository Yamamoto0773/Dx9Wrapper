#include "DirectXFigure.hpp"

#include <math.h>

namespace dx9 {

	DirectXFigure::DirectXFigure() {
	}

	DirectXFigure::~DirectXFigure() {
	}

	bool DirectXFigure::DrawLine(float begin_x, float begin_y, float end_x, float end_y, DWORD color, float lineWidth) {
		PointF begin = {begin_x, begin_y};
		PointF end = {end_x, end_y};
		return DrawLine(begin, end, color, lineWidth);
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

		return line.Draw(d3ddev9, effect, vertex_rect, &projMat, blendMode, topLayerPos);
	}

	




	bool DirectXFigure::DrawRectFrame(PointF &topLeft, PointF &bottomRight, DWORD color, float lineWidth) {
		return DrawRectFrame(topLeft.x, topLeft.y, bottomRight.x-topLeft.x, bottomRight.y-topLeft.y, color, lineWidth);
	}

	bool DirectXFigure::DrawRectFrame(RectF &rect, DWORD color, float lineWidth) {
		return DrawRectFrame(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, color, lineWidth);
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

		return rectframe.Draw(d3ddev9, effect, vertex_rect, &projMat, blendMode, topLayerPos);
	}

	



	bool DirectXFigure::DrawRect(RectF & rect, DWORD color) {
		return DrawRect(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, color);
	}

	bool DirectXFigure::DrawRect(PointF & topLeft, PointF & bottomRight, DWORD color) {
		return DrawRect(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y, color);
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
	
		return rectframe.Draw(d3ddev9, effect, vertex_rect, &projMat, blendMode, topLayerPos);

	}




	bool DirectXFigure::DrawCircleFrame(RectF & rectArea, DWORD color, float lineWidth) {
		return DrawCircleFrame(
			(rectArea.left+rectArea.right)/2.0f,
			(rectArea.top+rectArea.bottom)/2.0f, 
			rectArea.right-rectArea.left,
			rectArea.bottom-rectArea.top,
			color,
			lineWidth
			);
	}

	bool DirectXFigure::DrawCircleFrame(float x, float y, float w, float h, DWORD color, float lineWidth) {
		if (w < 0.0f || h < 0.0f) {
			return false;
		}
		if (lineWidth < 0.0f) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::CircleFrame circleFrame;
		circleFrame.SetPos({x, y}, w, h);
		circleFrame.SetColor(color);
		circleFrame.setLineWidth(lineWidth);

		return circleFrame.Draw(d3ddev9, effect, vertex_circle, &projMat, blendMode, topLayerPos);
	}

	





	bool DirectXFigure::DrawCircle(RectF &rectArea, DWORD color) {
		return DrawCircle(
			(rectArea.left+rectArea.right)/2.0f,
			(rectArea.top+rectArea.bottom)/2.0f, 
			rectArea.right-rectArea.left,
			rectArea.bottom-rectArea.top,
			color
			);
	}

	bool DirectXFigure::DrawCircle(float x, float y, float w, float h, DWORD color) {
		if (w < 0.0f || h < 0.0f) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		figure::Circle circle;
		circle.SetPos({x, y}, w, h);
		circle.SetColor(color);

		return circle.Draw(d3ddev9, effect, vertex_circle, &projMat, blendMode, topLayerPos);
	}

	

}