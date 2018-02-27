#pragma once

// 2017/12/15
// (c) 2017, NanamiYamamoto

////////////////////////////////////////////////////
// DirectX9.0を用いた描画を管理するクラス
// 参考:
// http://www.charatsoft.com/develop/otogema/
// http://marupeke296.com/DXG_Init.html
// http://marupeke296.com/DXGSmp_No1_Init.html
// http://sylphylunar.seesaa.net/article/390331341.html
////////////////////////////////////////////////////



#include<d3dx9.h>


#include<memory>
#include<string>
#include<sstream>
#include<iomanip>



// ブレンドモードを指定する時の列挙体
enum class BLENDMODE{
	// 通常合成
	// 結果色 = 合成色x合成色.alpha + 背景色x(1-合成色.alpha)
	NORMAL = 0,

	// 加算合成 (光の表現などに用いる)
	// 結果色 = 合成色x合成色.alpha + 背景色x1
	ADD
};


class DXDrawManager {
private:
	std::unique_ptr<IDirect3D9>			d3d9;
	std::unique_ptr<IDirect3DDevice9>	d3ddev9;
	D3DCAPS9							d3dcaps9;

	// DirectXを初期化する時に使う構造体
	// どんな値を設定したか取っておいた方がいいので、メンバ変数とする。
	D3DPRESENT_PARAMETERS				d3dpresent;

	BLENDMODE blendMode;

	bool isRightHand;
	unsigned long backGroundColor;
	
	
	std::string errMsg;
	std::unique_ptr<std::string> logMsg;
	
public:
	// ---------------------------------------
	// インスタンス化用の関数
	// フルスクリーンver.
	static DXDrawManager* CreateFull(
		HWND hwnd
		);

	// インスタンス化用の関数
	// ウィンドウver.
	static DXDrawManager* CreateWind(
		HWND hwnd,
		unsigned int screenW,
		unsigned int screenH
		);



	// ---------------------------------------
	// テクスチャの登録などを行う関数


	


	// ---------------------------------------
	// 描画の設定を行う関数

	// 座標の指定方法の切替　右手系/左手系
	void SetHand(bool isRight);

	// 描画をクリアする時の色(背景色)の指定 (各色成分は0-255の範囲で指定)
	void SetBackGround(unsigned	int r, unsigned int g, unsigned int b);
	// 描画をクリアする時の色(背景色)の指定 (rgbを24bitで指定)
	void SetBackGround(unsigned long rgb);

	// 描画時のテクスチャの補完方法の指定 (ジャギーがかかる場合はこの関数で設定して下さい)
	// https://msdn.microsoft.com/ja-jp/library/cc324337.aspx
	void SetTextureFilter(D3DTEXTUREFILTERTYPE type);

	// ブレンドモードの設定
	void SetBlendMode(BLENDMODE mode);


	
	// ---------------------------------------
	// メッセージのゲッター＆セッター
	// 最新のエラーメッセージを取得
	const char* GetErrMsg() const;
	
	// メッセージのゲッター＆セッター
	// ログメッセージを書き込む文字配列の指定
	void SetLogMsgBuffer(std::string* ptr);






	// ---------------------------------------
	// Direct3D9オブジェクトの生ポインタ取得
	const IDirect3D9* _GetDirect3D9() const { return d3d9.get(); }
	// Direct3DDevice9オブジェクトの生ポインタ取得
	const IDirect3DDevice9* _GetDirect3DDev9() const { return d3ddev9.get(); }


private:
	DXDrawManager(const DXDrawManager&) = delete;	// コピー不可
	DXDrawManager& operator=(const DXDrawManager&) = delete; // 代入不可
	DXDrawManager();
	~DXDrawManager();

	bool Create(
		HWND hwnd,
		bool isfull,
		unsigned int w,
		unsigned int h
		);

	void Delete();
	void Clear();

	// エラーメッセージを上書き保存
	void WriteErrMsg(const char *msg,...);
	// ログの書き込み
	void WriteLogMsg(const char *msg,...);

};