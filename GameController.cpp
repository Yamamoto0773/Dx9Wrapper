#include "GameController.hpp"

using namespace std;

GameController::GameController() {

}


GameController::~GameController() {
	Clear();
}


bool GameController::Init(HWND hWnd, HINSTANCE hInstance) {
	// DirectInput生成
	if (!di.Create(hWnd, hInstance)) {
		return false;
	}

	// キーボードを使う
	if (!di.CreateKeyboard()) {
		return false;
	}

	// MIDIキーボード入力の準備
	if (!mi.Create()) {
		return false;
	}

	// MIDIキーボード入力スタート
	if (!mi.StartInput()) {
		return false;
	}

	return true;
}


bool GameController::Update() {

	// キーボード入力
	unsigned char keyboard[256];
	BOOL IsValidKeybd = di.GetKeyboard(keyboard);

	if (!IsValidKeybd) {
		return false;
	}

	// midiキーボード入力
	MIDIKEYSTATE midiKey[MIDIKEYCNT];
	BOOL IsValidMidi = mi.GetKeyState(midiKey);


	if (IsValidMidi) {
		// midiキーボードを使う

		for (int i=0; i<KEYCOUNT; i++) {
			switch (midiKey[i]) {
				case MIDIKEYSTATE_PRESSED:
					bIsKeyPressed[i] = true;
					bIsKeyOn[i] = true;
					break;
				case MIDIKEYSTATE_RELEASED:
					bIsKeyOn[i] = false;
					break;
			}
		}

	}
	else {
		// キーボードを使う

		static const int KEYID[63] ={		// キーのリスト
			DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9, DIK_0, DIK_MINUS, DIK_CIRCUMFLEX, DIK_YEN, DIK_BACKSPACE, DIK_NUMLOCK, DIK_DIVIDE, DIK_MULTIPLY,
			DIK_Q, DIK_W, DIK_E, DIK_R, DIK_T, DIK_Y, DIK_U, DIK_I, DIK_O, DIK_P, DIK_AT, DIK_LBRACKET, DIK_RETURN, DIK_NUMPAD7, DIK_NUMPAD8, DIK_NUMPAD9,
			DIK_A, DIK_S, DIK_D, DIK_F, DIK_G, DIK_H, DIK_J, DIK_K, DIK_L, DIK_SEMICOLON, DIK_COLON, DIK_RBRACKET, DIK_NUMPAD4, DIK_NUMPAD5, DIK_NUMPAD6,
			DIK_Z, DIK_X, DIK_C, DIK_V, DIK_B, DIK_N, DIK_M, DIK_COMMA, DIK_PERIOD, DIK_SLASH, DIK_BACKSLASH, DIK_RSHIFT, DIK_NUMPAD1, DIK_NUMPAD2, DIK_NUMPAD3,
		};

		bool on[KEYCOUNT];
		for (int i=0; i<KEYCOUNT; i++) {
			on[i] = bIsKeyOn[i];	// バックアップ
		}

		ZeroMemory(bIsKeyOn, sizeof(bIsKeyOn));

		for (int i=0; i<63; i++) {
			double x;
			if (!GetKeyPosition(KEYID[i], 0.0, 0.99, &x)) continue;

			int virKeyNum = (int)(x*KEYCOUNT);		// 仮想キーの位置に変換

			if (keyboard[KEYID[i]]&0x80) {
				bIsKeyOn[virKeyNum] = true;

				if (!on[virKeyNum]) {
					// まだ押されていなければ押された瞬間とする
					bIsKeyPressed[virKeyNum] = true;
				}
			}
		}

	}


	return true;
}


bool GameController::AddCustomKey(string name, int keyID) {
	if (keyID < 0 || keyID >= KEYCOUNT) return false;

	int index = -1;
	for (int i=0; i<customKey.size(); i++) {
		if (name == customKey[i].name) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		CUSTOMKEY add;
		add.name = name;
		add.key.push_back(keyID);

		customKey.push_back(add);
	}
	else {
		customKey[index].key.push_back(keyID);
	}
	
	return true;
}


bool GameController::DeleteCustomKey(std::string name) {
	bool res = false;

	for (auto it = customKey.cbegin(); it != customKey.cend(); ++it) {
		if (it->name == name) {
			customKey.erase(it);
			res = true;
			break;
		}
	}

	return res;
}


bool GameController::IsKeyOn(std::string name) const {
	auto access = customKey.cend();

	for (auto it = customKey.cbegin(); it != customKey.end(); ++it) {
		if (name == it->name) {
			access = it;
			break;
		}
	}

	if (access == customKey.cend()) {
		// 指定した名前のカスタムキーがなかった
		return false;
	}
	else {

		for (int i=0; i<access->key.size(); i++) {
			if (IsKeyOn(access->key[i])) {
				return true;
			}
		}
		return false;
	}

}


bool GameController::IsKeyPressed(std::string name) const {
	auto access = customKey.cend();

	for (auto it = customKey.cbegin(); it != customKey.end(); ++it) {
		if (name == it->name) {
			access = it;
			break;
		}
	}

	if (access == customKey.cend()) {
		// 指定した名前のカスタムキーがなかった
		return false;
	}
	else {

		for (int i=0; i<access->key.size(); i++) {
			if (IsKeyPressed(access->key[i])) {
				return true;
			}
		}
		return false;
	}

}


void GameController::GetKeyState(bool* hold, bool* pressed) {
	memcpy(hold, bIsKeyOn, sizeof(bool)*KEYCOUNT);
	memcpy(pressed, bIsKeyPressed, sizeof(bool)*KEYCOUNT);
}


const bool GameController::IsKeyOn(int num) const {
	if (num < 0 || num >= KEYCOUNT)	return false;

	return bIsKeyOn[num];
}


const bool GameController::IsKeyPressed(int num) const {
	if (num < 0 || num >= KEYCOUNT)	return false;

	return bIsKeyPressed[num];
}


bool GameController::Clear() {

	ZeroMemory(bIsKeyOn, sizeof(bIsKeyOn));
	ZeroMemory(bIsKeyPressed, sizeof(bIsKeyPressed));

	if (!di.Delete())	return false;
	if (!mi.Clear())	return false;

	return true;
}



bool GameController::GetKeyPosition( int DIKcode , float min, float max, double *posX, double *posY) {

	switch (DIKcode) {
		case DIK_1:
			if (posX) *posX = 0.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_Q:
			if (posX) *posX = 1.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_A:
			if (posX) *posX = 1.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_2:
			if (posX) *posX = 2.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_Z:
			if (posX) *posX = 2.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_W:
			if (posX) *posX = 3.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_S:
			if (posX) *posX = 3.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_3:
			if (posX) *posX = 4.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_X:
			if (posX) *posX = 4.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_E:
			if (posX) *posX = 5.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_D:
			if (posX) *posX = 5.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_4:
			if (posX) *posX = 6.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_C:
			if (posX) *posX = 6.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_R:
			if (posX) *posX = 7.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_F:
			if (posX) *posX = 7.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_5:
			if (posX) *posX = 8.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_V:
			if (posX) *posX = 8.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_T:
			if (posX) *posX = 9.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_G:
			if (posX) *posX = 9.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_6:
			if (posX) *posX = 10.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_B:
			if (posX) *posX = 10.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_Y:
			if (posX) *posX = 11.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_H:
			if (posX) *posX = 11.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_7:
			if (posX) *posX = 12.0;
			if (posY) *posY = 0.0;
		case DIK_N:
			if (posX) *posX = 12.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_U:
			if (posX) *posX = 13.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_J:
			if (posX) *posX = 13.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_8:
			if (posX) *posX = 14.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_M:
			if (posX) *posX = 14.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_I:
			if (posX) *posX = 15.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_K:
			if (posX) *posX = 15.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_9:
			if (posX) *posX = 16.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_COMMA:
			if (posX) *posX = 16.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_O:
			if (posX) *posX = 17.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_L:
			if (posX) *posX = 17.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_0:
			if (posX) *posX = 18.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_PERIOD:
			if (posX) *posX = 18.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_P:
			if (posX) *posX = 19.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_SEMICOLON:
			if (posX) *posX = 19.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_MINUS:
			if (posX) *posX = 20.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_SLASH:
			if (posX) *posX = 20.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_AT:
			if (posX) *posX = 21.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_COLON:
			if (posX) *posX = 21.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_CIRCUMFLEX:
			if (posX) *posX = 22.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_BACKSLASH:
			if (posX) *posX = 22.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_LBRACKET:
			if (posX) *posX = 23.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_RBRACKET:
			if (posX) *posX = 23.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_YEN:
			if (posX) *posX = 24.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_BACKSPACE:
			if (posX) *posX = 25.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_RETURN:
			if (posX) *posX = 25.0;
			if (posY) *posY = 1.5;
			break;
		case DIK_RSHIFT:
			if (posX) *posX = 25.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_NUMLOCK:
			if (posX) *posX = 26.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_NUMPAD7:
			if (posX) *posX = 26.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_NUMPAD4:
			if (posX) *posX = 26.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_NUMPAD1:
			if (posX) *posX = 26.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_DIVIDE:
			if (posX) *posX = 27.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_NUMPAD8:
			if (posX) *posX = 27.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_NUMPAD5:
			if (posX) *posX = 27.0;
			if (posY) *posY = 2.0;
			break;
		case DIK_NUMPAD2:
			if (posX) *posX = 27.0;
			if (posY) *posY = 3.0;
			break;
		case DIK_MULTIPLY:
			if (posX) *posX = 28.0;
			if (posY) *posY = 0.0;
			break;
		case DIK_NUMPAD9:
			if (posX) *posX = 28.0;
			if (posY) *posY = 1.0;
			break;
		case DIK_NUMPAD6:
			if (posX) *posX = 28.0;
			if (posY) *posY = 2.0;
		case DIK_NUMPAD3:
			if (posX) *posX = 28.0;
			if (posY) *posY = 3.0;
			break;
		default:
			if (posX) *posX = -1.0;
			if (posY) *posY = -1.0;
			return false;
	}

	if (posX) (*posX) = min + (*posX)/28.0*(max-min);
	if (posY) (*posY) = min + (*posY)/3.0*(max-min);

	return true;
}