#define _USE_MATH_DEFINES
#include <math.h>  
#include <string>

#include "DirectXFont.hpp"


namespace dx9 {



	bool DirectXFont::Create(const std::wstring &fontName, size_t fontSize, FontWeight fontWeight, bool isItalic, bool isUnderLine, bool isStrikeOut, AntialiasLevel level) {
		DeleteAll();

		// フォントハンドルの生成
		LOGFONTW	lf;
		lf.lfHeight				= (LONG)fontSize;						// 文字の高さ
		lf.lfWidth				= 0;							// 文字幅
		lf.lfEscapement			= 0;							// 文字方向とX軸との角度
		lf.lfOrientation		= 0;							// 各文字とX軸との角度
		lf.lfWeight				= (LONG)fontWeight;					// 太さ
		lf.lfItalic				= isItalic;						// イタリック体
		lf.lfUnderline			= isUnderLine;					// 下線
		lf.lfStrikeOut			= isStrikeOut;					// 打ち消し線
		lf.lfCharSet			= DEFAULT_CHARSET;				// キャラクタセット
		lf.lfOutPrecision		= OUT_DEFAULT_PRECIS;			// 出力精度
		lf.lfClipPrecision		= CLIP_DEFAULT_PRECIS;			// クリッピングの精度
		lf.lfQuality			= PROOF_QUALITY;				// 出力品質
		lf.lfPitchAndFamily		= DEFAULT_PITCH | FF_MODERN;	// ピッチとファミリ
		wcscpy_s(lf.lfFaceName, fontName.c_str());

		hFont = CreateFontIndirectW(&lf);
		if (hFont == NULL) {
			return false;
		}

	
		this->fontSize = fontSize;
		antialiasLv = level;

		

		// デバイスにフォントを設定
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		// フォント情報を取得
		GetTextMetricsW(hdc, &tm);

		// 定義文字数分の要素を確保
		const UINT defineCharCnt = static_cast<UINT>(tm.tmLastChar - tm.tmFirstChar);
		texRes.resize(defineCharCnt);

		// 元のフォントに戻す
		SelectObject(hdc, oldFont);
		ReleaseDC(NULL, hdc);

		return true;
	}

	bool DirectXFont::Draw(float x, float y, const wchar_t* s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, false, 0, -1, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}


	bool DirectXFont::DrawInRect(RectF& rect, const wchar_t * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, true, 0, -1, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFont::Draw(float x, float y, size_t startCharCnt, int drawCharCnt, const wchar_t * s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, false, startCharCnt, drawCharCnt, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFont::DrawInRect(RectF & rect, size_t startCharCnt, int drawCharCnt, const wchar_t * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, true, startCharCnt, drawCharCnt, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFont::DrawInRect(RectF & rect, size_t startCharCnt, int drawCharCnt, size_t fontSize, const wchar_t * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, true, startCharCnt, drawCharCnt, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}



	bool DirectXFont::DrawFont(RectF & rect, bool isAlign, size_t startCharCnt, int drawCharCnt, size_t fontSize, const wchar_t * s, va_list vlist) {
		if (!isDrawable()) {
			return false;
		}
		if (!hFont) {
			return false;
		}

		if (isAlign) {
			if (rect.left > rect.right || rect.top > rect.bottom) 
				return false;
		}

		using namespace std;


		int len = _vsnwprintf_s(workBuf.data(), CHARACTER_MAXCNT+1, CHARACTER_MAXCNT, s, vlist);
		


		if (strlen == 0) {
			return true;
		}
	

		// テクスチャの作成
		StoreFontTex(workBuf.data());


		// 描画位置の算出 
		vector<pair<float, size_t>> headPos; // first:行頭のx座標 second:行頭文字が何番目か
		RectF strArea = {0.0f, 0.0f, 0.0f, 0.0f};
		size_t totalOffset = 0;
		int width = (int)(rect.right - rect.left);
		if (!isAlign) width = -1;
		int height = (int)(rect.bottom - rect.top);
		int lineCnt;

		for (lineCnt=0; true; lineCnt++) {

			if (isAlign && fontSize*(lineCnt+1) > height)
				break;

			int len;
			int offset = GetStrLength(workBuf.data(), totalOffset, letterSpace, fontSize, (float)width, len);
			if (offset <= 0) {
				break;
			}

			float x;
			if (isAlign) {
				switch (textAlign) {
					case dx9::TextAlign::LEFT:
					case dx9::TextAlign::NONE:
						x = rect.left;
						break;
					case dx9::TextAlign::CENTERX:
						x = rect.left + (width - len)/2.0f;
						break;
					case dx9::TextAlign::CENTERXY:
						x = rect.left + (width - len)/2.0f;
						break;
					case dx9::TextAlign::RIGHT:
						x = rect.left + (width - len);
						break;
				}
			}
			else {
				x = rect.left;
			}
			
			headPos.push_back({x, totalOffset});

			if (lineCnt == 0 || x < strArea.left)
				strArea.left = x;
			if (lineCnt == 0 || x + len > strArea.right)
				strArea.right = x + len;


			totalOffset += offset;
		}
		if (totalOffset == 0) return true;


		
		if (textAlign == TextAlign::CENTERXY)
			strArea.top = rect.top + (height - lineCnt*(int)fontSize)/2.0f;
		else
			strArea.top = rect.top;

		strArea.bottom = strArea.top + lineCnt*fontSize;



		// 引数を最適化
		size_t endCharCnt = 0;
		if (drawCharCnt < 0 || startCharCnt + drawCharCnt > totalOffset) {
			endCharCnt = totalOffset;
		}
		else {
			endCharCnt = startCharCnt + drawCharCnt;
		}


		// 文字列回転の原点を設定 
		PointF rotOriginPt;
		switch (fontRotOrigin) {
			case FontRotOrigin::TOP_L:
				rotOriginPt.x = strArea.left;
				rotOriginPt.y = strArea.top;
				break;
			case FontRotOrigin::TOP_R:
				rotOriginPt.x = strArea.right;
				rotOriginPt.y = strArea.top;
				break;
			case FontRotOrigin::BOTTOM_L:
				rotOriginPt.x = strArea.left;
				rotOriginPt.y = strArea.bottom;
				break;
			case FontRotOrigin::BOTTOM_R:
				rotOriginPt.x = strArea.right;
				rotOriginPt.y = strArea.bottom;
				break;
			case FontRotOrigin::CENTER:
				rotOriginPt.x = (strArea.left + strArea.right)/2.0f;
				rotOriginPt.y = (strArea.top + strArea.bottom)/2.0f;
				break;
		}



		// デフォルトステートのセット
		if (isRightHand)
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);				// 右回りを消去(右手系)
		else
			d3ddev9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 左回りを消去(左手系)

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

		D3DXMATRIX world, rot;
		PointF pos = {0, 0};
		PointF finalPos = {0, 0};
		auto nowHead = headPos.cbegin();
		auto ch = workBuf.cbegin();
		float fontscale = (float)fontSize/this->fontSize;


		// シェーダ開始
		UINT numPass = 0;
		effect->SetTechnique("Tech");
		effect->Begin(&numPass, 0);
		effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Mul_ColorAlpha_UVTexAlpha));
		effect->SetFloat("uv_left", 0.0f);
		effect->SetFloat("uv_top", 0.0f);
		effect->SetFloat("uv_width", 1.0f);
		effect->SetFloat("uv_height", 1.0f);

		effect->SetMatrix("proj", &projMat);
		effect->SetFloatArray("color", fontColor.data(), 4);

		for (size_t i=0; i<endCharCnt; i++) {
			UINT code = static_cast<UINT>(*ch);
			ch++;

			if (!texRes[code]) {
				continue;
			}
			if (nowHead != headPos.cend() && i == nowHead->second) {
				pos.x = nowHead->first;
				pos.y = strArea.top + fontSize*distance(headPos.cbegin(), nowHead);
				nowHead++;
			}

			if (i < startCharCnt)
				continue;

			finalPos.x = pos.x + texRes[code]->_chInfo().originX*fontscale;
			finalPos.y = pos.y - texRes[code]->_chInfo().originY*fontscale;

			PointF origin = {
				rotOriginPt.x - finalPos.x,
				rotOriginPt.y - finalPos.y
			};

			D3DXMatrixScaling(&world, (float)texRes[code]->GetWidth()*fontscale, (float)texRes[code]->GetHeight()*fontscale, 1.0f);	// ポリゴンサイズに
			D3DXMatrixRotationZ(&rot, charTravelAngle_rad);						// 回転
			world._41 = -origin.x;		// ピボット分オフセット
			world._42 = -origin.y;
			world = world * rot;
			world._41 += finalPos.x + origin.x;	// ピボット分オフセット
			world._42 += finalPos.y + origin.y;
			world._43 += topLayerPos/1000.0f;

			// ラスタライゼーションルールを用いて，テクスチャをずらす
			world._41 = ceil(world._41) - 0.5f;
			world._42 = floor(world._42) + 0.5f;


			effect->SetMatrix("world", &world);
			effect->SetTexture("tex", texRes[code]->GetPointer());
			effect->CommitChanges();
			d3ddev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			pos.x += texRes[code]->_chInfo().AreaW*fontscale + letterSpace;
		}

		effect->EndPass();
		effect->End();

		return true;
	}


	


	DirectXFont::DirectXFont() {
		hFont = nullptr;
	}

	DirectXFont::~DirectXFont() {

	}


	bool DirectXFont::Delete(unsigned int code) {
		if (code < 0 || code >= texRes.size()) {
			return false;
		}

		texRes[code].reset();

		return true;
	}

	bool DirectXFont::DeleteAll() {
		fontSize = 0;
		antialiasLv = AntialiasLevel::_15STEPS;
		fontColor[0] = 0.0f;
		fontColor[1] = 0.0f;
		fontColor[2] = 0.0f;
		fontColor[3] = 1.0f;

		letterSpace = 0;

		charTravelAngle_rad = 0.0f;

		fontRotOrigin = FontRotOrigin::TOP_L;
		textAlign = TextAlign::LEFT;
		
		if (hFont != nullptr) {
			DeleteObject(hFont);
		}

		texRes.clear();

		return true;
	}


	
	int DirectXFont::GetStrLength(const wchar_t* str, size_t offset, int letterSpace, size_t fontSize, float limit, int & length) {
		size_t strLength = wcslen(str);

		if (offset >= strLength) {
			return -1;
		}
	
		using namespace std;


		bool isLimitOn = (limit >= 0.0f);
		int charCnt = 0;

		float scale = (float)fontSize/this->fontSize;
		
		float totalLen = 0;
		for (size_t i=offset; i<strLength; i++) {
			
			unsigned code = (unsigned)(str[i]);

			if (code == L'\n') {
				charCnt++;
				break;
			}

			switch (code) {
				case L'\t':
				case L'\a':
				case L'\b':
				case L'\f':
				case L'\r':
				case L'\v':
					charCnt++;
					continue;
			}
		
			if (isLimitOn && totalLen + texRes[code]->_chInfo().sizeW*scale > limit)
				break;

			totalLen += texRes[code]->_chInfo().AreaW*scale + letterSpace;
			charCnt++;

		}

		length = (int)totalLen;
		
		return charCnt;
	}


	bool DirectXFont::StoreFontTex(const wchar_t* wstr) {
		using namespace std;

		// デバイスにフォントを設定
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		// 文字テクスチャ作成
		size_t len = wcslen(wstr);
		for (int i=0; i<len; i++) {

			UINT code = static_cast<UINT>(wstr[i]);

			switch (code) {
				case L'\t':
				case L'\a':
				case L'\b':
				case L'\f':
				case L'\r':
				case L'\v':
				case L'\n':
					continue;

			}

			if (!texRes[code]) {
				texRes[code] = make_unique<texture::FontTextureW>();
				if (!texRes[code]->Create(d3ddev9, hdc, wstr[i], antialiasLv, &tm)) {
					texRes[code].reset();
					return false;
				}
			}
		}

		// 元のフォントに戻す
		SelectObject(hdc, oldFont);
		ReleaseDC(NULL, hdc);

		return true;
	}

	void DirectXFont::SetFontColor(Color &color) {
		fontColor = color.getRGBAFloat();
	}

	void DirectXFont::SetCharTravelDirection(int deg) {
		charTravelAngle_rad = deg*(float)M_PI/180.0f;
	}

	void DirectXFont::SetCharTravelDirection(float rad) {
		charTravelAngle_rad = rad;
	}




}