#include "DXDrawManager.hpp"

#define _USE_MATH_DEFINES
#include <math.h>  


#define Successful (1)
#define Failed (0)

#define WRITELOG(x) { if (log != nullptr) { log->tlnwrite(x); } }
#define SAFE_RELEASE(x) { if (x != nullptr) {x->Release(); } }

namespace drawMng {

	LogManager* DXDrawManager::log;
	std::vector<DrawFormat> DXDrawManager::drawQueue;
	std::shared_ptr<DXDrawManager> DXDrawManager::instance;


	DXDrawManager::DXDrawManager() {
		Delete();
	}

	DXDrawManager::~DXDrawManager() {
		Delete();
	}


	bool DXDrawManager::CreateFull(HWND hwnd) {
		instance.reset(new DXDrawManager());

		if (instance->Create(hwnd, true, 0, 0) == Failed) {
			instance.reset();
			return false;
		}

		return true;
	}


	bool DXDrawManager::CreateWind(HWND hwnd, size_t screenW, size_t screenH) {
		instance.reset(new DXDrawManager());

		if (instance->Create(hwnd, false, screenW, screenH) == Failed) {
			instance.reset();
			return false;
		}

		return true;
	}

	bool DXDrawManager::AddTexture(size_t texID, const std::string& fileName) {
		if (texID < 0 || texID >= TEXTURE_MAXCNT) {
			return false;
		}
		if (!d3ddev9) {
			return false;
		}

		// 既に生成された画像ファイルであるかチェック
		int resID = GetTexResourceID(fileName);

		if (resID == -1) {
			// テクスチャ生成
			if (texRes.size() >= TEXTURERES_MAXCNT) {
				return false;
			}

			// 追加
			texRes.push_back(std::make_unique<TextureFile>());
			if (!texRes.back()->Create(d3ddev9, fileName)) {
				return false;
			}
			texRes.back()->SetName(fileName);

			resID = texRes.size()-1;
		}

		// 描画情報を保存
		texClip[texID].isValid	= true;
		texClip[texID].resourceID = resID;
		texClip[texID].size.w	= texRes[resID]->GetWidth();
		texClip[texID].size.h	= texRes[resID]->GetHeight();
		texClip[texID].uv		= {0.0f, 0.0f, 1.0f, 1.0f};


		return true;
	}

	bool DXDrawManager::AddTexture(size_t texID, const std::string& fileName, size_t x, size_t y, size_t w, size_t h) {
		if (texID < 0 || texID >= TEXTURE_MAXCNT) {
			return false;
		}
		if (!d3ddev9) {
			return false;
		}

		// 既に生成された画像ファイルであるかチェック
		int resID = GetTexResourceID(fileName);

		if (resID == -1) {
			// テクスチャ生成
			if (texRes.size() >= TEXTURERES_MAXCNT) {
				return false;
			}

			// 追加
			texRes.push_back(std::make_unique<TextureFile>());
			if (!texRes.back()->Create(d3ddev9, fileName)) {
				return false;
			}
			texRes.back()->SetName(fileName);

			resID = texRes.size()-1;
		}

		// 描画情報をセット
		int texW = texRes[resID]->GetWidth();
		int texH = texRes[resID]->GetHeight();

		texClip[texID].isValid	= true;
		texClip[texID].resourceID = resID;
		texClip[texID].size.w	= w;
		texClip[texID].size.h	= h;
		texClip[texID].uv.left	= (float)x/texW;
		texClip[texID].uv.top	= (float)y/texH;
		texClip[texID].uv.w		= (float)w/texW;
		texClip[texID].uv.h		= (float)h/texH;

		return true;
	}


	std::unique_ptr<dxFont::DirectXFont> DXDrawManager::CreateDirectXFont(const wchar_t * fontName, size_t fontSize, dxFont::FontWeight fontWeight, bool isItalic, bool isUnderLine, bool isStrikeOut, AntialiasLevel level) {
		using namespace dxFont;

		std::unique_ptr<DirectXFont> uptr;
		uptr = DirectXFont::Create(shared_from_this(), fontName, fontSize, fontWeight, isItalic, isUnderLine, isStrikeOut, level);

		return std::move(uptr);
	}

	
	bool DXDrawManager::DelTexture(size_t texID) {
		if (texID < 0 || texID >= TEXTURE_MAXCNT) {
			return false;
		}

		ZeroMemory(&texClip[texID], sizeof(DrawFormat));

		return true;
	}


	bool DXDrawManager::DrawBegin() {
		if (!d3ddev9) {
			return false;
		}
		if (isLost) {
			return false;
		}
		if (!isDrawStarted) {
			if (FAILED(d3ddev9->BeginScene())) {
				WRITELOG("failed to BeginScene()");
				return false;
			}
			isDrawStarted = true;
		}

		return true;
	}

	bool DXDrawManager::DrawEnd() {
		if (!isDrawStarted) {
			return false;
		}
		if (!d3ddev9) {
			return false;
		}

		// 描画終了
		d3ddev9->EndScene();
		isDrawStarted = false;

		// フリップ
		if (FAILED(d3ddev9->Present(NULL, NULL, NULL, NULL))) {
			WRITELOG("device lost");
			isLost = true;
			return false;
		}


		return true;
	}

	bool DXDrawManager::DrawTexture(size_t texID, float x, float y, DRAWTEX_COORD coord, float alpha) {
		return DrawTexture(texID, x, y, coord, 1.0f, 1.0f, alpha, 0);
	}

	bool DXDrawManager::DrawTexture(size_t texID, float x, float y, DRAWTEX_COORD coord, float xscale, float yscale, float alpha, int rotDeg) {
		if (texID < 0 || texID >= TEXTURE_MAXCNT) {
			return false;
		}
		if (!texClip[texID].isValid) {
			return false;
		}

		rotDeg%=360;

		DrawFormat fmt;

		fmt.origin.x = texClip[texID].size.w/2.0f;
		fmt.origin.y  = texClip[texID].size.h/2.0f;

		fmt.tex = texRes.at(texClip[texID].resourceID)->GetPointer();
		fmt.size = texClip[texID].size;
		fmt.uv = texClip[texID].uv;

		fmt.rotRad	= (float)(rotDeg*M_PI/180.0f);
		fmt.xscale	= xscale;
		fmt.yscale	= yscale;
		fmt.color	= {0.0f, 0.0f, 0.0f, alpha};
		fmt.blendMode=blendMode;


		// 描画位置の登録
		switch (coord) {
			case DRAWTEX_COORD::TOP_L:
				fmt.pos.x	= x + texClip[texID].size.w*(xscale-1.0f)/2.0f;
				fmt.pos.y	= y + texClip[texID].size.h*(yscale-1.0f)/2.0f;
				break;
			case DRAWTEX_COORD::TOP_R:
				fmt.pos.x	= x - texClip[texID].size.w*(xscale+1.0f)/2.0f;
				fmt.pos.y	= y + texClip[texID].size.h*(yscale-1.0f)/2.0f;
				break;
			case DRAWTEX_COORD::BOTTOM_R:
				fmt.pos.x	= x - texClip[texID].size.w*(xscale+1.0f)/2.0f;
				fmt.pos.y	= y - texClip[texID].size.h*(yscale+1.0f)/2.0f;
				break;
			case DRAWTEX_COORD::BOTTOM_L:
				fmt.pos.x	= x + texClip[texID].size.w*(xscale-1.0f)/2.0f;
				fmt.pos.y	= y - texClip[texID].size.h*(yscale+1.0f)/2.0f;
				break;
			case DRAWTEX_COORD::CENTER:
				fmt.pos.x	= x - texClip[texID].size.w/2.0f;
				fmt.pos.y	= y - texClip[texID].size.h/2.0f;
				break;
			default:
				break;
		}

		drawQueue.push_back(fmt);

		return true;
	}

	bool DXDrawManager::DrawAll() {
		if (!d3ddev9) {
			return false;
		}
		if (!vertex) {
			return false;
		}
		if (!effect) {
			return false;
		}
		if (!verDecl) {
			return false;
		}
		if (isLost) {
			return false;
		}
		if (!isDrawStarted) {
			return false;
		}


		// デフォルトステートのセット
		d3ddev9->SetRenderState(D3DRS_LIGHTING, FALSE);							// ライティング無効
		if (isRightHand)
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);					// 右回りを消去(右手系)
		else
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);					// 左回りを消去(左手系)

		d3ddev9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3ddev9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		d3ddev9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		d3ddev9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		d3ddev9->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		d3ddev9->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		d3ddev9->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		d3ddev9->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		// 板ポリゴンを登録
		d3ddev9->SetStreamSource(0, vertex, 0, sizeof(float)*5);
		// 頂点宣言を登録
		d3ddev9->SetVertexDeclaration(verDecl);

		// 2D描画用射影変換行列
		D3DXMATRIX proj;
		D3DXMatrixIdentity(&proj);
		proj._41 = -1.0f;
		proj._42 =  1.0f;

		// シェーダ開始
		UINT numPass = 0;
		effect->SetTechnique("Tech");
		effect->Begin(&numPass, 0);
		effect->BeginPass(0);

		// 描画リストのテクスチャを一気に描画する
		float colorRGBA[4];
		for (DrawFormat fmt : drawQueue) {

			// ブレンドモードを設定
			switch (fmt.blendMode) {
				case BLENDMODE::NORMAL:
					d3ddev9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					d3ddev9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
					break;
				case BLENDMODE::ADD:
					d3ddev9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					d3ddev9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
					break;
			}



			colorRGBA[0] = fmt.color.r;
			colorRGBA[1] = fmt.color.g;
			colorRGBA[2] = fmt.color.b;
			colorRGBA[3] = fmt.color.a;


			proj._11 =  2.0f / d3dpresent.BackBufferWidth;
			proj._22 = -2.0f / d3dpresent.BackBufferHeight;


			D3DXMATRIX world, scale, rot;
			D3DXMatrixScaling(&world, (float)fmt.size.w, (float)fmt.size.h, 1.0f);	// ポリゴンサイズに
			D3DXMatrixScaling(&scale, fmt.xscale, fmt.yscale, 1.0f);	// ローカルスケール
			D3DXMatrixRotationZ(&rot, fmt.rotRad);						// 回転
			world._41 = -fmt.origin.x;		// ピボット分オフセット
			world._42 = -fmt.origin.y;
			world = world * scale * rot;
			world._41 += fmt.pos.x-0.5f + fmt.origin.x;	// ピボット分オフセット
			world._42 += fmt.pos.y+0.5f + fmt.origin.y;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", &proj);
			effect->SetTexture("tex", fmt.tex);
			effect->SetFloat("uv_left", fmt.uv.left);
			effect->SetFloat("uv_top", fmt.uv.top);
			effect->SetFloat("uv_width", fmt.uv.w);
			effect->SetFloat("uv_height", fmt.uv.h);
			effect->SetFloatArray("color", colorRGBA, 4);
			effect->CommitChanges();
			d3ddev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		}

		effect->EndPass();
		effect->End();


		drawQueue.clear();

		return true;
	}



	bool DXDrawManager::ClearBackGround() {
		if (!d3ddev9) {
			return false;
		}
		if (isLost) {
			return false;
		}

		if (!isDrawStarted) {
			d3ddev9->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, backGroundColor, 1.0f, 0);
		}

		return true;
	}


	void DXDrawManager::SetHand(bool isRight) {
		isRightHand = isRight;
	}

	void DXDrawManager::SetBackGroundColor(size_t r, size_t g, size_t b) {
		// 最大値を255までにする
		r &= 0xff;
		g &= 0xff;
		b &= 0xff;
		SetBackGroundColor((unsigned long)(r|g|b));
	}


	void DXDrawManager::SetBackGroundColor(unsigned long rgb) {
		rgb &= 0xffffff;
		backGroundColor = rgb;
	}

	void DXDrawManager::SetTextureFilter(D3DTEXTUREFILTERTYPE type) {
		d3ddev9->SetSamplerState(0, D3DSAMP_MAGFILTER, type);
		d3ddev9->SetSamplerState(0, D3DSAMP_MINFILTER, type);
	}

	void DXDrawManager::SetBlendMode(BLENDMODE mode) {
		switch (mode) {
			case BLENDMODE::NORMAL:
				blendMode = mode;
				break;
			case BLENDMODE::ADD:
				blendMode = mode;
				break;
			default:
				break;
		}

	}



	void DXDrawManager::SetLogWriteDest(LogManager* dest) {
		if (dest == nullptr) {
			return;
		}

		log = dest;
	}


	

	bool DXDrawManager::Create(HWND hwnd, bool isfull, size_t w, size_t h) {
		Delete();
		Clear();

		HRESULT ret;


		// Direct3D9オブジェクトの取得
		d3d9.Attach(Direct3DCreate9(D3D_SDK_VERSION));
		if (d3d9 == nullptr) {
			WRITELOG("Failed to Create D3D9 Object");
			return false;
		}


		// ハードウェアの能力の取得
		ret = d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps9);
		if (FAILED(ret)) {
			WRITELOG("Failed to Get D3DCAPS9");
			return false;
		}


		// Direct3D9デバイス取得のための引数を設定
		d3dpresent.Windowed						= !isfull;
		d3dpresent.BackBufferWidth				= (UINT)w;
		d3dpresent.BackBufferHeight				= (UINT)h;
		if (isfull) {
			d3dpresent.BackBufferFormat			= D3DFMT_X8R8G8B8;
		}
		else {
			D3DDISPLAYMODE dispMode;
			ret = d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);
			if (FAILED(ret)) {
				WRITELOG("Failed to Get D3DDISPLAYMODE");
				return false;
			}
			d3dpresent.BackBufferFormat			= dispMode.Format;
		}
		d3dpresent.BackBufferCount				= 1;
		d3dpresent.MultiSampleType				= D3DMULTISAMPLE_NONE;
		d3dpresent.MultiSampleQuality			= 0;
		d3dpresent.SwapEffect					= D3DSWAPEFFECT_DISCARD;
		d3dpresent.hDeviceWindow				= NULL;	// あとで指定
		d3dpresent.EnableAutoDepthStencil		= true;
		d3dpresent.AutoDepthStencilFormat		= D3DFMT_D24S8;
		d3dpresent.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
		d3dpresent.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT;



		/*
		Direct3D9デバイスの取得開始
		速度の早い順番でテスト

		1.ハードウェア処理&HW頂点処理
		2.ハードウェア処理&SW頂点処理
		3.ソフトウェア処理&HW頂点処理
		4.ソフトウェア処理&SW頂点処理
		*/
		for (int i=0; i<4; i++) {
			D3DDEVTYPE devtype;
			DWORD behaviorFlags;

			switch (i) {
				case 0:
					devtype = D3DDEVTYPE_HAL;
					behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
					break;
				case 1:
					devtype = D3DDEVTYPE_HAL;
					behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					break;
				case 2:
					devtype = D3DDEVTYPE_REF;
					behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
					break;
				case 3:
					devtype = D3DDEVTYPE_REF;
					behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					break;
			}

			// マルチスレッド
			behaviorFlags |= D3DCREATE_MULTITHREADED;

			IDirect3DDevice9 *ptr;
			ret = d3d9->CreateDevice(D3DADAPTER_DEFAULT, devtype, hwnd, behaviorFlags, &d3dpresent, &ptr);
			if (SUCCEEDED(ret)) {
				d3ddev9.Attach(ptr);

				switch (i) {
					case 0: WRITELOG("Succeeded to Create D3DDevice9 (DeviceType:GPU VertexProcessing:Hardware)"); break;
					case 1: WRITELOG("Succeeded to Create D3DDevice9 (DeviceType:GPU VertexProcessing:Software)"); break;
					case 2: WRITELOG("Succeeded to Create D3DDevice9 (DeviceType:CPU VertexProcessing:Hardware)"); break;
					case 3: WRITELOG("Succeeded to Create D3DDevice9 (DeviceType:CPU VertexProcessing:Software)"); break;
				}

				break;	// 作成終了
			}

			if (i==3) {	// 最終ループなら
				WRITELOG("Failed to Create D3DDevice9");
				return false;
			}
		}


		// 板ポリゴン作成
		float vtx[] = {
			0.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // 0
			1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // 1
			0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // 2
			1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // 3
		};

		{
			IDirect3DVertexBuffer9 *ptr;
			if (FAILED(d3ddev9->CreateVertexBuffer(sizeof(vtx), 0, 0, D3DPOOL_MANAGED, &ptr, 0))) {
				WRITELOG("failed to create \"Vertex Buffer9\"");
				return false;
			}
			float *p = nullptr;
			ptr->Lock(0, 0, (void**)&p, 0);
			memcpy(p, vtx, sizeof(vtx));
			ptr->Unlock();

			vertex.Attach(ptr);
		}


		// シェーダ作成
		{
			ID3DXBuffer *error = nullptr;
			ID3DXEffect *ptr = nullptr;
			if (FAILED(D3DXCreateEffectFromFile(d3ddev9, L"sprite2.fx", 0, 0, 0, 0, &ptr, &error))) {
				OutputDebugStringA((const char*)error->GetBufferPointer());
				return false;
			}

			effect.Attach(ptr);
		}

		// 頂点宣言作成
		{
			IDirect3DVertexDeclaration9 *ptr;
			D3DVERTEXELEMENT9 elems[] = {
				{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
				{0, sizeof(float) * 3, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
				D3DDECL_END()
			};

			if (FAILED(d3ddev9->CreateVertexDeclaration(elems, &ptr))) {
				WRITELOG("failed to create \"Bertex Declaration9\"");
				return false;
			}
			verDecl.Attach(ptr);
		}


		// デフォルトステートのセット
		d3ddev9->SetRenderState(D3DRS_LIGHTING, FALSE);							// ライティング無効
		if (isRightHand)
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);					// 右回りを消去(右手系)
		else
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);					// 左回りを消去(左手系)

		d3ddev9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3ddev9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		d3ddev9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);			// テクスチャがはみ出た時に表示しないにする
		d3ddev9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		d3ddev9->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		d3ddev9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		d3ddev9->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		d3ddev9->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		d3ddev9->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		d3ddev9->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);


		return true;
	}

	void DXDrawManager::Delete() {
		// 生成した順番と逆順に解放

		//SAFE_RELEASE(verDecl);
		////verDecl.reset();
		//SAFE_RELEASE(effect);
		////effect.reset();
		//SAFE_RELEASE(vertex);
		////vertex.reset();
		//SAFE_RELEASE(d3ddev9);
		////d3ddev9.reset();
		//SAFE_RELEASE(d3d9);
		////d3d9.reset();
	}

	void DXDrawManager::Clear() {
		isLost = false;
		isDrawStarted = false;
		isRightHand = false;
		backGroundColor = 0xffffff;
		blendMode = BLENDMODE::NORMAL;

		texRes.clear();
		drawQueue.clear();
		ZeroMemory(texClip, sizeof(texClip));
		ZeroMemory(&d3dcaps9, sizeof(d3dcaps9));
		ZeroMemory(&d3dpresent, sizeof(d3dpresent));

	}


	int DXDrawManager::GetTexResourceID(const std::string& fileName) {
		for (size_t i=0; i<texRes.size(); i++) {
			if (texRes[i]->GetName() == fileName) {
				return i;
			}
		}

		return -1;
	}


}

