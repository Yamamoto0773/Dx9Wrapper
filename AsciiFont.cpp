#define _USE_MATH_DEFINES
#include <math.h>  
#include "AsciiFont.hpp"

#include <d3dx9.h>


namespace dx9 {



	AsciiFont::AsciiFont() {
		hFont = nullptr;
		fontColor = {0.0f, 0.0f, 0.0f, 1.0f};
	}

	AsciiFont::AsciiFont(const wchar_t * fontName, size_t fontSize, FontWeight fontWeight, bool isItalic, bool isUnderLine, bool isStrikeOut, AntialiasLevel level) {
		Create(fontName, fontSize, fontWeight, isItalic, isUnderLine, isStrikeOut, level);
	}


	AsciiFont::~AsciiFont() {
	}


	bool AsciiFont::Create(const wchar_t * fontName, size_t _fontSize, FontWeight fontWeight, bool isItalic, bool isUnderLine, bool isStrikeOut, AntialiasLevel level) {
		if (!isResCreated) return false;

		DeleteAll();

		// フォントハンドルの生成
		LOGFONTW lf;
		lf.lfHeight				= (LONG)_fontSize;						// 文字の高さ
		lf.lfWidth				= 0;							// 文字幅
		lf.lfEscapement			= 0;							// 文字方向とX軸との角度
		lf.lfOrientation		= 0;							// 各文字とX軸との角度
		lf.lfWeight				= (LONG)fontWeight;					// 太さ
		lf.lfItalic				= isItalic;						// イタリック体
		lf.lfUnderline			= isUnderLine;					// 下線
		lf.lfStrikeOut			= isStrikeOut;					// 打ち消し線
		lf.lfCharSet			= ANSI_CHARSET;					// キャラクタセット
		lf.lfOutPrecision		= OUT_DEFAULT_PRECIS;			// 出力精度
		lf.lfClipPrecision		= CLIP_DEFAULT_PRECIS;			// クリッピングの精度
		lf.lfQuality			= PROOF_QUALITY;				// 出力品質
		lf.lfPitchAndFamily		= DEFAULT_PITCH | FF_MODERN;	// ピッチとファミリ
		wcscpy_s(lf.lfFaceName, 32, fontName);

		hFont = CreateFontIndirectW(&lf);
		if (hFont == nullptr) {
			return false;
		}


		this->fontSize = _fontSize;
		antialiasLv = level;


		// デバイスにフォントを設定
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		// フォント情報を取得
		GetTextMetricsW(hdc, &tm);

		// ASCII文字のテクスチャを作成
		texRes.resize(L'~'-L' '+1);

		for (size_t i=0; i<texRes.size(); i++) {
			if (!texRes[i]) {
				texRes[i] = std::make_unique<texture::FontTexture>();
				if (!texRes[i]->Create(d3ddev9, hdc, static_cast<wchar_t>(L' ' + i), antialiasLv, &tm)) {
					texRes[i].reset();
					return false;
				}
			}
		}


		// 元のフォントに戻す
		SelectObject(hdc, oldFont);
		ReleaseDC(NULL, hdc);

		return true;
	}

	bool AsciiFont::Draw(float x, float y, const wchar_t* s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, false, 0, -1, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}


	bool AsciiFont::DrawInRect(const RectF& rect, const wchar_t * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, true, 0, -1, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}

	bool AsciiFont::Draw(float x, float y, size_t startCharCnt, int drawCharCnt, const wchar_t * s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, false, startCharCnt, drawCharCnt, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}

	bool AsciiFont::DrawInRect(const RectF & rect, size_t startCharCnt, int drawCharCnt, const wchar_t * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, true, startCharCnt, drawCharCnt, fontSize, s, vlist);
		va_end(vlist);

		return result;
	}


	bool AsciiFont::DrawInRect(const RectF & rect, size_t startCharCnt, int drawCharCnt, size_t _fontSize, const wchar_t * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, true, startCharCnt, drawCharCnt, _fontSize, s, vlist);
		va_end(vlist);

		return result;
	}


	bool AsciiFont::DrawFont(const RectF & rect, bool isAlign, size_t startCharCnt, int drawCharCnt, size_t _fontSize, const wchar_t * s, va_list vlist) {
		if (!isDrawable()) {
			return false;
		}
		if (!hFont) {
			return false;
		}
		if (!s) {
			return false;
		}

		if (isAlign) {
			if (rect.left > rect.right || rect.top > rect.bottom)
				return false;
		}

		using namespace std;

		// 書式文字列をwchar_t型の文字列に変換
		int numofChars = _vsnwprintf_s(&workBuf[0], CHARACTER_MAXCNT+1, CHARACTER_MAXCNT, s, vlist);

		if (numofChars == 0) {
			return true;
		}


		// 描画位置の算出 
		vector<pair<float, size_t>> headPos; // first:行頭のx座標 second:行頭文字が何番目か
		RectF strArea = { rect.right, rect.bottom, rect.left, rect.top };
		size_t totalOffset = 0;
		int width = (int)(rect.right - rect.left);
		int height = (int)(rect.bottom - rect.top);
		PointF adjustScale = {1.0f, 1.0f};


		if (strAdjust == TextureAdjust::NONE || !isAlign) {

			if (!isAlign) {
				width = -1;
			}

			int lineCnt;
			for (lineCnt = 0; true; lineCnt++) {

				if (isAlign && (int)_fontSize*(lineCnt+1) > height)
					break;


				// calc length of string
				int len;
				int offset = GetStrLength(workBuf.data(), totalOffset, letterSpace, _fontSize, (float)width, len);
				if (offset <= 0) {
					break;
				}

				// calc position of line head
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
				} else {
					x = rect.left;
				}

				headPos.push_back({ x, totalOffset });

				if (lineCnt == 0 || x < strArea.left)
					strArea.left = x;
				if (lineCnt == 0 || x + len > strArea.right)
					strArea.right = x + len;


				totalOffset += offset;

			}


			if (isAlign && textAlign == TextAlign::CENTERXY)
				strArea.top = rect.top + (height - lineCnt*(int)_fontSize)/2.0f;
			else
				strArea.top = rect.top;

			strArea.bottom = strArea.top + lineCnt*_fontSize;



		}
		else {


			// calc length of string
			int len;
			int offset = GetStrLength(workBuf.data(), totalOffset, letterSpace, _fontSize, -1.0f, len);
			if (offset <= 0) {
				return false;
			}

			// calc scale of string adjust
			float scale_x = 1.0f, scale_y = 1.0f;
			scale_x = (rect.right - rect.left)/len;
			scale_y = (float)height/_fontSize;


			switch (strAdjust) {
			case dx9::TextureAdjust::ASPECT_UNFIXED:
			case dx9::TextureAdjust::ASPECT_UNFIXED_REDUCEONLY:
				adjustScale = {scale_x, scale_y};
				break;
			case dx9::TextureAdjust::ASPECT_FIXED:
			case dx9::TextureAdjust::ASPECT_FIXED_REDUCEONLY:
				if (scale_x < scale_y) {
					adjustScale = { scale_x, scale_x };
				}
				else {
					adjustScale = { scale_y, scale_y };
				}
				break;
			}


			if (strAdjust == TextureAdjust::ASPECT_FIXED_REDUCEONLY ||
				strAdjust == TextureAdjust::ASPECT_UNFIXED_REDUCEONLY) {

				if (adjustScale.x > 1.0f)
					adjustScale.x = 1.0f;

				if (adjustScale.y > 1.0f)
					adjustScale.y = 1.0f;

			}


			len = (int)(len*adjustScale.x);


			// calc position of line head
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
			} else {
				x = rect.left;
			}

			headPos.push_back({ x, 0 });


			// set string area
			strArea.left = x;
			strArea.right = x+len;
			strArea.top = rect.top + (height - (int)_fontSize*adjustScale.y)/2.0f;
			strArea.bottom = strArea.top + (int)_fontSize*adjustScale.y;

			totalOffset += offset;

		}

			
		if (totalOffset == 0) return true;


		// set range of string which shold be drawn
		size_t endCharCnt = 0;
		if (drawCharCnt < 0 || startCharCnt + drawCharCnt > totalOffset) {
			endCharCnt = totalOffset;
		}
		else {
			endCharCnt = startCharCnt + drawCharCnt;
		}


		// 文字列回転の原点を設定 
		PointF rotOriginPt = { 0.0f, 0.0f };
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
		PointF finalPos = {0, 0};	// 最終的な描画位置
		auto nowHead = headPos.cbegin();
		auto ch = workBuf.cbegin();
		float fontscale = (float)_fontSize/this->fontSize;

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

		for (size_t i=startCharCnt; i<endCharCnt; i++) {
			UINT subscr = *ch - L' ';
			ch++;

			if (subscr<0 || subscr>=texRes.size() || !texRes[subscr]) {
				continue;
			}
			if (nowHead != headPos.cend() && i == nowHead->second) {
				pos.x = nowHead->first;
				pos.y = strArea.top + _fontSize*distance(headPos.cbegin(), nowHead);
				nowHead++;
			}

			if (i < startCharCnt)
				continue;

			finalPos.x = pos.x + texRes[subscr]->_chInfo().originX*fontscale*adjustScale.x;
			finalPos.y = pos.y - texRes[subscr]->_chInfo().originY*fontscale*adjustScale.y;

			PointF origin = {
				rotOriginPt.x - finalPos.x,
				rotOriginPt.y - finalPos.y
			};


			D3DXMatrixScaling(
				&world,
				(float)texRes[subscr]->GetWidth()*fontscale*adjustScale.x, 
				(float)texRes[subscr]->GetHeight()*fontscale*adjustScale.y, 
				1.0f
			);	// ポリゴンサイズに
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
			effect->SetTexture("tex", texRes[subscr]->GetPointer());
			effect->CommitChanges();
			d3ddev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


			pos.x += texRes[subscr]->_chInfo().AreaW*fontscale*adjustScale.x + letterSpace;
		}

		effect->EndPass();
		effect->End();

		return true;
	}


	bool AsciiFont::DeleteAll() {
		Clear();

		if (hFont != nullptr) {
			DeleteObject(hFont);
		}

		texRes.clear();

		return true;
	}

	int AsciiFont::GetStrLength(const wchar_t* str, size_t offset, int _letterSpace, size_t _fontSize, float limit, int & length) {
		size_t strLength = wcslen(str);

		if (offset >= strLength) {
			return -1;
		}

		using namespace std;


		bool isLimitOn = (limit >= 0.0f);
		int charCnt = 0;

		float scale = (float)_fontSize/this->fontSize;

		float totalLen = 0;
		unsigned code;
		for (size_t i=offset; i<strLength; i++) {


			code = (unsigned)(str[i]);

			if (code == L'\n') {
				charCnt++;
				break;
			}

			// ASCII文字かチェック
			if (code < L' ' || code > L'~')
				continue;

			unsigned subscr = code - L' ';

			if (isLimitOn && totalLen + texRes[subscr]->_chInfo().sizeW*scale > limit)
				break;

			totalLen += texRes[subscr]->_chInfo().AreaW*scale + _letterSpace;
			charCnt++;

		}
		length = (int)totalLen;

		return charCnt;
	}

}