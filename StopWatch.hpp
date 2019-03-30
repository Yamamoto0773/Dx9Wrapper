#pragma once

/*
StopWatchクラス

概要:時間を高精度(1ms以下)で測定することができるクラスです。


作成者:山本七海
作成日時:2017/10/29
参考:http://www.charatsoft.com/develop/otogema/
*/


#include <Windows.h>

class Stopwatch {
private:
	long long llFrequency;

	long long llStartCount;
	long long llPauseStCount;

	double dTime;

	bool bIsHighPrecValid;
	bool bIsPaused;
	bool bIsReset;


public:
	Stopwatch();
	~Stopwatch();

	// タイマーをスタート
	void Start();

	// タイマーを一時停止
	double Pause();

	// タイマーをリセット
	void Reset();

	// 時間を取得
	double GetTime();


	// 高精度タイマーが有効かどうかを返す
	bool _IsHighPrecisionValid() { return bIsHighPrecValid; }

	// タイマーの分解能を返す
	long long _GetResolutionFreq() { return llFrequency; }

	// 現在のシステムのカウント値を返す
	long long _GetNowCount();

};