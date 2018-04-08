#include "DirectXFigure_old.h"


#define SAFE_RELEASE(x) { if(x){ x->Release(); x=NULL; } }
#define SAFE_FREE(x) { if(x){ free(x); x=NULL;}}



DirectXFigure_old::DirectXFigure_old() {
	pLine = NULL;
	lpDev = NULL;
	pVertex = NULL;

	bFill			= TRUE;
	bStroke			= TRUE;
	bAntialias		= TRUE;
	fStrokeWeight	= 1.0f;
	dwStrokeColor	= 0xffffffff;	// 不透明の白
	dwFillColor		= 0xffffffff;

}

DirectXFigure_old::~DirectXFigure_old() {
	SAFE_RELEASE(pVertex);											// 解放
	SAFE_RELEASE(pLine);

	lpDev = NULL;
	pVertex = NULL;
}


BOOL DirectXFigure_old::Init(LPDIRECT3DDEVICE9 dev) {
	if (!dev)
		return FALSE;
	if (MAX_VERTEX < 3)
		return FALSE;
	if (MAX_VERTEX < ELLIPSEVERTEX)
		return FALSE;

	lpDev = dev;

	HRESULT hr;
	hr = lpDev->CreateVertexBuffer(sizeof(D3DVERTEX)*(MAX_VERTEX*2+2), D3DUSAGE_DYNAMIC, D3DFVF_2DVERTEX, D3DPOOL_DEFAULT, &pVertex, NULL);	// 頂点バッファ作成
	if (FAILED(hr))
		return FALSE;

	hr = D3DXCreateLine(lpDev, &pLine);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}


BOOL DirectXFigure_old::stroke(int red, int green, int blue, int alpha) {
	if (!(0 <= red && red <= 255) ||
		!(0 <= green && green <= 255) ||
		!(0 <= blue && blue <= 255) ||
		!(0 <= alpha && alpha <= 255))
		return FALSE;

	dwStrokeColor = D3DCOLOR_ARGB(alpha, red, green, blue);
	bStroke = TRUE;

	return TRUE;
}

BOOL DirectXFigure_old::noStroke() {
	bStroke = FALSE;

	return TRUE;
}

BOOL DirectXFigure_old::fill(int red, int green, int blue, int alpha) {
	if (!(0 <= red && red <= 255) ||
		!(0 <= green && green <= 255) ||
		!(0 <= blue && blue <= 255) ||
		!(0 <= alpha && alpha <= 255))
		return FALSE;

	dwFillColor = D3DCOLOR_ARGB(alpha, red, green, blue);
	bFill = TRUE;

	return TRUE;
}

BOOL DirectXFigure_old::noFill() {
	bFill = FALSE;

	return TRUE;
}

BOOL DirectXFigure_old::strokeWidth(FLOAT width) {
	if (!pLine)
		return FALSE;
	if (width < 0)
		return FALSE;

	fStrokeWeight = width;
	pLine->SetWidth(fStrokeWeight);

	return TRUE;
}

BOOL DirectXFigure_old::smooth() {
	if (!pLine)
		return FALSE;
	pLine->SetAntialias(TRUE);
	bAntialias = TRUE;

	return TRUE;
}

BOOL DirectXFigure_old::noSmooth() {
	if (!pLine)
		return FALSE;
	pLine->SetAntialias(FALSE);
	bAntialias = FALSE;

	return TRUE;
}

BOOL DirectXFigure_old::line(float x1, float y1, float x2, float y2) {
	if (!pLine)
		return FALSE;

	if (bStroke) {
		// 輪郭線の描画がonなら

		// 頂点データ作成 
		D3DXVECTOR2 vec[2] = {
			{x1-0.5f, y1-0.5f},		// ラスタライゼーションルールがあるため頂点位置をずらす
			{x2-0.5f, y2-0.5f}
		};

		// 描画
		pLine->Begin();
		pLine->Draw(vec, 2, dwStrokeColor);
		pLine->End();
	}

	return TRUE;
}

BOOL DirectXFigure_old::rect(float x, float y, float w, float h) {
	if (!lpDev)
		return FALSE;
	if (!pVertex)
		return FALSE;
	if (MAX_VERTEX < 4)
		return FALSE;
	if (w == 0 || h==0)
		return FALSE;

	lpDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 左手系
	lpDev->SetFVF(D3DFVF_2DVERTEX);
	lpDev->SetTexture(0, NULL);											// ポリゴンだけを描画

	x-=0.5, y-=0.5;		// ラスタライゼーションルールのため頂点位置をずらす 
	Point pt[4] = {
		{x, y},
		{x+w, y},
		{x, y+h},
		{x+w, y+h}
	};

	if (bFill) {
		// 塗りつぶしがonなら
		void *pData = NULL;

		D3DVERTEX v[4] = {
			{(float)pt[0].x, (float)pt[0].y, 0.0f, 1.0f, dwFillColor},	// 左上
			{(float)pt[1].x, (float)pt[1].y, 0.0f, 1.0f, dwFillColor},	// 右上
			{(float)pt[2].x, (float)pt[2].y, 0.0f, 1.0f, dwFillColor},	// 左下
			{(float)pt[3].x, (float)pt[3].y, 0.0f, 1.0f, dwFillColor}	// 右下
		};

		if (FAILED(pVertex->Lock(0, sizeof(D3DVERTEX)*4, (void**)&pData, 0)))	// 頂点バッファをロック
			return FALSE;
		memcpy(pData, v, sizeof(D3DVERTEX)*4);							// 頂点バッファに頂点座標を書き込み
		pVertex->Unlock();													// 頂点バッファをアンロック

		lpDev->SetStreamSource(0, pVertex, 0, sizeof(D3DVERTEX));
		lpDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);					// 描画
	}


	if (bStroke) {
		// 輪郭線の描画がonなら
		Point gpt;

		// 重心を求める
		gpt.x = fabs(pt[1].x+pt[0].x)/2.0;
		gpt.y = fabs(pt[2].y+pt[1].y)/2.0;

		for (int i=0; i<4; i++)
			pt[i].x -= gpt.x, pt[i].y -= gpt.y;								//頂点座標の原点をずらす

		Point scale1, scale2;	// 拡大、縮小倍率
		scale1.x = (fabs(pt[0].x)+fStrokeWeight/2)/(fabs(pt[0].x));
		scale1.y = (fabs(pt[0].y)+fStrokeWeight/2)/(fabs(pt[0].y));
		scale2.x = (fabs(pt[0].x)-fStrokeWeight/2)/(fabs(pt[0].x));
		scale2.y = (fabs(pt[0].y)-fStrokeWeight/2)/(fabs(pt[0].y));

		Point temp;
		temp = pt[2], pt[2] = pt[3], pt[3] = temp;							// 頂点定義の順番を変更

		// 輪郭線の座標を格納
		D3DVERTEX v[10];
		for (int i=0; i<8; i++) {
			if (i%2 == 0)	v[i] = {(float)(pt[i/2].x*scale2.x +gpt.x), (float)(pt[i/2].y*scale2.y +gpt.y), 0.0f, 1.0f, dwStrokeColor};
			else			v[i] = {(float)(pt[i/2].x*scale1.x +gpt.x), (float)(pt[i/2].y*scale1.y +gpt.y), 0.0f, 1.0f, dwStrokeColor};
		}
		v[8] = v[0], v[9] = v[1];

		void *pData;
		if (FAILED(pVertex->Lock(0, sizeof(D3DVERTEX)*10, (void**)&pData, 0)))	// 頂点バッファをロック
			return FALSE;
		memcpy(pData, v, sizeof(D3DVERTEX)*10);							// 頂点バッファに頂点座標を書き込み
		pVertex->Unlock();													// 頂点バッファをアンロック

		lpDev->SetStreamSource(0, pVertex, 0, sizeof(D3DVERTEX));
		lpDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 8);
	}

	return TRUE;
}


BOOL DirectXFigure_old::ellipse(float x, float y, float w, float h, int vertexCnt) {
	if (!lpDev)
		return FALSE;
	if (!pVertex)
		return FALSE;
	if (!(2 < vertexCnt && vertexCnt <= MAX_VERTEX))
		return FALSE;
	if (w == 0 || h==0)
		return FALSE;

	lpDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 左手系
	lpDev->SetFVF(D3DFVF_2DVERTEX);
	lpDev->SetTexture(0, NULL);											// ポリゴンだけを描画

	x-=0.5, y-=0.5;		// ラスタライゼーションルールのため頂点位置をずらす 

	D3DVERTEX *v = NULL;
	v = (D3DVERTEX*)malloc(sizeof(D3DVERTEX)*(vertexCnt+2));
	ZeroMemory(v, sizeof(D3DVERTEX)*(vertexCnt+2));
	v[0] = {x, y, 0.0f, 1.0f, dwFillColor};
	double angle;
	int i;
	for (i=0; i<vertexCnt+1; i++) {
		angle = 2*M_PI/vertexCnt *i;
		v[i+1] = v[0];
		v[i+1].x = (float)(x + w*cos(angle));
		v[i+1].y = (float)(y + h*sin(angle));
	}


	if (bFill) {
		// 塗りつぶしがonなら
		void *pData = NULL;
		if (FAILED(pVertex->Lock(0, sizeof(D3DVERTEX)*(vertexCnt+2), (void**)&pData, 0))) {
			SAFE_FREE(v);
			return FALSE;
		}
		memcpy(pData, v, sizeof(D3DVERTEX)*(vertexCnt+2));				// 頂点バッファに頂点座標を書き込み
		pVertex->Unlock();

		lpDev->SetStreamSource(0, pVertex, 0, sizeof(D3DVERTEX));
		lpDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, vertexCnt);			// 描画
	}


	if (bStroke) {
		// 輪郭線の描画がonなら

		D3DVERTEX *pLineV = NULL;
		pLineV = (D3DVERTEX*)malloc(sizeof(D3DVERTEX)*(vertexCnt*2+2));

		Point scale1, scale2;	// 拡大、縮小倍率
		scale1.x = (w+fStrokeWeight/2)/w;
		scale1.y = (h+fStrokeWeight/2)/h;
		scale2.x = (w-fStrokeWeight/2)/w;
		scale2.y = (h-fStrokeWeight/2)/h;

		for (i=0; i<vertexCnt+2; i++)
			v[i].x -= x, v[i].y -= y;	// 円の中心を原点に合わせる

		for (i=0; i<vertexCnt*2+2; i++) {
			if (i%2 == 0)	pLineV[i] = {(float)(v[1+i/2].x*scale2.x +x), (float)(v[1+i/2].y*scale2.y +y), 0.0f, 1.0f, dwStrokeColor};
			else			pLineV[i] = {(float)(v[1+i/2].x*scale1.x +x), (float)(v[1+i/2].y*scale1.y +y), 0.0f, 1.0f, dwStrokeColor};
		}

		void *pData = NULL;
		if (FAILED(pVertex->Lock(0, sizeof(D3DVERTEX)*(vertexCnt*2+2), (void**)&pData, 0))) {	// 頂点バッファをロック
			SAFE_FREE(pLineV);
			return FALSE;
		}
		memcpy(pData, pLineV, sizeof(D3DVERTEX)*(vertexCnt*2+2));							// 頂点バッファに頂点座標を書き込み
		pVertex->Unlock();													// 頂点バッファをアンロック

		lpDev->SetStreamSource(0, pVertex, 0, sizeof(D3DVERTEX));
		lpDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, vertexCnt*2);

		SAFE_FREE(pLineV);
	}


	SAFE_FREE(v);
	return TRUE;
}

BOOL DirectXFigure_old::quad(size_t vertexCnt, ...) {
	if (!lpDev)
		return FALSE;
	if (!pVertex)
		return FALSE;
	if (!(3 < vertexCnt && vertexCnt < MAX_VERTEX))
		return FALSE;

	// -----------頂点データ格納-------------------------
	Point *pPt = NULL;
	pPt = (Point*)malloc(sizeof(Point)*vertexCnt);
	va_list ap;
	va_start(ap, vertexCnt);
	for (int i=0; i<vertexCnt; i++) {
		pPt[i].x = va_arg(ap, int);
		pPt[i].y = va_arg(ap, int);
	}
	va_end(ap);
	//---------------------------------------------------

	lpDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 左手系
	lpDev->SetFVF(D3DFVF_2DVERTEX);
	lpDev->SetTexture(0, NULL);											// ポリゴンだけを描画
	if (bAntialias)
		lpDev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	else
		lpDev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);


	for (int i=0; i<vertexCnt; i++)
		pPt[i].x -= 0.5, pPt[i].y -= 0.5;			// ラスタライゼーションルールのため頂点位置をずらす

	if (bFill) {
		// 塗りつぶしがonなら
		void *pData = NULL;

		// 頂点定義
		D3DVERTEX *v = NULL;
		v = (D3DVERTEX*)malloc(sizeof(D3DVERTEX)*vertexCnt);
		for (int i=0; i<vertexCnt; i++)
			v[i] = {(float)pPt[i].x, (float)pPt[i].y, 0.0f, 1.0f, dwFillColor};

		if (FAILED(pVertex->Lock(0, sizeof(D3DVERTEX)*vertexCnt, (void**)&pData, 0)))	// 頂点バッファをロック
			return FALSE;
		memcpy(pData, v, sizeof(D3DVERTEX)*vertexCnt);							// 頂点バッファに頂点座標を書き込み
		pVertex->Unlock();															// 頂点バッファをアンロック

		lpDev->SetStreamSource(0, pVertex, 0, sizeof(D3DVERTEX));
		lpDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, vertexCnt-2);					// 描画

		SAFE_FREE(v);
	}


	if (bStroke) {
		// 輪郭線の描画がonなら

		// 輪郭線の頂点を求める
		Point *plineV = NULL;
		plineV = (Point*)malloc(sizeof(Point)*(vertexCnt*2+2));
		GetPolyLineVertex(plineV, pPt, vertexCnt);

		// 輪郭線の頂点を格納
		D3DVERTEX *v = NULL;
		v = (D3DVERTEX*)malloc(sizeof(D3DVERTEX)*(vertexCnt*2+2));
		for (int i=0; i<vertexCnt*2+2; i++)
			v[i] = {(float)plineV[i].x, (float)plineV[i].y, 0.0f, 1.0f, dwStrokeColor};
		SAFE_FREE(plineV);

		void *pData;
		if (FAILED(pVertex->Lock(0, sizeof(D3DVERTEX)*(vertexCnt*2+2), (void**)&pData, 0))) {	// 頂点バッファをロック
			SAFE_FREE(v);
			return FALSE;
		}
		memcpy(pData, v, sizeof(D3DVERTEX)*(vertexCnt*2+2));								// 頂点バッファに頂点座標を書き込み
		pVertex->Unlock();																		// 頂点バッファをアンロック

		lpDev->SetStreamSource(0, pVertex, 0, sizeof(D3DVERTEX));
		lpDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, vertexCnt*2);

		SAFE_FREE(v);
	}

	SAFE_FREE(pPt);
	return TRUE;
}


BOOL DirectXFigure_old::GetPolyLineVertex(Point *pPolyLineVerList, Point *pPolyVerList, size_t polyVerCnt) {
	if (!lpDev)
		return FALSE;
	if (!(3 < polyVerCnt && polyVerCnt < MAX_VERTEX))
		return FALSE;

	int i;

	// 多角形の辺をベクトル形式にする
	LPD3DXVECTOR2 polyLineVec = NULL;
	polyLineVec = (LPD3DXVECTOR2)malloc(sizeof(D3DXVECTOR2)*polyVerCnt);
	for (i=1; i<polyVerCnt+1; i++) {
		polyLineVec[i-1].x = pPolyVerList[i%polyVerCnt].x - pPolyVerList[i-1].x;
		polyLineVec[i-1].y = pPolyVerList[i%polyVerCnt].y - pPolyVerList[i-1].y;
	}

	// 多角形の辺の長さを求める
	double *polyLineLen = NULL;
	polyLineLen = (double *)malloc(sizeof(double)*polyVerCnt);
	for (i=0; i<polyVerCnt; i++)
		polyLineLen[i] = GetVec2Length(&polyLineVec[i]);

	// 輪郭線の頂点の座標を求める
	double verDirection;		// 輪郭線の頂点の向き
	D3DXVECTOR2 bisector;		// 角の2等分線のベクトル
	double dist;				// 多角形の頂点と輪郭線の頂点間の距離
	Point pt;
	for (i=1; i<polyVerCnt+1; i++) {
		polyLineVec[i-1]*=-1.0f;	// ベクトルを反対向きに
		verDirection = D3DXVec2Dot(&polyLineVec[i-1], &polyLineVec[i%polyVerCnt])/(polyLineLen[i-1]*polyLineLen[i%polyVerCnt]);
		verDirection = sqrt((verDirection+1.0)/2.0);
		polyLineVec[i-1]*=-1.0f;
		bisector = polyLineVec[i-1]/GetVec2Length(&polyLineVec[i-1]) - polyLineVec[i%polyVerCnt]/GetVec2Length(&polyLineVec[i%polyVerCnt]);
		bisector/= GetVec2Length(&bisector);	// 単位ベクトル化
		dist = (fStrokeWeight/2)/sqrt(1.0-pow(verDirection, 2.0));

		// 角の2等分線のベクトルのどちら側が多角形の内側なのかを判定する
		pt = {(double)(pPolyVerList[i%polyVerCnt].x +bisector.x*0.01), (double)(pPolyVerList[i%polyVerCnt].y +bisector.y*0.01)};
		if (CheckInsidePolyArea(pPolyVerList, polyVerCnt, &pt))
			dist*=-1;

		pPolyLineVerList[(i-1)*2].x = pPolyVerList[i%polyVerCnt].x - bisector.x*dist;
		pPolyLineVerList[(i-1)*2].y = pPolyVerList[i%polyVerCnt].y - bisector.y*dist;
		pPolyLineVerList[(i-1)*2+1].x = pPolyVerList[i%polyVerCnt].x + bisector.x*dist;
		pPolyLineVerList[(i-1)*2+1].y = pPolyVerList[i%polyVerCnt].y + bisector.y*dist;
	}
	pPolyLineVerList[polyVerCnt*2+2-2] = pPolyLineVerList[0];
	pPolyLineVerList[polyVerCnt*2+2-1] = pPolyLineVerList[1];


	SAFE_FREE(polyLineLen);
	SAFE_FREE(polyLineVec);
	return TRUE;
}


Point DirectXFigure_old::GetPolygonCenter(int count, Point *pPolyVerList) {
	int i;
	double s, s1;
	Point pt, gpt;
	double gx = 0.0, gy = 0.0;

	s = 0.0;
	for (i = 2; i < count; i++) {
		s1 = GetTriangleArea(pPolyVerList[0], pPolyVerList[i-1], pPolyVerList[i]);
		pt = GetTriangleCenter(pPolyVerList[0], pPolyVerList[i-1], pPolyVerList[i]);
		gx += s1 * pt.x;
		gy += s1 * pt.y;
		s += s1;
	}

	gpt.x = gx / s;
	gpt.y = gy / s;

	return gpt;
}

double DirectXFigure_old::GetTriangleArea(Point pt1, Point pt2, Point pt3) {
	return  (fabs(pt1.x*(pt2.y - pt3.y) + pt2.x*(pt3.y - pt1.y) + pt3.x*(pt1.y - pt2.y))/2);
}

double DirectXFigure_old::GetVec2Length(D3DXVECTOR2 *v) {
	return (sqrt(v->x * v->x + v->y * v->y));
}

BOOL DirectXFigure_old::LineSegCross(Point * a1, Point * a2, Point * b1, Point * b2) {
	double ta = (b1->x - b2->x) * (a1->y - b1->y) + (b1->y - b2->y) * (b1->x - a1->x);
	double tb = (b1->x - b2->x) * (a2->y - b1->y) + (b1->y - b2->y) * (b1->x - a2->x);
	double tc = (a1->x - a2->x) * (b1->y - a1->y) + (a1->y - a2->y) * (a1->x - b1->x);
	double td = (a1->x - a2->x) * (b2->y - a1->y) + (a1->y - a2->y) * (a1->x - b2->x);

	return tc * td < 0 && ta * tb < 0;	// 交差している場合,true
}

BOOL DirectXFigure_old::CheckInsidePolyArea(Point * pPolyVerList, size_t polyVerCnt, Point * judgePt) {
	int i;

	double maxY = pPolyVerList[0].y;
	for (i=1; i<polyVerCnt; i++) {
		if (maxY < pPolyVerList[i].y) maxY = pPolyVerList[i].y;
	}

	Point pt = {judgePt->x, maxY+1.0};	// 判定点から、垂直下向き方向に探索する

	int cnt = 0;
	for (i=1; i<polyVerCnt+1; i++) {
		if (LineSegCross(&pPolyVerList[i-1], &pPolyVerList[i%polyVerCnt], judgePt, &pt))
			cnt++;
	}

	if (cnt%2 == 1)	return TRUE;	// 点が多角形の内側の場合
	else			return FALSE;	// 点が多角形の外側の場合
}

Point DirectXFigure_old::GetTriangleCenter(Point pt1, Point pt2, Point pt3) {
	Point pt;
	pt.x = (pt1.x + pt2.x + pt3.x) / 3;
	pt.y = (pt1.y + pt2.y + pt3.y) / 3;

	return pt;
}

