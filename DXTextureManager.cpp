#include "DXTextureManager.hpp"

#define _USE_MATH_DEFINES
#include <math.h>


namespace dx9 {


	DXTextureManager::DXTextureManager() :
		drawTexCoord(DrawTexCoord::TOP_L),
		filterColor({ 1.0f, 1.0f, 1.0f }),
		filterBlendMode(BLENDMODE::NORMAL),
		textureAdjust(TextureAdjust::ASPECT_FIXED) {}

	DXTextureManager::~DXTextureManager() {}



	bool DXTextureManager::CreateFromFile(Texture &tex, const std::wstring& fileName) {
		return CreateFromFile(tex, fileName, { 0, 0, 0, 0 });
	}


	bool DXTextureManager::CreateFromFile(Texture &tex, const std::wstring& fileName, ClipArea clipArea) {

		// 既に生成された画像ファイルであるかチェック
		auto texPos = FindTexture(fileName);

		if (texPos == texPool.end()) {
			// テクスチャを新規作成

			texPos = FindEmptyPool();

			if (texPos == texPool.end()) {
				texPool.push_back(std::make_shared<texture::DXTextureBase>());
				texPos = texPool.end() - 1;
			} else {
				*texPos = std::make_shared<texture::DXTextureBase>();
			}

			if (!(*texPos)->Create(d3ddev9, fileName)) {
				return false;
			}
			(*texPos)->SetName(fileName);
		}


		// 切り抜き情報作成
		texture::TexClip clip;
		size_t x = clipArea.upleft_x;
		size_t y = clipArea.upleft_y;
		size_t w = clipArea.size_w;
		size_t h = clipArea.size_h;

		if (!(x && y && w && h)) {

			if ((x+w > (*texPos)->GetWidth()) || (y+h > (*texPos)->GetHeight())) {
				return false;
			}

			clip.size.w = (*texPos)->GetWidth();
			clip.size.h = (*texPos)->GetHeight();
			clip.uv = { 0.0f, 0.0f, 1.0f, 1.0f };
		} else {
			clip.size = { w, h };
			clip.uv = {
				(float)x/(*texPos)->GetWidth(),
				(float)y/(*texPos)->GetHeight(),
				(float)w/(*texPos)->GetWidth(),
				(float)h/(*texPos)->GetHeight()
			};
		}


		// 書き換え
		tex.set(*texPos, clip);


		return true;
	}

	bool DXTextureManager::CreateEmptyTex(Texture &tex, size_t w, size_t h) {
		using namespace texture;

		std::shared_ptr<DXTextureBase> emptyTex(std::make_shared<DXTextureBase>());
		if (!emptyTex->Create(d3ddev9, w, h))
			return false;

		TexClip clip;
		clip.size = { w, h };
		clip.uv = { 0.0f, 0.0f, 1.0f, 1.0f };

		tex.set(emptyTex, clip);

		return true;
	}

	bool DXTextureManager::CreateFromD3DTex9(Texture & tex, const texture::DXTextureBase &texbase) {
		if (texbase.GetPointer() == nullptr) return false;

		D3DSURFACE_DESC desc;
		texbase.GetPointer()->GetLevelDesc(0, &desc);

		texture::TexClip clip;
		clip.size.w = desc.Width;
		clip.size.h = desc.Height;
		clip.uv = { 0.0f, 0.0f, 1.0f, 1.0f };

		tex.set(std::make_shared<texture::DXTextureBase>(texbase), clip);

		return true;
	}



	bool DXTextureManager::DrawTexture(Texture & tex, float x, float y, float scale_x, float scale_y, float alpha, int rotDeg, bool isClip) {
		if (!tex) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}


		// 描画位置の算出
		PointF pos;
		Size size = tex.getSize();
		switch (drawTexCoord) {
		case DrawTexCoord::TOP_L:
			pos.x = x + size.w*(scale_x-1.0f)/2.0f;
			pos.y = y + size.h*(scale_y-1.0f)/2.0f;
			break;
		case DrawTexCoord::TOP_R:
			pos.x = x - size.w*(scale_x+1.0f)/2.0f;
			pos.y = y + size.h*(scale_y-1.0f)/2.0f;
			break;
		case DrawTexCoord::BOTTOM_R:
			pos.x = x - size.w*(scale_x+1.0f)/2.0f;
			pos.y = y - size.h*(scale_y+1.0f)/2.0f;
			break;
		case DrawTexCoord::BOTTOM_L:
			pos.x = x + size.w*(scale_x-1.0f)/2.0f;
			pos.y = y - size.h*(scale_y+1.0f)/2.0f;
			break;
		case DrawTexCoord::CENTER:
			pos.x = x - size.w/2.0f;
			pos.y = y - size.h/2.0f;
			break;
		default:
			break;
		}


		UVCoord uv = tex.clipInfo.uv;

		return DrawTexture_main(tex, pos.x, pos.y, uv.left, uv.top, uv.w, uv.h, scale_x, scale_y, alpha, rotDeg, isClip);
	}

	bool DXTextureManager::DrawTexture(Texture & tex, RectF posArea, float alpha, int rotDeg, bool isClip) {
		if (!tex) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		float pos_w = posArea.right - posArea.left;
		float pos_h = posArea.bottom - posArea.top;


		if (pos_w <= 0.0f)				return false;
		if (pos_h <= 0.0f)				return false;


		ClipArea clipArea;
		clipArea.upleft_x = 0;
		clipArea.upleft_y = 0;
		clipArea.size_w = tex.getSize().w;
		clipArea.size_h = tex.getSize().h;


		return DrawTexture(tex, posArea, clipArea, alpha, rotDeg, isClip);
	}

	bool DXTextureManager::DrawTexture(Texture &tex, RectF posArea, ClipArea clipArea, float alpha, int rotDeg, bool isClip) {
		if (!tex) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}

		float pos_w = posArea.right - posArea.left;
		float pos_h = posArea.bottom - posArea.top;


		if (pos_w <= 0.0f)				return false;
		if (pos_h <= 0.0f)				return false;
		if (clipArea.size_w == 0.0)		return false;
		if (clipArea.size_h == 0.0)		return false;
		if (clipArea.upleft_x + clipArea.size_w > tex.getSize().w)	return false;
		if (clipArea.upleft_y + clipArea.size_h > tex.getSize().h)	return false;



		// set finally texture scale 			
		float xscale = (float)pos_w/clipArea.size_w;
		float yscale = (float)pos_h/clipArea.size_h;



		// set finally texture position
		switch (textureAdjust) {
		case dx9::TextureAdjust::NONE:
			xscale = yscale = 1.0f;
			break;
		case dx9::TextureAdjust::ASPECT_UNFIXED:
		case dx9::TextureAdjust::ASPECT_UNFIXED_REDUCEONLY:
			break;
		case dx9::TextureAdjust::ASPECT_FIXED:
		case dx9::TextureAdjust::ASPECT_FIXED_REDUCEONLY:
			if (xscale > yscale) {
				xscale = yscale;
			} else {
				yscale = xscale;
			}
			break;
		}

		if (textureAdjust == TextureAdjust::ASPECT_FIXED_REDUCEONLY ||
			textureAdjust == TextureAdjust::ASPECT_UNFIXED_REDUCEONLY) {

			if (xscale < 1.0f && yscale < 1.0f) {
				xscale = 1.0f;
				yscale = 1.0f;
			}
		}


		PointF pos = {
			posArea.left + (pos_w - clipArea.size_w*xscale)/2.0f,
			posArea.top + (pos_h - clipArea.size_h*yscale)/2.0f
		};



		UVCoord uv = tex.clipInfo.uv;
		uv.left += (float)clipArea.upleft_x/tex.getSize().w;
		uv.top += (float)clipArea.upleft_y/tex.getSize().h;
		uv.w = (float)clipArea.size_w/tex.getSize().w;
		uv.h = (float)clipArea.size_h/tex.getSize().h;



		return DrawTexture_main(tex, pos.x, pos.y, uv.left, uv.top, uv.w, uv.h, xscale, yscale, alpha, rotDeg, isClip);
	}

	int DXTextureManager::CleanTexPool() {
		int releasedCnt = 0;
		for (size_t i = 0; i<texPool.size(); i++) {
			if (texPool[i] && texPool[i].use_count() == 1) {
				texPool[i].reset();
				releasedCnt++;
			}
		}

		return releasedCnt;
	}

	void DXTextureManager::SetColorFilter(Color &color, BLENDMODE blendmode) {
		auto ary = color.getRGBAFloat();
		filterColor = {
			ary[0], ary[1], ary[2]
		};
		filterBlendMode = blendmode;
	}

	std::pair<ColorRGB, BLENDMODE> DXTextureManager::GetColorFilterRGB() {
		return std::pair<ColorRGB, BLENDMODE>(
			{ColorRGB(filterColor[0], filterColor[1], filterColor[2]), blendMode }
		);
	}

	std::pair<ColorHSB, BLENDMODE> DXTextureManager::GetColorFilterHSB() {
		auto ary = ColorRGB(filterColor[0], filterColor[1], filterColor[2]).getColorHSB().getFloat();
		return std::pair<ColorHSB, BLENDMODE>(
			{ ColorHSB(ary.h, ary.s, ary.b), blendMode }
		);
	}

	void DXTextureManager::RemoveColorFilter() {
		SetColorFilter(ColorRGB(255, 255, 255), BLENDMODE::NORMAL);
	}



	bool DXTextureManager::DrawTexture_main(Texture & tex, float x, float y, float uv_left, float uv_top, float uv_w, float uv_h, float scale_x, float scale_y, float alpha, int rotDeg, bool isClip) {
		if (!tex) {
			return false;
		}
		if (!isDrawable()) {
			return false;
		}


		// デフォルトステートのセット
		if (isRightHand)
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);					// 右回りを消去(右手系)
		else
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);					// 左回りを消去(左手系)

		d3ddev9->SetSamplerState(0, D3DSAMP_MINFILTER, static_cast<DWORD>(texFilter));
		d3ddev9->SetSamplerState(0, D3DSAMP_MAGFILTER, static_cast<DWORD>(texFilter));

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

		// 板ポリゴンを登録
		d3ddev9->SetStreamSource(0, vertex_rect, 0, sizeof(float)*5);



		// シェーダ開始
		UINT numPass = 0;
		effect->SetTechnique("Tech");
		effect->Begin(&numPass, 0);


		// set shader  pass
		if (isClip) {
			effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Mul_ColorAlpha_UVTexAlpha));
		} else {
			switch (filterBlendMode) {
			case dx9::BLENDMODE::NORMAL:
				effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Mul_UVTex_color));
				break;
			case dx9::BLENDMODE::ADD:
				effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Add_UVTex_color));
				break;
			default:
				break;
			}
		}




		D3DXMATRIX world, rot;

		// create parameter for shader
		rotDeg %= 360;
		float rotRad = (float)(rotDeg*M_PI/180);

		std::array<float, 4> color_rgba = {
			filterColor[0], filterColor[1], filterColor[2], alpha
		};
		Size size;
		size.w = (size_t)(tex.getSize().w*uv_w*scale_x);
		size.h = (size_t)(tex.getSize().h*uv_h*scale_y);

		D3DXMatrixScaling(&world, (float)size.w, (float)size.h, 1.0f);	// ポリゴンサイズに
		D3DXMatrixRotationZ(&rot, rotRad);						// 回転
		world._41 = -(size.w/2.0f);		// ピボット分オフセット
		world._42 = -(size.h/2.0f);
		world = world * rot;
		world._41 += x-0.5f + size.w/2.0f;	// ピボット分オフセット
		world._42 += y+0.5f + size.h/2.0f;
		world._43 += topLayerPos/1000.0f;

		effect->SetMatrix("world", &world);
		effect->SetMatrix("proj", &projMat);
		effect->SetTexture("tex", tex.tex->GetPointer());
		effect->SetFloat("uv_left", uv_left);
		effect->SetFloat("uv_top", uv_top);
		effect->SetFloat("uv_width", uv_w);
		effect->SetFloat("uv_height", uv_h);
		effect->SetFloatArray("color", color_rgba.data(), 4);
		effect->CommitChanges();
		d3ddev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		effect->EndPass();
		effect->End();

		return true;
	}



	tex_vector::iterator DXTextureManager::FindTexture(const std::wstring& fileName) {
		for (auto it = texPool.begin(); it!=texPool.end(); it++) {
			if (*it && (*it)->GetName() == fileName) {
				return it;
			}
		}

		return texPool.end();
	}

	tex_vector::iterator DXTextureManager::FindEmptyPool() {
		for (auto it = texPool.begin(); it!=texPool.end(); it++) {
			if (*it == nullptr) {
				return it;
			}
		}

		return texPool.end();
	}


}