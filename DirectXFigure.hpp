#pragma once


#include "Figure.hpp"

namespace dx9 {

	class DirectXFigure : public DXDrawManager {

	private:
		std::unique_ptr<EmptyTexture> emptyTex;
	

	public:
		DirectXFigure();
		~DirectXFigure();

		bool Create();


		// ----------------------------------------------
		// 描画設定

		// ----------------------------------------------
		// 描画関数

		// 直線描画
		bool DrawLine(PointF &begin, PointF &end, DWORD color, float lineWidth=1.0f);
		bool DrawLine(float begin_x, float begin_y, float end_x, float end_y, DWORD color, float lineWidth=1.0f);

		// 長方形の輪郭の描画
		bool DrawRectFrame(RectF &rect, DWORD color, float lineWidth=1.0f);
		bool DrawRectFrame(PointF &topLeft, PointF &bottomRight, DWORD color, float lineWidth=1.0f);
		bool DrawRectFrame(float x, float y, float w, float h, DWORD color, float lineWidth=1.0f);

		// 長方形の描画
		bool DrawRect(RectF &rect, DWORD color);
		bool DrawRect(PointF &topLeft, PointF &bottomRight, DWORD color);
		bool DrawRect(float x, float y, float w, float h, DWORD color);
		

		// 円の輪郭の描画
		bool DrawCircleFrame(float x, float y, float w, float h, DWORD color, float lineWidth=1.0f);

		// 円の描画
		bool DrawCircle(float x, float y, float w, DWORD color, float lineWidth=1.0f);



	};



}