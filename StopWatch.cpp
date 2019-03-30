#include "Stopwatch.hpp"

Stopwatch::Stopwatch() {
	llStartCount = 0;
	llPauseStCount = 0;
	llFrequency = 0;

	bIsPaused = true;
	bIsReset = true;
	dTime = 0.0;

	// 周波数を取得
	LARGE_INTEGER freq;
	if (QueryPerformanceFrequency(&freq)) {
		bIsHighPrecValid = true;
		llFrequency = freq.QuadPart;
	}
	else {
		/*
		高精度タイマーが使用できない場合、timeGetTime関数で代用します。
		その際、精度を1msで取得するので周波数を1000Hzに設定します。
		*/

		llFrequency = 1000LL;
		bIsHighPrecValid = false;
	}
}


Stopwatch::~Stopwatch() {
}


void Stopwatch::Start() {

	if (bIsPaused) {
		// タイマー開始の時間を変更
		if (bIsReset) {
			llStartCount = _GetNowCount();
		}
		else {
			// 一時停止した時間分だけずらす
			llStartCount += _GetNowCount() - llPauseStCount;
		}

		bIsPaused = false;
		bIsReset = false;
	}

}


double Stopwatch::Pause() {

	if (bIsPaused) {
		return dTime;
	}
	else {

		// 一時停止開始時のカウントを取得
		llPauseStCount = _GetNowCount();

		// 一時停止時のタイムを保存
		dTime = (double)(llPauseStCount - llStartCount)/llFrequency;

		bIsPaused = true;

		return dTime;
	}

}


void Stopwatch::Reset() {
	bool _isPaused = bIsPaused;

	dTime = 0.0;
	bIsReset = true;
	bIsPaused = true;

	if (!_isPaused)
		Start();

}


double Stopwatch::GetTime() {

	if (bIsPaused == false) {

		// タイムを更新
		dTime = (double)(_GetNowCount() - llStartCount)/llFrequency;
	}

	return dTime;
}




long long Stopwatch::_GetNowCount() {
	LARGE_INTEGER now;

	// 現在のカウントを取得
	if (bIsHighPrecValid) {
		QueryPerformanceCounter(&now);
	}
	else {
		timeBeginPeriod(1);
		now.QuadPart = (long long)timeGetTime();
		timeEndPeriod(1);
	}

	return now.QuadPart;
}
