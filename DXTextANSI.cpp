#include "DXTextANSI.h"

DXTextANSI::DXTextANSI() {
	lpDev = NULL;
	lpVertexBuffer = NULL;
	lpEffect = NULL;
	lpDecl = NULL;

	iDrawWidth = 640;
	iDrawHeight = 480;
	iFontSize = 0;

	ZeroMemory(mCharData, sizeof(mCharData));
	for (int i=0; i<CHARACTERKIND; i++) {
		lpFontTex[i] = NULL;
	}

}

DXTextANSI::~DXTextANSI() {
	Clear();

	SAFE_RELEASE(lpVertexBuffer);
	SAFE_RELEASE(lpEffect);
	SAFE_RELEASE(lpDecl);
}


BOOL DXTextANSI::Init(LPDIRECT3DDEVICE9 dev, int drawWidth, int drawHeight) {
	if (!dev)
		return FALSE;
	if (drawWidth<=0 || drawHeight<=0)
		return FALSE;

	// ロケールを日本に設定
	setlocale(LC_CTYPE, "jpn");

	lpDev		= dev;
	iDrawWidth	= drawWidth;
	iDrawHeight	= drawHeight;


	// ポリゴン生成
	HRESULT hr;
	hr = lpDev->CreateVertexBuffer(sizeof(Vtx)*4, 0, 0, D3DPOOL_MANAGED, &lpVertexBuffer, 0);
	if (FAILED(hr))
		return FALSE;

	// 単位フォントポリゴン作成
	Vtx vtx[4] ={
		{ 0.0f, -1.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f,  0.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f,  0.0f, 1.0f, 1.0f, 0.0f },
	};
	Vtx *p = NULL;

	lpVertexBuffer->Lock(0, sizeof(Vtx)*4, (void**)&p, 0);
	memcpy(p, vtx, sizeof(Vtx)*4);
	lpVertexBuffer->Unlock();


	// シェーダ作成
	ID3DXBuffer	*error=NULL;
	if (FAILED(D3DXCreateEffectFromFile(lpDev, L"sprite.fx", 0, 0, 0, 0, &lpEffect, &error))) {
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


BOOL DXTextANSI::Create(int fontSize, int fontWeight, WCHAR * fontName, bool italic) {
	if (!lpDev)
		return FALSE;

	Clear();

	// フォントの生成
	LOGFONT	lf;
	lf.lfHeight				= fontSize;						// 文字の高さ
	lf.lfWidth				= 0;							// 文字幅
	lf.lfEscapement			= 0;							// 文字方向とX軸との角度
	lf.lfOrientation		= 0;							// 各文字とX軸との角度
	lf.lfWeight				= fontWeight;					// 太さ
	lf.lfItalic				= italic;						// イタリック体
	lf.lfUnderline			= FALSE;						// 下線
	lf.lfStrikeOut			= FALSE;						// 打ち消し線
	lf.lfCharSet			= ANSI_CHARSET;					// キャラクタセット
	lf.lfOutPrecision		= OUT_DEFAULT_PRECIS;			// 出力精度
	lf.lfClipPrecision		= CLIP_DEFAULT_PRECIS;			// クリッピングの精度
	lf.lfQuality			= PROOF_QUALITY;				// 出力品質
	lf.lfPitchAndFamily		= DEFAULT_PITCH | FF_MODERN;	// ピッチとファミリ
	StringCchCopy(lf.lfFaceName, 32, fontName);	// フォント名

	HFONT hFont = CreateFontIndirect(&lf);
	if (hFont == NULL) {
		DeleteObject(hFont);
		return FALSE;
	}

	iFontSize = fontSize;


	// デバイスに選択したフォントを設定
	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);


	///// 文字テクスチャ作成 //////
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	CONST MAT2 mat ={ { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };


	for (int i=0; i<CHARACTERKIND; i++) {
		DWORD size = 0;
		UINT code = 0x20+i;
		GLYPHMETRICS gm;


		// 文字のグリフビットマップを取得
		if ((size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &gm, 0, NULL, &mat)) == GDI_ERROR) {
			SelectObject(hdc, oldFont);			// 元のフォントに戻す
			ReleaseDC(NULL, hdc);
			DeleteObject(hFont);
			Clear();
			return FALSE;
		}
		BYTE *pMono = new BYTE[size];
		GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &gm, size, pMono, &mat);


		// 文字情報を保存
		mCharData[i].iWidth			= (gm.gmBlackBoxX + 3) / 4 * 4;
		mCharData[i].iHeight		= gm.gmBlackBoxY;
		mCharData[i].iAreaWidth		= gm.gmCellIncX;
		mCharData[i].iAreaHeight	= tm.tmHeight;
		mCharData[i].iOriginX		= gm.gmptGlyphOrigin.x;
		mCharData[i].iOriginY		= gm.gmptGlyphOrigin.y-tm.tmAscent;		// 左上原点


		// テクスチャ生成
		lpDev->CreateTexture(mCharData[i].iWidth, mCharData[i].iHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &lpFontTex[i], NULL);

		// テクスチャにフォントビットマップを書き込み
		D3DLOCKED_RECT lockedRect;
		lpFontTex[i]->LockRect(0, &lockedRect, NULL, 0);  // ロック
		DWORD *texBuf = (DWORD*)lockedRect.pBits;   // テクスチャメモリへのポインタ

		for (int y = 0; y < mCharData[i].iHeight; y++) {
			for (int x = 0; x < mCharData[i].iWidth; x++) {
				DWORD alpha;
				if (code == ' ')	// スペースの場合は何も描画しない
					alpha = 0;
				else
					alpha = pMono[y * mCharData[i].iWidth + x] * 255 / 16;	// 16階調を256階調に変換

				texBuf[y * mCharData[i].iWidth + x] = (alpha << 24) & 0xff000000;
			}
		}

		lpFontTex[i]->UnlockRect(0);  // アンロック


		delete[] pMono;
	}

	SelectObject(hdc, oldFont);			// 元のフォントに戻す
	ReleaseDC(NULL, hdc);
	DeleteObject(hFont);				// 今回のフォントを消去

	return TRUE;
}


BOOL DXTextANSI::Draw(int x, int y, int fontSize, int charInterval, DWORD color, const char * s, ...) {
	BOOL result;
	RECT rect ={ x, y, 0, 0 };

	va_list list;
	va_start(list, s);
	result = DrawTEXT(&rect, fontSize, charInterval, TEXTALIGN_NONE|TEXTSCALE_NONE, color, s, list);
	va_end(list);

	return result;
}


BOOL DXTextANSI::DrawInRect(RECT * rect, int fontSize, int charInterval, DWORD format, DWORD color, const char * s, ...) {
	BOOL result;

	va_list list;
	va_start(list, s);
	result = DrawTEXT(rect, fontSize, charInterval, format, color, s, list);
	va_end(list);

	return result;
}


BOOL DXTextANSI::Clear() {

	ZeroMemory(mCharData, sizeof(mCharData));
	for (int i=0; i<CHARACTERKIND; i++) {
		SAFE_RELEASE(lpFontTex[i]);
	}

	return TRUE;
}


DWORD DXTextANSI::ConvertFromRGBA(int red, int green, int blue, int alpha) {
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




int DXTextANSI::CalcTextPosition(RECT * rect, float inScale, int charInterval, DWORD format, const char * s, POINT * pt, float *outScale) {
	if (!pt)
		return FALSE;
	if (!s)
		return FALSE;
	if (!rect)
		return FALSE;
	if (!outScale)
		return FALSE;

	// フォーマットの組み合わせをチェック
	if ((format&0x0F) == TEXTALIGN_NONE) {
		if ((format&0xF0) == TEXTSCALE_AUTOX || (format&0xF0) == TEXTSCALE_AUTOXY)
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


	while (lineHead < strlen(s)) {
		float lineLen = 0.0f;
		float lineHeight = mCharData[0].iAreaHeight*inScale;

		// 縦方向にはみ出したら
		if (offsetY + lineHeight > rectY) {
			if ((format&0x0F) != TEXTALIGN_NONE && (format&0xF0) != TEXTSCALE_AUTOXY) {
				break;
			}
		}


		//// 行の長さの算出 ////
		bool canPut = false;
		int i = -1;
		while ((lineHead+ ++i) < strlen(s)) {

			// 文字コード取得
			UINT code = (UINT)s[lineHead +i];

			// 改行なら行の終わりとする
			if (code == (UINT)'\n') {
				i++;
				canPut = true;
				break;
			}

			// 横方向にはみ出したら
			if (lineLen + mCharData[code-0x20].iAreaWidth*inScale > rectX) {
				if ((format&0x0F) != TEXTALIGN_NONE && (format&0xF0) == TEXTSCALE_NONE) {
					break;
				}
			}

			lineLen += mCharData[code-0x20].iAreaWidth*inScale + charInterval;
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
			// 文字コード取得
			UINT code = (UINT)s[charCnt];

			if (code == (UINT)'\n')
				continue;

			pt[charCnt].x = rect->left + (int)offsetX;
			pt[charCnt].y = rect->top + (int)offsetY;

			offsetX += (mCharData[code-0x20].iAreaWidth*inScale + charInterval)*(*outScale);
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



int DXTextANSI::DrawTEXT(RECT * rect, int fontSize, int charInterval, DWORD format, DWORD color, const char * s, va_list arg) {
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
	if (!lpFontTex[0])
		return FALSE;


	// 可変長引数リストに従って通常の文字列に変換
	int length;
	char tmp[MAXCHARACTER+1];
	vsnprintf(tmp, MAXCHARACTER+1, s, arg);

	// 文字列の最適化
	char str[MAXCHARACTER+1];
	length = OptimizeString(str, tmp);


	// 描画位置の算出
	POINT *pt = NULL;
	pt = (POINT*)malloc(sizeof(POINT)*(length));
	if (!pt)
		return FALSE;

	float tmpScale = (float)fontSize/iFontSize;
	float scale;
	int charCnt = CalcTextPosition(rect, tmpScale, charInterval, format, str, pt, &scale);
	if (charCnt < 0)
		return FALSE;


	// 描画倍率設定
	float scaleX, scaleY;
	if ((format&0xF0) == TEXTSCALE_AUTOX)
		scaleX = tmpScale*scale, scaleY = tmpScale;
	else if ((format&0xF0) == TEXTSCALE_AUTOXY)
		scaleX = tmpScale*scale, scaleY = tmpScale*scale;
	else
		scaleX = tmpScale, scaleY = tmpScale;


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
	lpEffect->BeginPass(0);

	for (int i=0; i<charCnt; i++) {

		// 描画する文字の文字コード取得
		UINT code = (UINT)str[i];

		if (code == (UINT)'\n')
			continue;

		if (lpFontTex[code-0x20] == NULL)
			continue;	// テクスチャが存在しないならスキップ

		// スクリーンの左上隅を原点とした座標に変換
		pt[i].x -= iDrawWidth / 2;
		pt[i].y -= iDrawHeight / 2;
		pt[i].y *= -1;


		// ワールドビュー射影変換行列を作成
		D3DXMatrixScaling(&localScale, (float)mCharData[code-0x20].iWidth*scaleX, (float)mCharData[code-0x20].iHeight*scaleY, 1.0f);				// ポリゴンを文字の大きさにする
		D3DXMatrixTranslation(&localOffset, (float)mCharData[code-0x20].iOriginX*scaleX, (float)mCharData[code-0x20].iOriginY*scaleY, 0.0f);		// ポリゴンを文字の原点に移動
		D3DXMATRIX localMat = localScale*localOffset;
		D3DXMatrixTranslation(&worldOffset, (float)pt[i].x -0.5f, (float)pt[i].y +0.5f, 0.0f);
		world = localMat * worldOffset;
		D3DXMATRIX matWorldViewProj = world*ortho;

		// シェーダ定数設定
		lpEffect->SetMatrix("matWorldViewProj", &matWorldViewProj);	// ワールドビュー射影変換行列を設定
		lpEffect->SetFloatArray("color", colorRGBA, 4);				// 色指定
		lpEffect->SetTexture("tex", lpFontTex[code-0x20]);			// テクスチャ指定
		lpEffect->CommitChanges();
		// 描画開始
		
		lpDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);			// 描画
		
		
	}
	// シェーダ終了
	lpEffect->EndPass();
	lpEffect->End();


	free(pt);


	return TRUE;
}

int DXTextANSI::OptimizeString(char * dst, const char * src) {
	if (!src)
		return -1;
	if (!dst)
		return -1;

	int cnt = 0;

	for (int i=0; i<strlen(src); i++) {
		int code = (UINT)src[i];

		if ((0x20 <= code && code <= 0x7E) || code == '\n' || code == '\t')
			dst[cnt++] = src[i];
	}

	dst[cnt] = '\0';	// NULL文字

	return cnt;
}
