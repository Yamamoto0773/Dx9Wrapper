#define _CRT_SECURE_NO_WARNINGS

#include "DirectXText.h"

#include <strsafe.h>
#include <locale.h>



// メモリの再開放を防ぐ
#define SAFE_FREE(x) { if (x) { free(x); x=NULL;} }
#define SAFE_RELEASE(x) {if (x) {x->Release(); x=NULL;} }


DirectXText::DirectXText() {
	// ロケールを日本に設定
	setlocale(LC_CTYPE, "jpn");

	lpDev			= NULL;
	lpVertexBuffer	= NULL;
	lpEffect		= NULL;
	lpDecl			= NULL;
	iDrawWidth		= 640;
	iDrawHeight		= 480;

	for (int j = 0; j < DFONT_MAXSTRING; j++) {
		pCharData[j] = NULL;
		pSetString[j] = NULL;
		iFontSize[j] = 0;
		for (int i=0; i<DFONT_MAXCHARACTER; i++)
			lpFontTex[j][i] = NULL;
	}

}


DirectXText::~DirectXText() {
	for (int j = 0; j < DFONT_MAXSTRING; j++) {
		Clear(j);
	}

	SAFE_RELEASE(lpVertexBuffer);
	SAFE_RELEASE(lpEffect);
	SAFE_RELEASE(lpDecl);
	lpDev = NULL;
}


BOOL DirectXText::Init(LPDIRECT3DDEVICE9 dev, int drawWidth, int drawHeight) {
	if (!dev)
		return FALSE;
	if (drawWidth <= 0 || drawHeight <= 0)
		return FALSE;


	iDrawWidth = drawWidth;
	iDrawHeight = drawHeight;
	lpDev = dev;


	// ポリゴン生成
	HRESULT hr;
	hr = lpDev->CreateVertexBuffer(sizeof(Vtx)*4, 0, 0, D3DPOOL_MANAGED, &lpVertexBuffer, 0);
	if (FAILED(hr))
		return FALSE;


	// 単位フォントポリゴン作成
	Vtx vtx[4] ={
		{ 0.0f, -1.0f, 1.0f,  0.0f, 1.0f },
		{ 0.0f,  0.0f, 1.0f,  0.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f,  1.0f, 1.0f },
		{ 1.0f,  0.0f, 1.0f,  1.0f, 0.0f },
	};
	Vtx *p = NULL;

	lpVertexBuffer->Lock(0, 0, (void**)&p, 0);
	memcpy(p, vtx, sizeof(Vtx)*4);
	lpVertexBuffer->Unlock();


	// シェーダ作成
	ID3DXBuffer	*error=NULL;
	hr = D3DXCreateEffectFromFile(lpDev, L"sprite.fx", 0, 0, 0, 0, &lpEffect, &error);
	if (FAILED(hr)) {
		OutputDebugStringA((const char*)error->GetBufferPointer());
		return FALSE;
	}


	// 頂点宣言作成
	D3DVERTEXELEMENT9 elems[] ={
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},				// 頂点位置 (x, y, z)
		{0, sizeof(float)*3, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},	// テクスチャ座標
		D3DDECL_END()
	};
	dev->CreateVertexDeclaration(elems, &lpDecl);
	if (!lpDecl)
		return FALSE;


	return TRUE;
}


BOOL DirectXText::SetString(int strID, FONTSTATUS * status, const char * s, ...) {
	if (!lpDev)
		return FALSE;
	if (!(0 <= strID && strID < DFONT_MAXSTRING))
		return FALSE;

	Clear(strID);

	///// 書式文字列をWCHAR型の文字列に変換 /////
	va_list ap;
	va_start(ap, s);


	// 書式変換文字列に従って通常の文字列に書き込み
	char tmp[DFONT_MAXCHARACTER*2+1];
	vsnprintf(tmp, DFONT_MAXCHARACTER*2+1, s, ap);

	va_end(ap);

	// wchar型に変換
	WCHAR tmp2[DFONT_MAXCHARACTER+1];
	mbstowcs(tmp2, tmp, DFONT_MAXCHARACTER+1);

	// 文字列の最適化
	WCHAR tmp3[DFONT_MAXCHARACTER+1];
	int strLen = OptimizeString(tmp3, tmp2);


	pSetString[strID] = (WCHAR*)malloc(sizeof(WCHAR)*(strLen+1));
	if (!pSetString[strID]) {
		Clear(strID);
		return FALSE;
	}

	wcsncpy(pSetString[strID], tmp3, strLen+1);


	pCharData[strID] = (CHARDATA*)malloc(sizeof(CHARDATA)*strLen);
	if (!pCharData) {
		Clear(strID);
		return FALSE;
	}



	// 階調の最大値を求める
	int grad = 0;
	switch (status->iAntiAliasing) {
		case GGO_GRAY2_BITMAP: grad = 4; break;
		case GGO_GRAY4_BITMAP: grad = 16; break;
		case GGO_GRAY8_BITMAP: grad = 64; break;
		default: grad = 16;
	}


	// フォントの生成
	LOGFONT	lf;
	lf.lfHeight				= status->iFontSize;			// 文字の高さ
	lf.lfWidth				= 0;							// 文字幅
	lf.lfEscapement			= 0;							// 文字方向とX軸との角度
	lf.lfOrientation		= 0;							// 各文字とX軸との角度
	lf.lfWeight				= status->iFontWeight;			// 太さ
	lf.lfItalic				= status->bItalic;				// イタリック体
	lf.lfUnderline			= status->bUnderline;			// 下線
	lf.lfStrikeOut			= status->bStruckOut;			// 打ち消し線
	lf.lfCharSet			= DEFAULT_CHARSET;				// キャラクタセット
	lf.lfOutPrecision		= OUT_DEFAULT_PRECIS;			// 出力精度
	lf.lfClipPrecision		= CLIP_DEFAULT_PRECIS;			// クリッピングの精度
	lf.lfQuality			= PROOF_QUALITY;				// 出力品質
	lf.lfPitchAndFamily		= DEFAULT_PITCH | FF_MODERN;	// ピッチとファミリ
	StringCchCopy(lf.lfFaceName, 32, status->wcFontName);	// フォント名

	HFONT hFont = CreateFontIndirect(&lf);
	if (hFont == NULL) {
		DeleteObject(hFont);
		Clear(strID);
		return FALSE;
	}
	iFontSize[strID] = status->iFontSize;

	// デバイスに選択したフォントを設定
	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);



	//// 文字テクスチャ作成 ////
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	CONST MAT2 mat ={ { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };

	for (int i = 0; i < strLen; i++) {
		GLYPHMETRICS	gm;
		DWORD size = 0;

		// 文字コード取得
		UINT code = (UINT)pSetString[strID][i];


		///// 例外文字の処理 ////
		bool space = false;
		switch (code) {
			case (UINT)L' ':
			case (UINT)L'　':
				space = true;
				break;
			case (UINT)L'\n':
				continue;
		}


		// 文字のグリフビットマップを取得
		if ((size = GetGlyphOutline(hdc, code, status->iAntiAliasing, &gm, 0, NULL, &mat)) == GDI_ERROR) {
			SelectObject(hdc, oldFont);			// 元のフォントに戻す
			ReleaseDC(NULL, hdc);
			DeleteObject(hFont);
			Clear(strID);
			return FALSE;
		}
		BYTE *pMono = new BYTE[size];
		GetGlyphOutline(hdc, code, status->iAntiAliasing, &gm, size, pMono, &mat);

		// 文字情報を保存
		pCharData[strID][i].iWidth			= (gm.gmBlackBoxX + 3) / 4 * 4;
		pCharData[strID][i].iHeight			= gm.gmBlackBoxY;
		pCharData[strID][i].iAreaWidth		= gm.gmCellIncX;
		pCharData[strID][i].iAreaHeight		= tm.tmHeight;
		pCharData[strID][i].iOriginX		= gm.gmptGlyphOrigin.x;
		pCharData[strID][i].iOriginY		= gm.gmptGlyphOrigin.y-tm.tmAscent;		// 左上原点
		


		// テクスチャ作成
		lpDev->CreateTexture(pCharData[strID][i].iWidth, pCharData[strID][i].iHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &lpFontTex[strID][i], NULL);

		if (!space) {
			// スペースでない場合
			// テクスチャにフォントビットマップ情報を書き込み
			D3DLOCKED_RECT lockedRect;
			lpFontTex[strID][i]->LockRect(0, &lockedRect, NULL, 0);  // ロック
			DWORD *texBuf = (DWORD*)lockedRect.pBits;   // テクスチャメモリへのポインタ

			for (int y = 0; y < pCharData[strID][i].iHeight; y++) {
				for (int x = 0; x < pCharData[strID][i].iWidth; x++) {
					DWORD alpha = pMono[y * pCharData[strID][i].iWidth + x] * 255 / grad;
					texBuf[y * pCharData[strID][i].iWidth + x] = (alpha << 24) & 0xff000000;
				}
			}

			lpFontTex[strID][i]->UnlockRect(0);  // アンロック
		}

		delete[] pMono;
	}

	// デバイスコンテキストとフォントはもういらないので解放
	SelectObject(hdc, oldFont);			// 元のフォントに戻す
	ReleaseDC(NULL, hdc);
	DeleteObject(hFont);				// 今回のフォントを消去


	return TRUE;
}



BOOL DirectXText::Draw(int strID, int x, int y, int fontSize, int charInterval, DWORD color) {
	BOOL result;
	RECT rect ={ x, y, 0, 0 };

	result = DrawInRect(strID, &rect, fontSize, charInterval, TEXTALIGN_NONE|TEXTSCALE_NONE, color);

	return result;
}



BOOL DirectXText::DrawInRect(int strID, RECT * rect, int fontSize, int charInterval, DWORD format, DWORD color) {
	if (!lpDev)
		return FALSE;
	if (!lpVertexBuffer)
		return FALSE;
	if (!lpEffect)
		return FALSE;
	if (!lpDecl)
		return FALSE;
	if (!rect)
		return FALSE;
	if (!(0 <= strID && strID < DFONT_MAXSTRING))
		return FALSE;
	if (!pSetString[strID])
		return FALSE;
	

	// 描画位置の算出
	POINT *pt = NULL;
	pt = (POINT*)malloc(sizeof(POINT)*wcslen(pSetString[strID]));
	if (!pt)
		return FALSE;

	float tmp = (float)fontSize/iFontSize[strID];
	float scale;

	int charCnt = CalcTextPosition(strID, rect, tmp, charInterval, format, pSetString[strID], pt, &scale);
	if (charCnt < 0)
		return FALSE;


	// 描画倍率設定
	float scaleX, scaleY;
	if (((format&0xF0)) == TEXTSCALE_AUTOX)
		scaleX = tmp*scale, scaleY = tmp;
	else if (((format&0xF0)) == TEXTSCALE_AUTOXY)
		scaleX = tmp*scale, scaleY = tmp*scale;
	else
		scaleX = tmp, scaleY = tmp;


	// 指定色をfloat型配列に変換
	float colorRGBA[4]={
		(color>>16 & 0x000000ff) / 255.0f,
		(color>>8 & 0x000000ff) /255.0f,
		(color & 0x000000ff) / 255.0f,
		(color>>24 & 0x000000ff) / 255.0f
	};


	//-------------------描画設定---------------------------------------
	// ライトはオフで
	lpDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	// αブレンド設定
	lpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	lpDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	lpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//テクスチャのアルファを透明度を使用する設定
	lpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	lpDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	lpDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	lpDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	lpDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	lpDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);					// 左手系

	lpDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);			// テクスチャがはみ出た時に表示しないにする
	lpDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	lpDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	lpDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//-------------------------------------------------------------------


	D3DXMATRIX worldOffset;						// ポリゴンの配置座標
	D3DXMATRIX localScale;						// ポリゴンの大きさ (拡大倍率
	D3DXMATRIX localOffset;						// ポリゴンの原点座標
	D3DXMATRIX localMat;
	D3DXMATRIX world;

	// 左手座標系正射影行列を設定
	D3DXMATRIX ortho;
	D3DXMatrixOrthoLH(&ortho, (float)iDrawWidth, (float)iDrawHeight, 0.0f, 1000.0f);
	lpDev->SetTransform(D3DTS_PROJECTION, &ortho);


	//// 文字描画 /////
	lpDev->SetVertexDeclaration(lpDecl);
	lpDev->SetStreamSource(0, lpVertexBuffer, 0, sizeof(Vtx));
	lpEffect->SetTechnique("BasicTech");

	// シェーダ開始
	UINT numPass = 0;
	lpEffect->Begin(&numPass, 0);

	for (int i=0; i<charCnt; i++) {

		// 描画する文字の文字コード取得
		UINT code = (UINT)pSetString[strID][i];


		// テクスチャが存在するかチェック
		if (!lpFontTex[strID][i])
			continue;


		// スクリーンの左上隅を原点とした座標に変換
		pt[i].x -= iDrawWidth / 2;
		pt[i].y -= iDrawHeight / 2;
		pt[i].y *= -1;


		// ワールドビュー射影変換行列を作成
		D3DXMatrixScaling(&localScale, (float)pCharData[strID][i].iWidth*scaleX, (float)pCharData[strID][i].iHeight*scaleY, 1.0f);				// ポリゴンを文字の大きさにする
		D3DXMatrixTranslation(&localOffset, (float)pCharData[strID][i].iOriginX*scaleX, (float)pCharData[strID][i].iOriginY*scaleY, 0.0f);		// ポリゴンを文字の原点に移動
		D3DXMATRIX localMat = localScale*localOffset;
		D3DXMatrixTranslation(&worldOffset, (float)pt[i].x -0.5f, (float)pt[i].y +0.5f, 0.0f);
		world = localMat * worldOffset;
		D3DXMATRIX matWorldViewProj = world*ortho;

		// シェーダ定数設定
		lpEffect->SetMatrix("matWorldViewProj", &matWorldViewProj);	// ワールドビュー射影変換行列を設定
		lpEffect->SetFloatArray("color", colorRGBA, 4);				// 色指定
		lpEffect->SetTexture("tex", lpFontTex[strID][i]);			// テクスチャ指定

		// 描画開始
		lpEffect->BeginPass(0);
		lpDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);			// 描画
		lpEffect->EndPass();
		
	}
	// シェーダ終了
	lpEffect->End();

	free(pt);

	return TRUE;
}


BOOL DirectXText::Clear(int strID) {
	if (!(0 <= strID && strID < DFONT_MAXSTRING))
		return FALSE;

	for (int i = 0; i < DFONT_MAXCHARACTER; i++) {
		SAFE_RELEASE(lpFontTex[strID][i]);
	}

	SAFE_FREE(pCharData[strID]);
	SAFE_FREE(pSetString[strID]);

	iFontSize[strID] = 0;

	return TRUE;
}


int DirectXText::CalcTextPosition(int strID, RECT * rect, float inScale, int charInterval, DWORD format, const WCHAR * s, POINT * pt, float *outScale) {
	if (!pt)
		return FALSE;
	if (!s)
		return FALSE;
	if (!rect)
		return FALSE;
	if (!outScale)
		return FALSE;

	// フォーマットの組み合わせをチェック
	if (((format&0x0F)) == TEXTALIGN_NONE) {
		if (((format&0xF0)) == TEXTSCALE_AUTOX || ((format&0xF0)) == TEXTSCALE_AUTOXY)
			return FALSE;
	}

	int charCnt = 0;

	// 領域の縦横の長さを求める
	int rectX = rect->right - rect->left;
	int rectY = rect->bottom - rect->top;


	int lineHead = 0;	// 計算対象である行の、先頭文字の番号
	int lineEnd = 0;	// 計算対象である行の、最後の文字の番号
	float offsetX = 0;
	float offsetY = 0;
	float scaleX = 1.0f;
	float scaleY = 1.0f;


	while (lineHead < wcslen(s)) {
		float lineLen = 0.0f;
		float lineHeight = pCharData[strID][0].iAreaHeight*inScale;

		// 縦方向にはみ出したら
		if (offsetY + lineHeight > rectY) {
			if (((format&0x0F)) != TEXTALIGN_NONE && ((format&0xF0)) != TEXTSCALE_AUTOXY) {
				break;
			}
		}


		//// 行の長さの算出 ////
		bool canPut = false;
		int i = -1;
		while ((lineHead+ ++i) < wcslen(s)) {

			// 文字コード取得
			UINT code = (UINT)s[lineHead +i];

			
			// 改行なら行の終わりとする
			if (code == (UINT)'\n') {
				i++;
				canPut = true;
				break;
			}

			// 横方向にはみ出したら
			if (lineLen + pCharData[strID][lineHead +i].iAreaWidth*inScale > rectX) {
				if (((format&0x0F)) != TEXTALIGN_NONE && (format&0xF0) == TEXTSCALE_NONE) {
					break;
				}
			}

			lineLen += pCharData[strID][lineHead +i].iAreaWidth*inScale + charInterval;
			canPut = true;
		}

		if (!canPut)	// 1文字も配置できない場合は終了
			break;

		lineEnd = lineHead + i-1;
		lineLen -= charInterval;	// 行末の空白を削除

		
		// 縮小倍率計算
		if ((format&0xF0) != TEXTSCALE_NONE) {
			if (lineLen > rectX)
				scaleX = rectX/lineLen;
			if (lineHeight > rectY)
				scaleY = rectY/lineHeight;

			if ((format&0xF0) == TEXTSCALE_AUTOXY)
				*outScale = (scaleX < scaleY) ? scaleX : scaleY; // 小さい方に合わせて縦横比を維持
			else
				*outScale = scaleX;

			lineLen *= (*outScale);
			lineHeight *= (*outScale);
		}
		else {
			*outScale = 1.0f;
		}
		

		// 行頭の描画位置の算出
		switch ((format&0x0F)) {
			case TEXTALIGN_CENTERX:
				offsetX = (rectX - lineLen)/2;
				break;
			case TEXTALIGN_CENTERXY:
				offsetX = (rectX - lineLen)/2;
				offsetY = (rectY - lineHeight)/2;
				break;
			case TEXTALIGN_RIGHT:
				offsetX = rectX - lineLen;
				break;
			case TEXTALIGN_LEFT:
			case TEXTALIGN_NONE:
				offsetX = 0;
		}


		// 描画位置の保存
		for (charCnt=lineHead; charCnt<=lineEnd; charCnt++) {
			pt[charCnt].x = rect->left + (int)offsetX;
			pt[charCnt].y = rect->top + (int)offsetY;

			offsetX += (pCharData[strID][charCnt].iAreaWidth*inScale + charInterval)*(*outScale);
		}

		
		// 改行処理
		if ((format&0xF0) == TEXTSCALE_AUTOX ||
			(format&0xF0) == TEXTSCALE_AUTOXY ||
			(format&0x0F) == TEXTALIGN_CENTERXY)		// 1行のみのフォーマットはここで終了
			break;

		offsetX = 0;
		offsetY += lineHeight;

		lineHead = lineEnd+1;	// 行頭文字を更新
	}

	return charCnt;
}



BOOL DirectXText::OptimizeString(WCHAR *dst, const WCHAR *src) {
	if (!src)
		return -1;
	if (!dst)
		return -1;

	int cnt = 0;

	for (int i=0; i<wcslen(src); i++) {
		switch (src[i]) {
			case L'\t':
			case L'\a':
			case L'\b':
			case L'\f':
			case L'\r':
			case L'\v':
				break;
			default:
				dst[cnt++] = src[i];
		}
	}

	dst[cnt] = L'\0';	// NULL文字

	return cnt;
}




BOOL DirectXText::SetFontStatus(FONTSTATUS * status, int fontSize, WCHAR * fontName, int fontWeight, bool italic) {
	BOOL b;

	b = SetFontStatusEX(status, fontSize, fontName, FF_DONTCARE, italic, false, false, GGO_GRAY4_BITMAP);

	return b;
}


BOOL DirectXText::SetFontStatusEX(FONTSTATUS * status, int fontSize, WCHAR * fontName, int fontWeight, bool italic, bool underLine, bool struckOut, const int antiAliasing) {
	if (!status)
		return FALSE;
	if (fontSize <= 0)
		return FALSE;
	if (!(0 <= fontWeight && fontWeight <= 1000))
		return FALSE;
	if (!(antiAliasing == GGO_GRAY2_BITMAP || antiAliasing == GGO_GRAY4_BITMAP || antiAliasing == GGO_GRAY8_BITMAP))
		return FALSE;


	status->iFontSize		= fontSize;
	status->iFontWeight		= fontWeight;
	status->iAntiAliasing	= antiAliasing;
	status->bItalic			= italic;
	status->bUnderline		= underLine;
	status->bStruckOut		= struckOut;
	StringCchCopy(status->wcFontName, 32, fontName);

	return TRUE;
}




DWORD DirectXText::ConvertFromRGBA(int red, int green, int blue, int alpha) {
	if (red > 255)		red = 255;
	if (red < 0)		red = 0;
	if (green > 255)	green = 255;
	if (green < 0)		green = 0;
	if (blue > 255)		blue = 255;
	if (blue < 0)		blue = 0;
	if (alpha > 255)	alpha = 255;
	if (alpha < 0)		alpha = 0;

	DWORD color = 0x00000000;

	color = alpha << 24 | red << 16 | green << 8 | blue;

	return color;
}
