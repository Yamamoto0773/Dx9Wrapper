#pragma once

//////////////////////////////////////////////////////////////
// DirectX9.0を用いて図形を描画するクラスです
// 
// (c) 2017, Yamamoto Nanami
//////////////////////////////////////////////////////////////

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <d3dx9.h>
#include <d3d9.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdarg.h>
#include <stdio.h>


typedef struct _Point {
	double x, y;
}Point;


typedef struct _D3DVERTEX {
	float x, y, z;	// 頂点座標
	float rhw;		// 除算数
	DWORD color;	// 頂点の色
}D3DVERTEX;

#define D3DFVF_2DVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
#define ELLIPSEVERTEX 30
#define MAX_VERTEX 100



class DirectXFigure {
private:
	LPDIRECT3DDEVICE9		lpDev;
	IDirect3DVertexBuffer9 *pVertex;

	ID3DXLine	*pLine;

	BOOL		bStroke;			// 輪郭線を描画するか
	BOOL		bFill;				// 塗りつぶしをするか
	BOOL		bAntialias;			// アンチエイリアスをかけるか

	float		fStrokeWeight;		// 線の太さ

	D3DCOLOR	dwStrokeColor;		// 線の色
	D3DCOLOR	dwFillColor;		// 塗りつぶしの色
	

public:
	DirectXFigure();
	~DirectXFigure();

	
	BOOL Init(LPDIRECT3DDEVICE9 lpDev);		// 初期化


	BOOL stroke(int red, int green, int blue, int alpha = 255);
	BOOL noStroke();
	BOOL fill(int red, int green, int blue, int alpha = 255);
	BOOL noFill();
	BOOL strokeWidth(FLOAT width);
	BOOL smooth();
	BOOL noSmooth();

	BOOL line(float x1, float y1, float x2, float y2);
	BOOL rect(float x, float y, float w, float h);
	BOOL ellipse(float x, float y, float w, float h, int vertexCnt=ELLIPSEVERTEX);
	BOOL quad(size_t vertexCnt, ...);

private:
	BOOL GetPolyLineVertex(Point *pPolyLineVerList, Point *pPolyVerList, size_t polyVerCnt);
	Point GetTriangleCenter(Point pt1, Point pt2, Point pt3);
	Point GetPolygonCenter(int count, Point *ppt);
	double GetTriangleArea(Point pt1, Point pt2, Point pt3);
	double GetVec2Length(D3DXVECTOR2 *v);
	BOOL LineSegCross(Point *a1, Point *a2, Point *b1, Point *b2);
	BOOL CheckInsidePolyArea(Point *pPolyVerList, size_t polyVerCnt, Point *judgePt);

};
