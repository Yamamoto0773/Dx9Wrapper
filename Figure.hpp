#pragma once


#include "DXDrawManager.hpp"
#include "DXTextureManager.hpp"


namespace dx9 {

	namespace figure {

		class Figure {

		protected:
			
			Figure();
			virtual ~Figure();

		public:

			virtual bool Draw(
				IDirect3DDevice9 * dev,
				ID3DXEffect * effect,
				const EmptyTexture *tex,
				D3DXMATRIX * projMat,
				BLENDMODE blendMode,
				float layerPos
				) = 0;
		};




		class Line : public Figure {

		private:
			PointF begin;
			D3DXVECTOR2 vec;

			float length;
			float rotRad;

			float colorRGBA[4];
			float lineWidth;

		public:
			Line();
			~Line();

			void SetPos(PointF &begin, PointF &end);
			void SetColor(DWORD color);
			void SetLineWidth(float lineWidth);

			bool Draw(
				IDirect3DDevice9 * dev,
				ID3DXEffect * effect,
				const EmptyTexture *tex,
				D3DXMATRIX * projMat,
				BLENDMODE blendMode,
				float layerPos
				) override;
		};



	}



}