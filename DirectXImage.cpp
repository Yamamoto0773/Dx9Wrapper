#include "DirectXImage.hpp"

#define _USE_MATH_DEFINES
#include <math.h>


namespace dx9 {

	bool DirectXImage::AddTexture(size_t texID, const std::wstring& fileName) {
		if (texID < 0 || texID >= TEXTURE_MAXCNT) {
			return false;
		}

		if (texRes[texID]) {
			texRes[texID]->Delete();
		}

		// 既に生成された画像ファイルであるかチェック
		int resID = GetTexResourceID(fileName);


		// 以下，テクスチャの作成処理

		if (resID == -1) {
			// テクスチャを新規作成して登録
			texRes[texID] = std::make_unique<TextureFile>();
			if (!texRes[texID]->Create(d3ddev9, fileName)) {
				return false;
			}
			texRes[texID]->SetName(fileName);
		}
		else {
			// テクスチャをコピーして登録
			texRes[texID] = std::make_unique<TextureFile>();
			texRes[resID]->CopyTo(*texRes[texID]);	// テクスチャのコピー
		}


		// 切り抜き情報を保存
		Size size = {texRes[texID]->GetWidth(), texRes[texID]->GetHeight()};
		texRes[texID]->SetClipSize(size);
		UVCoord uv = {0.0f, 0.0f, 1.0f, 1.0f};
		texRes[texID]->SetClipUV(uv);


		return true;
	}


	bool DirectXImage::AddTexture(size_t texID, const std::wstring& fileName, size_t x, size_t y, size_t w, size_t h) {
		if (texID < 0 || texID >= TEXTURE_MAXCNT) {
			return false;
		}

		if (texRes[texID]) {
			texRes[texID]->Delete();
		}

		// 既に生成された画像ファイルであるかチェック
		int resID = GetTexResourceID(fileName);


		// 以下，テクスチャの作成処理

		if (resID == -1) {
			// テクスチャを新規作成して登録
			texRes[texID] = std::make_unique<TextureFile>();
			if (!texRes[texID]->Create(d3ddev9, fileName)) {
				return false;
			}
			texRes[texID]->SetName(fileName);
		}
		else {
			// テクスチャをコピーして登録
			texRes[texID] = std::make_unique<TextureFile>();
			texRes[resID]->CopyTo(*texRes[texID]);	// テクスチャのコピー
		}


		// 切り抜き情報を保存
		Size size = {w, h};
		texRes[texID]->SetClipSize(size);

		UVCoord uv = {
			(float)x/texRes.at(texID)->GetWidth(),
			(float)y/texRes.at(texID)->GetHeight(),
			(float)w/texRes.at(texID)->GetWidth(),
			(float)h/texRes.at(texID)->GetHeight()
		};
		texRes[texID]->SetClipUV(uv);

		return true;
	}


	
	bool dx9::DirectXImage::Draw(size_t texID, float x, float y, DrawTexCoord coord, float alpha, float xscale, float yscale, int rotDeg) {
		if (texID < 0 || texID >= TEXTURE_MAXCNT) {
			return false;
		}
		if (!texRes[texID]) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		rotDeg%=360;
		float rotRad = (float)rotDeg*M_PI/180;
		float colorRGBA[4] = {1.0f, 1.0f, 1.0f, alpha};
		

		// 描画位置の算出
		PointF pos;
		Size size = {texRes[texID]->GetWidth(), texRes[texID]->GetHeight()};
		switch (coord) {
			case DrawTexCoord::TOP_L:
				pos.x	= x + size.w*(xscale-1.0f)/2.0f;
				pos.y	= y + size.h*(yscale-1.0f)/2.0f;
				break;
			case DrawTexCoord::TOP_R:
				pos.x	= x - size.w*(xscale+1.0f)/2.0f;
				pos.y	= y + size.h*(yscale-1.0f)/2.0f;
				break;
			case DrawTexCoord::BOTTOM_R:
				pos.x	= x - size.w*(xscale+1.0f)/2.0f;
				pos.y	= y - size.h*(yscale+1.0f)/2.0f;
				break;
			case DrawTexCoord::BOTTOM_L:
				pos.x	= x + size.w*(xscale-1.0f)/2.0f;
				pos.y	= y - size.h*(yscale+1.0f)/2.0f;
				break;
			case DrawTexCoord::CENTER:
				pos.x	= x - size.w/2.0f;
				pos.y	= y - size.h/2.0f;
				break;
			default:
				break;
		}


		// デフォルトステートのセット
		if (isRightHand)
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);					// 右回りを消去(右手系)
		else
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);					// 左回りを消去(左手系)

		d3ddev9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3ddev9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);


		// シェーダ開始
		UINT numPass = 0;
		effect->SetTechnique("Tech");
		effect->Begin(&numPass, 0);
		effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Mul_UVTex_color));


		// ブレンドモードを設定
		switch (blendMode) {
			case BLENDMODE::NORMAL:
				d3ddev9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				d3ddev9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				break;
			case BLENDMODE::ADD:
				d3ddev9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				d3ddev9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
		}

		TexClip clipInfo = texRes[texID]->GetClipInfo();

		D3DXMATRIX world, scale, rot;
		D3DXMatrixScaling(&world, (float)clipInfo.size.w, (float)clipInfo.size.h, 1.0f);	// ポリゴンサイズに
		D3DXMatrixScaling(&scale, xscale, yscale, 1.0f);	// ローカルスケール
		D3DXMatrixRotationZ(&rot, rotRad);						// 回転
		world._41 = -(clipInfo.size.w/2.0f);		// ピボット分オフセット
		world._42 = -(clipInfo.size.h/2.0f);
		world = world * scale * rot;
		world._41 += pos.x-0.5f + clipInfo.size.w/2.0f;	// ピボット分オフセット
		world._42 += pos.y+0.5f + clipInfo.size.h/2.0f;
		world._43 += GetTopLayerPos()/1000.0f;

		effect->SetMatrix("world", &world);
		effect->SetMatrix("proj", &projMat);
		effect->SetTexture("tex", texRes[texID]->GetPointer());
		effect->SetFloat("uv_left", clipInfo.uv.left);
		effect->SetFloat("uv_top", clipInfo.uv.top);
		effect->SetFloat("uv_width", clipInfo.uv.w);
		effect->SetFloat("uv_height", clipInfo.uv.h);
		effect->SetFloatArray("color", colorRGBA, 4);
		effect->CommitChanges();
		d3ddev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		effect->EndPass();
		effect->End();

		return true;
	}



	int DirectXImage::GetTexResourceID(const std::wstring& fileName) {
		for (size_t i=0; i<texRes.size(); i++) {
			if (texRes[i] && texRes[i]->GetName() == fileName) {
				return i;
			}
		}

		return -1;
	}




	bool DirectXImage::DelTexture(size_t texID) {
		if (texID < 0 || texID >= TEXTURE_MAXCNT) {
			return false;
		}

		if (texRes[texID]) {
			texRes[texID]->Delete();
		}

		return true;
	}

	

}