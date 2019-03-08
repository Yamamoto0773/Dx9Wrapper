#pragma once


#include "DXDrawManager.hpp"
#include "DXTextureBase.hpp"
#include <array>

namespace dx9 {

	namespace figure {

		class Figure {

		protected:
			std::array<float, 4> colorRGBA;	// a value is 0.0f - 1.0f
			float rotRad;

			Figure();
			virtual ~Figure();

		public:
			void SetColor(const Color &color);
		
			void SetRot(int deg);

			virtual bool Draw(
				IDirect3DDevice9 * dev,
				ID3DXEffect * effect,
				IDirect3DVertexBuffer9 *vtx,
				D3DXMATRIX * projMat,
				BLENDMODE blendMode,
				size_t layerPos
				) = 0;
		};




		class Line : public Figure {

		protected:
			PointF begin;
			Vec2 vec;

			float length;

			float lineWidth;

		public:
			Line();
			~Line();

			void SetRot(int) = delete;

			void SetPos(const PointF& begin, const PointF& end);
			void SetLineWidth(float lineWidth);

			bool Draw(
				IDirect3DDevice9 * dev,
				ID3DXEffect * effect,
				IDirect3DVertexBuffer9 *vtx,
				D3DXMATRIX * projMat,
				BLENDMODE blendMode,
				size_t layerPos
				) override;
		};


	
		class Rect : public Figure {

		protected:
			PointF topLeft;
			float w, h;

		public:
			Rect();
			~Rect();

			void SetPos(float x, float y, float w, float h);
			
			virtual bool Draw(
				IDirect3DDevice9 * dev,
				ID3DXEffect * effect,
				IDirect3DVertexBuffer9 *vtx,
				D3DXMATRIX * projMat,
				BLENDMODE blendMode,
				size_t layerPos
				) override;

		};



		class RectFrame : public Rect {

		private:
		
			float lineWidth;


		public:
			RectFrame();
			~RectFrame();

			void SetLineWidth(float lineWidth);

			bool Draw(
				IDirect3DDevice9 * dev,
				ID3DXEffect * effect,
				IDirect3DVertexBuffer9 *vtx,
				D3DXMATRIX * projMat,
				BLENDMODE blendMode,
				size_t layerPos
				) override;

		};


		class Circle : public Figure {
		protected :
			PointF center;
			float w, h;

		public :
			Circle();
			~Circle();

			void SetPos(const PointF& center, float w, float h);
			
			bool Draw(
				IDirect3DDevice9 * dev,
				ID3DXEffect * effect,
				IDirect3DVertexBuffer9 *vtx,
				D3DXMATRIX * projMat,
				BLENDMODE blendMode,
				size_t layerPos
				) override;

		};



		class CircleFrame : public Circle {
		private:
			float lineWidth;

		public:
			CircleFrame();
			~CircleFrame();

			void setLineWidth(float lineWidth);

			bool Draw(
				IDirect3DDevice9 * dev,
				ID3DXEffect * effect,
				IDirect3DVertexBuffer9 *vtx,
				D3DXMATRIX * projMat,
				BLENDMODE blendMode,
				size_t layerPos
				) override;
		};


	}



}