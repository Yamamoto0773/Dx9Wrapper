#include "DXTextureManager.hpp"

#define _USE_MATH_DEFINES
#include <math.h>


namespace dx9 {

	namespace texture {

		bool DXTextureManager::CreateFromFile(Texture &tex, const std::wstring& fileName) {
			return CreateFromFile(tex, fileName, 0, 0, 0, 0);
		}


		bool DXTextureManager::CreateFromFile(Texture &tex, const std::wstring& fileName, size_t x, size_t y, size_t w, size_t h) {
					
			// 既に生成された画像ファイルであるかチェック
			auto texPos = FindTexture(fileName);
		
			if (texPos == texPool.end()) {
				// テクスチャを新規作成

				texPos = FindEmptyPool();

				if (texPos == texPool.end()) {
					texPool.push_back(std::make_shared<DXTextureBase>());
					texPos = texPool.end() - 1;
				}
				else {
					*texPos = std::make_shared<DXTextureBase>();
				}
				
				if (!(*texPos)->Create(d3ddev9, fileName)) {
					return false;
				}
				(*texPos)->SetName(fileName);
			}


			// 切り抜き情報作成
			TexClip clip;
			if (!(x && y && w && h)) {

				if ((x+w > (*texPos)->GetWidth()) || (y+h > (*texPos)->GetHeight())) {
					return false;
				}

				clip.size.w = (*texPos)->GetWidth();
				clip.size.h = (*texPos)->GetHeight();
				clip.uv = {0.0f, 0.0f, 1.0f, 1.0f};
			}
			else {
				clip.size = {w, h};
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

			std::shared_ptr<DXTextureBase> emptyTex(std::make_shared<DXTextureBase>());
			if (!emptyTex->Create(d3ddev9, w, h))
				return false;

			TexClip clip;
			clip.size = {w, h};
			clip.uv = { 0.0f, 0.0f, 1.0f, 1.0f };
			
			tex.set(emptyTex, clip);
			
			return true;
		}

		bool DXTextureManager::CreateFromD3DTex9(Texture & tex, const DXTextureBase &texbase) {
			if (!(&texbase)) return false;

			D3DSURFACE_DESC desc;
			texbase.GetPointer()->GetLevelDesc(0, &desc);

			texture::TexClip clip;
			clip.size.w = desc.Width;
			clip.size.h = desc.Height;
			clip.uv		= {0.0f, 0.0f, 1.0f, 1.0f};

			tex.set(std::make_shared<DXTextureBase>(texbase), clip);

			return true;
		}



		bool DXTextureManager::DrawTexture(Texture &tex, float x, float y, DrawTexCoord coord, DWORD color, float xscale, float yscale, int rotDeg, bool isClip) {
			if (!tex) {
				return false;
			}
			if (!isDrawable()) {
				return false;
			}

			rotDeg%=360;
			float rotRad = (float)(rotDeg*M_PI/180);
			float colorRGBA[4] = {((color>>16)&0xff)/255.0f, ((color>>8)&0xff)/255.0f, ((color)&0xff)/255.0f, ((color>>24)&0xff)/255.0f};


			// 描画位置の算出
			PointF pos;
			Size size = tex.getSize();
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

			// 板ポリゴンを登録
			d3ddev9->SetStreamSource(0, vertex_rect, 0, sizeof(float)*5);

			// シェーダ開始
			UINT numPass = 0;
			effect->SetTechnique("Tech");
			effect->Begin(&numPass, 0);

			if (isClip)
				effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Mul_ColorAlpha_TexAlpha));
			else 
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

			texture::TexClip clipInfo = tex.clipInfo;

			D3DXMATRIX world, scale, rot;
			D3DXMatrixScaling(&world, (float)clipInfo.size.w, (float)clipInfo.size.h, 1.0f);	// ポリゴンサイズに
			D3DXMatrixScaling(&scale, xscale, yscale, 1.0f);	// ローカルスケール
			D3DXMatrixRotationZ(&rot, rotRad);						// 回転
			world._41 = -(clipInfo.size.w/2.0f);		// ピボット分オフセット
			world._42 = -(clipInfo.size.h/2.0f);
			world = world * scale * rot;
			world._41 += pos.x-0.5f + clipInfo.size.w/2.0f;	// ピボット分オフセット
			world._42 += pos.y+0.5f + clipInfo.size.h/2.0f;
			world._43 += topLayerPos/1000.0f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", &projMat);
			effect->SetTexture("tex", tex.tex->GetPointer());
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

		int DXTextureManager::CleanTexPool() {
			int releasedCnt = 0;
			for (size_t i=0; i<texPool.size(); i++) {
				if (texPool[i] && texPool[i].use_count() == 1) {
					texPool[i].reset();
					releasedCnt++;
				}
			}

			return releasedCnt;
		}



		tex_vector::iterator DXTextureManager::FindTexture(const std::wstring& fileName) {
			for (auto it=texPool.begin(); it!=texPool.end(); it++) {
				if (*it && (*it)->GetName() == fileName) {
					return it;
				}
			}

			return texPool.end();
		}

		tex_vector::iterator DXTextureManager::FindEmptyPool() {
			for (auto it=texPool.begin(); it!=texPool.end(); it++) {
				if (*it == nullptr) {
					return it;
				}
			}

			return texPool.end();
		}


	}

}