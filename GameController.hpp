#pragma once


//////////////////////////////////
// 音ゲーのコントローラーに相当するクラスです
// (c) 2018, Yamamoto Nanami
/////////////////////////////////


#include "CDIPro81.h"
#include "MIDIinput.h"
#include <vector>
#include <string>


class GameController {	
	static const int KEYCOUNT = 25;

private:
	MIDIInput mi;
	CDIPro81 di;


	typedef struct _CUSTOMKEY {
		std::string name;
		std::vector<int> key;
	}CUSTOMKEY;
	std::vector<CUSTOMKEY> customKey;
	
	bool bIsKeyOn[KEYCOUNT];
	bool bIsKeyPressed[KEYCOUNT];
	

private:
	// キーの位置を指定した値の範囲で返す
	bool GetKeyPosition(int DIKcode, float min, float max, double *posX=nullptr, double *posY=nullptr);


public:
	GameController();
	virtual ~GameController();

	static const int GetKeyCount() { return KEYCOUNT; };
	
	// クラスの初期化
	bool Init(HWND hnd, HINSTANCE hInstance);

	// 入力状態の更新
	bool Update();

	// 後片付け
	bool Clear();


	/*
	指定した番号の入力キーの状態を取得
	*/

	// 入力状態の取得
	void GetKeyState(bool* hold, bool* pressed);

	// 指定番号の入力デバイス(hold)の状態を取得
	const bool IsKeyOn(int num) const;

	// 指定番号の入力デバイス(pressed)の状態を取得
	const bool IsKeyPressed(int num) const;

	

	/*
	カスタムキー関連
	*/
	
	// カスタムキーを作成
	bool AddCustomKey(std::string name, int keyID);

	// カスタムキーを削除
	bool DeleteCustomKey(std::string name);

	// カスタムキーの状態を取得
	bool IsKeyOn(std::string name) const;
	bool IsKeyPressed(std::string name) const;

};