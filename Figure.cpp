
#include "Figure.hpp"
#include "template.hpp"

#define _USE_MATH_DEFINES
#include <math.h>  


namespace dx9 {

	namespace figure {

		

		Figure::Figure() :
			colorRGBA({0.0f, 0.0f, 0.0f, 1.0f}),
			rotRad(0.0f) {
		}

		Figure::~Figure() {
		}

		void Figure::SetColor(DWORD color) {
			colorRGBA = {
				(color>>16 & 0xff) / 255.0f,
				(color>>8 & 0xff) /255.0f,
				(color & 0xff) / 255.0f,
				(color>>24 & 0xff) / 255.0f
			};
		}


		void Figure::SetRot(int deg) {
			deg%=360;
			rotRad = M_PI*deg/180.0f;
		}



		Line::Line() :
			begin({0.0f, 0.0f}),
			vec(1.0f, 1.0f),
			length(1.0f),
			lineWidth(1.0f) {

		}

		Line::~Line() {
		}

		void Line::SetPos(PointF & begin, PointF & end) {
			this->begin = begin;
			vec = {end.x - begin.x, end.y - begin.y};

			// 線の傾きを求める
			rotRad = atan2(vec.x, vec.y);

			// 線の長さを求める
			length = D3DXVec2Length(&vec);

		}

		void Line::SetLineWidth(float lineWidth) {
			this->lineWidth = lineWidth;
		}


		bool Line::Draw(IDirect3DDevice9 *dev, ID3DXEffect *effect, IDirect3DVertexBuffer9 *vtx, D3DXMATRIX *projMat, BLENDMODE blendMode, float layerPos) {
			// ブレンドモードを設定
			switch (blendMode) {
				case BLENDMODE::NORMAL:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
					break;
				case BLENDMODE::ADD:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
					break;
			}

			// 板ポリゴンを登録
			dev->SetStreamSource(0, vtx, 0, sizeof(float)*5);

			// シェーダ開始
			UINT numPass = 0;
			effect->SetTechnique("Tech");
			effect->Begin(&numPass, 0);
			effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Color));


			D3DXMATRIX world, rot;
			D3DXMatrixScaling(&world, (float)length, lineWidth, 1.0f);	// ポリゴンサイズに
			D3DXMatrixRotationZ(&rot, rotRad);						// 回転
			world._41 = -length/2.0f;
			world._42 = -lineWidth/2.0f;
			world = world * rot;
			world._41 += length/2.0f + begin.x;
			world._42 += lineWidth/2.0f + begin.y;
			world._43 += layerPos/1000.0f;

			// ラスタライゼーションルールを用いて，テクスチャをずらす
			world._41 = ceil(world._41) - 0.5f;
			world._42 = floor(world._42) + 0.5f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", projMat);
			effect->SetFloatArray("color", colorRGBA.data(), 4);
			effect->CommitChanges();
			dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			effect->EndPass();
			effect->End();

			return true;
		}








		Rect::Rect() : 
			topLeft({0, 0}),
			w(0),
			h(0) {
		}

		Rect::~Rect() {
		}



		void Rect::SetPos(float x, float y, float w, float h) {
			this->topLeft = {x, y};
			this->w = w;
			this->h = h;
		}


		bool Rect::Draw(IDirect3DDevice9 * dev, ID3DXEffect * effect, IDirect3DVertexBuffer9 *vtx, D3DXMATRIX * projMat, BLENDMODE blendMode, float layerPos) {
			// ブレンドモードを設定
			switch (blendMode) {
				case BLENDMODE::NORMAL:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
					break;
				case BLENDMODE::ADD:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
					break;
			}

			// 板ポリゴンを登録
			dev->SetStreamSource(0, vtx, 0, sizeof(float)*5);

			// シェーダ開始
			UINT numPass = 0;
			effect->SetTechnique("Tech");
			effect->Begin(&numPass, 0);
			effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Color));


			D3DXMATRIX world, rot;
			D3DXMatrixScaling(&world, (float)w, h, 1.0f);	// ポリゴンサイズに
			D3DXMatrixRotationZ(&rot, rotRad);						// 回転
			world._41 = -w/2.0f;
			world._42 = -h/2.0f;
			world = world * rot;
			world._41 += w/2.0f + topLeft.x;
			world._42 += h/2.0f + topLeft.y;
			world._43 += layerPos/1000.0f;

			// ラスタライゼーションルールを用いて，テクスチャをずらす
			world._41 = ceil(world._41) - 0.5f;
			world._42 = floor(world._42) + 0.5f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", projMat);
			effect->SetFloatArray("color", colorRGBA.data(), 4);
			effect->CommitChanges();
			dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			effect->EndPass();
			effect->End();

			return true;
		}





		RectFrame::RectFrame() : lineWidth(1.0f) {
		}

		RectFrame::~RectFrame() {
		}


		void RectFrame::SetLineWidth(float lineWidth) {
			this->lineWidth = lineWidth;
		}


		bool RectFrame::Draw(IDirect3DDevice9 * dev, ID3DXEffect * effect, IDirect3DVertexBuffer9 *vtx, D3DXMATRIX * projMat, BLENDMODE blendMode, float layerPos) {
			// ブレンドモードを設定
			switch (blendMode) {
				case BLENDMODE::NORMAL:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
					break;
				case BLENDMODE::ADD:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
					break;
			}

			// 板ポリゴンを登録
			dev->SetStreamSource(0, vtx, 0, sizeof(float)*5);

			// シェーダ開始
			UINT numPass = 0;
			effect->SetTechnique("Tech");
			effect->Begin(&numPass, 0);
			effect->BeginPass(static_cast<UINT>(shader::ShaderPass::RectFrame));


			D3DXMATRIX world, rot;
			D3DXMatrixScaling(&world, (float)w, h, 1.0f);	// ポリゴンサイズに
			D3DXMatrixRotationZ(&rot, rotRad);						// 回転
			world._41 = -w/2.0f;
			world._42 = -h/2.0f;
			world = world * rot;
			world._41 += w/2.0f + topLeft.x;
			world._42 += h/2.0f + topLeft.y;
			world._43 += layerPos/1000.0f;

			// ラスタライゼーションルールを用いて，テクスチャをずらす
			world._41 = ceil(world._41) - 0.5f;
			world._42 = floor(world._42) + 0.5f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", projMat);
			effect->SetFloatArray("color", colorRGBA.data(), 4);
			effect->SetFloat("frameWeight_u", lineWidth/w);
			effect->SetFloat("frameWeight_v", lineWidth/h);
			effect->CommitChanges();
			dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			effect->EndPass();
			effect->End();

			return true;
		}



		Circle::Circle() :
			center({0.0f, 0.0f}),
			w(0),
			h(0) {
		}

		Circle::~Circle() {
		}

		void Circle::SetPos(PointF center, float w, float h) {
			this->center = center;
			this->w = w;
			this->h = h;
		}


		bool Circle::Draw(IDirect3DDevice9 * dev, ID3DXEffect * effect, IDirect3DVertexBuffer9 * vtx, D3DXMATRIX * projMat, BLENDMODE blendMode, float layerPos) {
			// ブレンドモードを設定
			switch (blendMode) {
				case BLENDMODE::NORMAL:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
					break;
				case BLENDMODE::ADD:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
					break;
			}

			// 板ポリゴンを登録
			dev->SetStreamSource(0, vtx, 0, sizeof(float)*5);

			// シェーダ開始
			UINT numPass = 0;
			effect->SetTechnique("Tech");
			effect->Begin(&numPass, 0);
			effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Color));


			D3DXMATRIX world, rot;
			D3DXMatrixScaling(&world, (float)w, h, 1.0f);	// ポリゴンサイズに
			D3DXMatrixRotationZ(&rot, rotRad);						// 回転
			world._41 = -w/2.0f;
			world._42 = -h/2.0f;
			world = world * rot;
			world._41 += w/2.0f + center.x;
			world._42 += h/2.0f + center.y;
			world._43 += layerPos/1000.0f;

			// ラスタライゼーションルールを用いて，テクスチャをずらす
			world._41 = ceil(world._41) - 0.5f;
			world._42 = floor(world._42) + 0.5f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", projMat);
			effect->SetFloatArray("color", colorRGBA.data(), 4);
			effect->CommitChanges();
			dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resource::CIRCLE_VERTEXCNT);

			effect->EndPass();
			effect->End();

			return true;
		}





		CircleFrame::CircleFrame() {
		}

		CircleFrame::~CircleFrame() {
		}

		void CircleFrame::setLineWidth(float lineWidth) {
			this->lineWidth = lineWidth;
		}

		bool CircleFrame::Draw(IDirect3DDevice9 * dev, ID3DXEffect * effect, IDirect3DVertexBuffer9 * vtx, D3DXMATRIX * projMat, BLENDMODE blendMode, float layerPos) {
			// ブレンドモードを設定
			switch (blendMode) {
				case BLENDMODE::NORMAL:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
					break;
				case BLENDMODE::ADD:
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
					break;
			}

			// 板ポリゴンを登録
			dev->SetStreamSource(0, vtx, 0, sizeof(float)*5);

			// シェーダ開始
			UINT numPass = 0;
			effect->SetTechnique("Tech");
			effect->Begin(&numPass, 0);
			effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Color));
		

			D3DXMATRIX world, rot;
			D3DXMatrixScaling(&world, (float)w, h, 1.0f);	// ポリゴンサイズに
			D3DXMatrixRotationZ(&rot, rotRad);						// 回転
			world._41 = -w/2.0f;
			world._42 = -h/2.0f;
			world = world * rot;
			world._41 += w/2.0f + center.x;
			world._42 += h/2.0f + center.y;
			world._43 += layerPos/1000.0f;

			// ラスタライゼーションルールを用いて，テクスチャをずらす
			world._41 = ceil(world._41) - 0.5f;
			world._42 = floor(world._42) + 0.5f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", projMat);
			effect->SetFloatArray("color", colorRGBA.data(), 4);
			float focus[2] = {sqrt(pow(w/2, 2)-pow(h/2, 2)), 0.0f};
			effect->SetFloatArray("focusPt", focus, 2);
			effect->SetFloat("frameWeight", lineWidth);
			effect->SetFloat("circle_w", w);
			effect->SetFloat("circle_h", h);
			effect->CommitChanges();
			dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resource::CIRCLE_VERTEXCNT);

			effect->EndPass();
			effect->End();

			return true;
		}

}
	

}