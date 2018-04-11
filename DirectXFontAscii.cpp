#define _USE_MATH_DEFINES
#include <math.h>  
#include "DirectXFontAscii.hpp"

namespace dx9 {



	DirectXFontAscii::DirectXFontAscii() {
		hFont = nullptr;
		texFilter = TextureFilter::LINEAR;
	}


	DirectXFontAscii::~DirectXFontAscii() {
	}


	bool DirectXFontAscii::Create(const char * fontName, size_t fontSize, FontWeight fontWeight, bool isItalic, bool isUnderLine, bool isStrikeOut, AntialiasLevel level) {
		DeleteAll();

		// フォントハンドルの生成
		LOGFONTA	lf;
		lf.lfHeight				= fontSize;						// 文字の高さ
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
				texRes[i] = std::make_unique<FontTextureA>();
				if (!texRes[i]->Create(d3ddev9, hdc, ' ' + i, antialiasLv, &tm)) {
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

	bool DirectXFontAscii::Draw(float x, float y, DWORD color, const char* s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, TEXTALIGN_NONE|TEXTSCALE_NONE, color, 0, -1, fontSize, 0, 0, FontRotOrigin::CENTER, s, vlist);
		va_end(vlist);

		return result;
	}


	bool DirectXFontAscii::DrawInRect(RectF& rect, DWORD format, DWORD color, const char * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, format, color, 0, -1, fontSize, 0, 0, FontRotOrigin::CENTER, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::Draw(float x, float y, DWORD color, size_t startCharCnt, int drawCharCnt, const char * s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, TEXTALIGN_NONE|TEXTSCALE_NONE, color, startCharCnt, drawCharCnt, fontSize, 0, 0, FontRotOrigin::CENTER, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::DrawInRect(RectF & rect, DWORD format, DWORD color, size_t startCharCnt, int drawCharCnt, const char * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, format, color, startCharCnt, drawCharCnt, fontSize, 0, 0, FontRotOrigin::CENTER, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::Draw(float x, float y, DWORD color, float rotateDeg, FontRotOrigin rotOrigin, const char * s, ...) {
		RectF rect = {x, y, 0.0f, 0.0f};

		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, TEXTALIGN_NONE|TEXTSCALE_NONE, color, 0, -1, fontSize, 0, rotateDeg, rotOrigin, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::DrawInRect(RectF & rect, DWORD format, DWORD color, float rotateDeg, FontRotOrigin rotOrigin, const char * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, format, color, 0, -1, fontSize, 0, rotateDeg, rotOrigin, s, vlist);
		va_end(vlist);

		return result;
	}

	bool DirectXFontAscii::DrawInRect(RectF & rect, DWORD format, DWORD color, size_t startCharCnt, int drawCharCnt, size_t fontSize, float letterSpace, float rotateDeg, FontRotOrigin rotOrigin, const char * s, ...) {
		va_list vlist;
		va_start(vlist, s);
		bool result = DrawFont(rect, format, color, startCharCnt, drawCharCnt, fontSize, letterSpace, rotateDeg, rotOrigin, s, vlist);
		va_end(vlist);

		return result;
	}


	bool DirectXFontAscii::DrawFont(RectF & rect, DWORD format, DWORD color, size_t startCharCnt, int drawCharCnt, size_t fontSize, float letterSpace, float rotateDeg, FontRotOrigin rotOrigin, const char * s, va_list vlist) {
		if (!isDrawable()) {
			return false;
		}
		if (!hFont) {
			return false;
		}

		using namespace std;

		// 書式文字列をchar型の文字列に変換
		_locale_t locale = _wcreate_locale(LC_CTYPE, L"jpn");
		string str(CHARACTER_MAXCNT, '\0');
		int strLen = _vsnprintf_s_l(&str[0], CHARACTER_MAXCNT+1, CHARACTER_MAXCNT, s, locale, vlist);
		_free_locale(locale);


		str.resize(strLen);


		// 描画位置の算出 
		vector<PointF> pos(strLen);
		float inScale = (float)fontSize/this->fontSize;
		float outScale;
		RectF strArea;

		int charCnt = CalcTextPosition(str.c_str(), rect, inScale, letterSpace, format, pos, strArea, outScale);
		if (charCnt < 0) {
			return false;
		}



		// 描画倍率設定
		float scaleX, scaleY;
		if (((format&0xF0)) == TEXTSCALE_AUTOX) {
			scaleX = inScale*outScale, scaleY = inScale;
		}
		else if (((format&0xF0)) == TEXTSCALE_AUTOXY) {
			scaleX = inScale*outScale, scaleY = inScale*outScale;
		}
		else {
			scaleX = inScale, scaleY = inScale;
		}


		// 指定色をfloat型配列に変換
		float colorRGBA[4]= {
			(color>>16 & 0x000000ff) / 255.0f,
			(color>>8 & 0x000000ff) /255.0f,
			(color & 0x000000ff) / 255.0f,
			(color>>24 & 0x000000ff) / 255.0f
		};


		// 引数を最適化
		float rotateRad = (float)(M_PI * rotateDeg/180.0f);
		size_t endCharCnt = 0;
		if (drawCharCnt < 0 || startCharCnt + drawCharCnt > charCnt) {
			endCharCnt = charCnt;
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


		// シェーダ開始
		UINT numPass = 0;
		effect->SetTechnique("Tech");
		effect->Begin(&numPass, 0);
		effect->BeginPass(static_cast<UINT>(shader::ShaderPass::Mul_ColorAlpha_TexAlpha));


		for (size_t i=startCharCnt; i<endCharCnt; i++) {
			UINT subscr = str[i] - ' ';


			if (subscr<0 || subscr>=texRes.size() || !texRes[subscr]) {
				continue;
			}

			PointF origin = {
				rotOriginPt.x - pos[i].x,
				rotOriginPt.y - pos[i].y
			};

			D3DXMATRIX world, scale, rot;
			D3DXMatrixScaling(&world, (float)texRes[subscr]->GetWidth(), texRes[subscr]->GetHeight(), 1.0f);	// ポリゴンサイズに
			D3DXMatrixScaling(&scale, scaleX, scaleY, 1.0f);	// ローカルスケール
			D3DXMatrixRotationZ(&rot, rotateRad);						// 回転
			world._41 = -origin.x;		// ピボット分オフセット
			world._42 = -origin.y;
			world = world * scale * rot;
			world._41 += pos[i].x + origin.x;	// ピボット分オフセット
			world._42 += pos[i].y + origin.y;
			world._43 += GetTopLayerPos()/1000.0f;

			// ラスタライゼーションルールを用いて，テクスチャをずらす
			world._41 = ceil(world._41) - 0.5f;
			world._42 = floor(world._42) + 0.5f;

			effect->SetMatrix("world", &world);
			effect->SetMatrix("proj", &projMat);
			effect->SetTexture("tex", texRes[subscr]->GetPointer());
			effect->SetFloatArray("color", colorRGBA, 4);
			effect->CommitChanges();
			d3ddev9->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		}

		effect->EndPass();
		effect->End();

		return true;
	}


	bool DirectXFontAscii::DeleteAll() {
		fontSize = 0;
		antialiasLv = AntialiasLevel::_15STEPS;
		texFilter = TextureFilter::NONE;

		if (hFont != nullptr) {
			DeleteObject(hFont);
		}

		texRes.clear();

		return true;
	}


	int DirectXFontAscii::CalcTextPosition(const std::string &str, RectF & rect, float inScale, int letterSpace, DWORD format, std::vector<PointF>& pt, RectF & strArea, float & outScale) {

		strArea.left = 0;
		strArea.right = 0;
		strArea.top = 0;
		strArea.bottom = 0;

		// フォーマットの組み合わせをチェック
		if (((format&0x0F)) == TEXTALIGN_NONE) {
			if (((format&0xF0)) == TEXTSCALE_AUTOX || ((format&0xF0)) == TEXTSCALE_AUTOXY)
				return false;
		}

		int charCnt = 0;

		// 領域の縦横の長さを求める
		float rectX = rect.right - rect.left;
		float rectY = rect.bottom - rect.top;

		int lineHead = 0;	// 計算対象である行の、先頭文字の番号
		int lineEnd = 0;	// 計算対象である行の、最後の文字の番号
		float offsetX = 0;
		float offsetY = 0;
		float scaleX = 1.0f;
		float scaleY = 1.0f;

		float fontHeight = tm.tmHeight;

		while (lineHead < str.size()) {
			float lineLen = 0.0f;
			float lineHeight = fontHeight*inScale;

			// 縦方向にはみ出したら
			if (offsetY + lineHeight > rectY) {
				if (((format&0x0F)) != TEXTALIGN_NONE && ((format&0xF0)) != TEXTSCALE_AUTOXY) {
					break;
				}
			}

			//// 行の長さの算出 ////
			bool canPut = false;
			int i = -1;
			while ((lineHead+ ++i) < str.size()) {

				// 文字コード取得
				UINT code = (UINT)(str[lineHead +i]);

				// 改行なら行の終わりとする
				if (code == (UINT)L'\n') {
					i++;
					canPut = true;
					break;
				}

				// ASCII文字かチェック
				if (code < ' ' || code > '~') {
					return -1;
				}


				// 横方向にはみ出したら
				if (lineLen + texRes[code-' ']->_chInfo().AreaW*inScale > rectX) {
					if (((format&0x0F)) != TEXTALIGN_NONE && (format&0xF0) == TEXTSCALE_NONE) {
						break;
					}
				}

				lineLen += texRes[code-' ']->_chInfo().AreaW*inScale + letterSpace;
				canPut = true;
			}

			if (!canPut)	// 1文字も配置できない場合は終了
				break;

			lineEnd = lineHead + i-1;
			lineLen -= letterSpace;	// 行末の空白を削除


									// 縮小倍率計算
			if ((format&0xF0) != TEXTSCALE_NONE) {
				if (lineLen > rectX)
					scaleX = rectX/lineLen;
				if (lineHeight > rectY)
					scaleY = rectY/lineHeight;

				if ((format&0xF0) == TEXTSCALE_AUTOXY)
					outScale = (scaleX < scaleY) ? scaleX : scaleY; // 小さい方に合わせて縦横比を維持
				else
					outScale = scaleX;

				lineLen *= outScale;
				lineHeight *= outScale;
			}
			else {
				outScale = 1.0f;
			}


			// 行頭の描画位置の算出
			switch ((format&0x0F)) {
				case TEXTALIGN_CENTERX:
					offsetX = (rectX - lineLen)/2.0f;
					break;
				case TEXTALIGN_CENTERXY:
					offsetX = (rectX - lineLen)/2.0f;
					offsetY = (rectY - lineHeight)/2;
					break;
				case TEXTALIGN_RIGHT:
					offsetX = rectX - lineLen;
					break;
				case TEXTALIGN_LEFT:
				case TEXTALIGN_NONE:
					offsetX = 0;
			}

			if (offsetX < strArea.left) {
				strArea.left = offsetX;
			}

			if (offsetY < strArea.top) {
				strArea.top = offsetY;
			}


			// 描画位置の保存
			for (charCnt=lineHead; charCnt<=lineEnd; charCnt++) {
				// 文字コード取得
				UINT subscr = (UINT)(str[charCnt]) - ' ';

				if (subscr<0 || subscr>=texRes.size() || !texRes[subscr]) {
					continue;
				}

				pt[charCnt].x = rect.left + texRes[subscr]->_chInfo().originX + offsetX;
				pt[charCnt].y = rect.top - texRes[subscr]->_chInfo().originY + offsetY;

				offsetX += (texRes[subscr]->_chInfo().AreaW*inScale + letterSpace)*outScale;
			}

			if ((offsetX - letterSpace*outScale) > strArea.right) {
				strArea.right = offsetX - letterSpace*outScale;
			}


			// 改行処理
			offsetX = 0;
			offsetY += lineHeight;
			strArea.bottom += lineHeight;

			lineHead = lineEnd+1;	// 行頭文字を更新

									// 1行のみのフォーマットはここで終了
			if ((format&0xF0) == TEXTSCALE_AUTOX ||
				(format&0xF0) == TEXTSCALE_AUTOXY ||
				(format&0x0F) == TEXTALIGN_CENTERXY)
				break;

		}

		strArea.left += rect.left;
		strArea.top += rect.top;
		strArea.right += rect.left;
		strArea.bottom += rect.top;


		return charCnt;
	}




}