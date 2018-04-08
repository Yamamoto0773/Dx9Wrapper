
#include "Figure.hpp"


namespace dx9 {

	namespace figure {

		

		Figure::Figure() {
		}

		Figure::~Figure() {
		}



		Line::Line() :
			begin({0.0f, 0.0f}),
			vec(1.0f, 1.0f),
			length(1.0f),
			rotRad(0.0f),
			colorRGBA(),
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

		void Line::SetColor(DWORD color) {
			// 指定色をfloat型配列に変換
			colorRGBA[0] = (color>>16 & 0x000000ff) / 255.0f;
			colorRGBA[1] = (color>>8 & 0x000000ff) /255.0f;
			colorRGBA[2] = (color & 0x000000ff) / 255.0f;
			colorRGBA[3] = (color>>24 & 0x000000ff) / 255.0f;
		}

		void Line::SetLineWidth(float lineWidth) {
			this->lineWidth = lineWidth;
		}


		bool Line::Draw(IDirect3DDevice9 *dev, ID3DXEffect *effect, const EmptyTexture *tex, D3DXMATRIX *projMat, BLENDMODE blendMode, float layerPos) {
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


			// シェーダ開始
			UINT numPass = 0;
			effect->SetTechnique("Tech");
			effect->Begin(&numPass, 0);
			effect->BeginPass(0);


			D3DXMATRIX world, scale, rot;
			D3DXMatrixScaling(&world, (float)length, lineWidth, 1.0f);	// ポリゴンサイズに
			D3DXMatrixScaling(&scale, 1.0f, 1.0f, 1.0f);	// ローカルスケール
			D3DXMatrixRotationZ(&rot, rotRad);						// 回転
			world._41 = 0.0f;
			world._42 = 0.0f;
			world = world * scale * rot;
			world._41 += begin.x -0.5f;
			world._42 += begin.y +0.5f;
			world._43 += layerPos/1000.0f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", projMat);
			effect->SetTexture("tex", tex->GetPointer());
			effect->SetFloat("uv_left", 0.0f);
			effect->SetFloat("uv_top", 0.0f);
			effect->SetFloat("uv_width", 1.0f);
			effect->SetFloat("uv_height", 1.0f);
			effect->SetFloatArray("color", colorRGBA, 4);
			effect->CommitChanges();
			dev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			effect->EndPass();
			effect->End();

			return true;
		}

	}
	

}