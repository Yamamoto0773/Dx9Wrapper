#define _USE_MATH_DEFINES
#include <math.h>  
#include "DirectXFontAscii.hpp"


namespace dx9 {



	DirectXFontAscii::DirectXFontAscii() {
		hFont = nullptr;
		fontColor = {0.0f, 0.0f, 0.0f, 1.0f};
	}


	DirectXFontAscii::~DirectXFontAscii() {
	}


	bool DirectXFontAscii::Create(const char * fontName, size_t fontSize, FontWeight fontWeight, bool isItalic, bool isUnderLine, bool isStrikeOut, AntialiasLevel level) {
		DeleteAll();

		// フォントハンドルの生成
		LOGFONTA	lf;
		lf.lfHeight				= (LONG)fontSize;						// 文字の高さ
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
		strcpy_s(lf.lfFaceName, 32, fontName);

		hFont = CreateFontIndirectA(&lf);
		if (hFont == nullptr) {
			return false;
		}


		this->fontSize = fontSize;
		antialiasLv = level;

	
		// デバイスにフォントを設定
		HDC hdc = GetDC(NULL);
		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		// フォント情報を取得
		GetTextMetricsA(hdc, &tm);

		// ASCII文字のテクスチャを作成
		texRes.resize('~'-' '+1);

		for (size_t i=0; i<texRes.size(); i++) {
			if (!texRes[i]) {
				texRes[i] = std::make_unique<texture::FontTextureA>();
				if (!texRes[i]->Create(d3ddev9, hdc, (char)(' ' + i), antialiasLv, &tm)) {
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

	bool DirectXFontAscii::Draw(float x, float y,  const char* s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, TextAlign::NONE, 0, -1, fontSize, 0, 0, FontRotOrigin::CENTER, s, vlist);
		va_end(vlist);

		return result;
	}


	bool DirectXFontAscii::DrawInRect(RectF& rect, TextAlign format,  const char * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, format, 0, -1, fontSize, 0, 0, FontRotOrigin::CENTER, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::Draw(float x, float y,  size_t startCharCnt, int drawCharCnt, const char * s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, TextAlign::NONE, startCharCnt, drawCharCnt, fontSize, 0, 0, FontRotOrigin::CENTER, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::DrawInRect(RectF & rect, TextAlign format,  size_t startCharCnt, int drawCharCnt, const char * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, format, startCharCnt, drawCharCnt, fontSize, 0, 0, FontRotOrigin::CENTER, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::Draw(float x, float y,  float rotateDeg, FontRotOrigin rotOrigin, const char * s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, TextAlign::NONE, 0, -1, fontSize, 0, rotateDeg, rotOrigin, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::DrawInRect(RectF & rect, TextAlign format,  float rotateDeg, FontRotOrigin rotOrigin, const char * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, format, 0, -1, fontSize, 0, rotateDeg, rotOrigin, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::DrawInRect(RectF & rect, TextAlign format,  size_t startCharCnt, int drawCharCnt, size_t fontSize, float letterSpace, float rotateDeg, FontRotOrigin rotOrigin, const char * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, format, startCharCnt, drawCharCnt, fontSize, letterSpace, rotateDeg, rotOrigin, s, vlist);
		va_end(vlist);

		return result;
	}


	bool DirectXFontAscii::DrawFont(RectF & rect, TextAlign format, size_t startCharCnt, int drawCharCnt, size_t fontSize, float letterSpace, float rotateDeg, FontRotOrigin rotOrigin, const char * s, va_list vlist) {
		if (!isDrawable()) {
			return false;
		}
		if (!hFont) {
			return false;
		}
		if (format != TextAlign::NONE) {
			if (rect.left > rect.right || rect.top > rect.bottom) {
				return false;
			}
		}

		using namespace std;

		// 書式文字列をchar型の文字列に変換
		_locale_t locale = _wcreate_locale(LC_CTYPE, L"jpn");
		int len = _vsnprintf_s_l(&workBuf[0], CHARACTER_MAXCNT+1, CHARACTER_MAXCNT, s, locale, vlist);
		_free_locale(locale);

		if (len == 0) {
			return true;
		}
		

		// 描画位置の算出 
		vector<pair<float, size_t>> headPos; // first:行頭のx座標 second:行頭文字が何番目か
		RectF strArea = {rect.right, rect.bottom, rect.left, rect.top};
		size_t totalOffset = 0;
		int width = (int)(rect.right - rect.left);
		if (format == TextAlign::NONE) width = -1;
		int height = (int)(rect.bottom - rect.top);
		size_t lineCnt;

		for (lineCnt = 0; true; lineCnt++) {

			if (format != TextAlign::NONE && fontSize*lineCnt > height)
				break;

			size_t len;
			int offset = GetStrLength(workBuf.data(), totalOffset, (float)width, len);
			if (offset <= 0) {
				break;
			}

			float x;
			switch (format) {
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

			
			headPos.push_back({x, totalOffset});


			if (x < strArea.left)
				strArea.left = x;
			if (x + len > strArea.right)
				strArea.right = x + len;


			totalOffset += offset;
		}
		if (totalOffset == 0) return false;
		


		if (format == TextAlign::CENTERXY)
			strArea.top = rect.top + (height - lineCnt*fontSize)/2.0f;
		else
			strArea.top = rect.top;

		strArea.bottom = strArea.top + lineCnt*fontSize;


		// 引数を最適化
		float rotateRad = (float)(M_PI * rotateDeg/180.0f);
		size_t endCharCnt = 0;
		if (drawCharCnt < 0 || startCharCnt + drawCharCnt > totalOffset) {
			endCharCnt = totalOffset;
		}
		else {
			endCharCnt = startCharCnt + drawCharCnt;
		}


		// 文字列回転の原点を設定 
		PointF rotOriginPt;
		switch (rotOrigin) {
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


		// シェーダ開始
		UINT numPass = 0;
		effect->SetTechnique("Tech");
		effect->Begin(&numPass, 0);
		effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Mul_ColorAlpha_TexAlpha));


		D3DXMATRIX world, scale, rot;
		PointF pos = {0, 0};
		PointF finalPos = {0, 0};	// 最終的な描画位置
		auto nowHead = headPos.cbegin();
		auto ch = workBuf.cbegin();

		for (size_t i=startCharCnt; i<endCharCnt; i++) {
			UINT subscr = *ch - ' ';
			ch++;

			if (subscr<0 || subscr>=texRes.size() || !texRes[subscr]) {
				continue;
			}
			if (nowHead != headPos.cend() && i == nowHead->second) {
				pos.x = nowHead->first;
				pos.y = strArea.top + fontSize*distance(headPos.cbegin(), nowHead);
				nowHead++;
			}

			if (i < startCharCnt)
				continue;

			finalPos.x = pos.x + texRes[subscr]->_chInfo().originX;
			finalPos.y = pos.y - texRes[subscr]->_chInfo().originY;
			
			PointF origin = {
				rotOriginPt.x - finalPos.x,
				rotOriginPt.y - finalPos.y
			};

			
			D3DXMatrixScaling(&world, (float)texRes[subscr]->GetWidth(), (float)texRes[subscr]->GetHeight(), 1.0f);	// ポリゴンサイズに
			D3DXMatrixScaling(&scale, 1.0f, 1.0f, 1.0f);	// ローカルスケール
			D3DXMatrixRotationZ(&rot, rotateRad);						// 回転
			world._41 = -origin.x;		// ピボット分オフセット
			world._42 = -origin.y;
			world = world * scale * rot;
			world._41 += finalPos.x + origin.x;	// ピボット分オフセット
			world._42 += finalPos.y + origin.y;
			world._43 += topLayerPos/1000.0f;

			// ラスタライゼーションルールを用いて，テクスチャをずらす
			world._41 = ceil(world._41) - 0.5f;
			world._42 = floor(world._42) + 0.5f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", &projMat);
			effect->SetTexture("tex", texRes[subscr]->GetPointer());
			effect->SetFloatArray("color", fontColor.data(), 4);
			effect->CommitChanges();
			d3ddev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


			pos.x += texRes[subscr]->_chInfo().AreaW;
		}

		effect->EndPass();
		effect->End();

		return true;
	}


	bool DirectXFontAscii::DeleteAll() {
		fontSize = 0;
		antialiasLv = AntialiasLevel::_15STEPS;

		if (hFont != nullptr) {
			DeleteObject(hFont);
		}

		texRes.clear();

		return true;
	}

	int DirectXFontAscii::GetStrLength(const char* str, size_t offset, float limit, size_t & length) {
		size_t strLength = strlen(str);

		if (offset >= strLength) {
			return -1;
		}

		using namespace std;

		
		bool isLimitOn = (limit >= 0.0f);
		int charCnt = 0;

		size_t totalLen = 0;
		unsigned code;
		for (size_t i=offset; i<strLength; i++) {


			code = (unsigned)(str[i]);

			if (code == '\n') {
				charCnt++;
				break;
			}

			// ASCII文字かチェック
			if (code < ' ' || code > '~')
				continue;
				
			unsigned subscr = code - ' ';

			if (isLimitOn && totalLen + texRes[subscr]->_chInfo().sizeW > limit)
				break;

			totalLen += texRes[subscr]->_chInfo().AreaW;
			charCnt++;

		}
		length = totalLen;

		return charCnt;
	}



	
void DirectXFontAscii::SetFontColor(DWORD &color) {
	color &= 0xffffffff;
	fontColor[0] = (color>>16 & 0xff) / 255.0f;
	fontColor[1] = (color>>8 & 0xff) /255.0f;
	fontColor[2] = (color & 0xff) / 255.0f;
	fontColor[3] = (color>>24 & 0xff) / 255.0f;
}

void DirectXFontAscii::SetFontColor(size_t r, size_t g, size_t b, size_t a) {
	r &= 255;
	g &= 255;
	b &= 255;
	a &= 255;

	fontColor[0] = r/255.0f;
	fontColor[1] = g/255.0f;
	fontColor[2] = b/255.0f;
	fontColor[3] = a/255.0f;
}


}