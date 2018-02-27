#include "HighPrecisionTimer.hpp"

HighPrecisionTimer::HighPrecisionTimer() {
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


HighPrecisionTimer::~HighPrecisionTimer() {
}


void HighPrecisionTimer::Start() {

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


void HighPrecisionTimer::Pause() {

	if (bIsPaused) {
		/*
		既に一時停止中の場合は何もしません
		*/
	}
	else {

		// 一時停止開始時のカウントを取得
		llPauseStCount = _GetNowCount();

		// 一時停止時のタイムを保存
		dTime = (double)(llPauseStCount - llStartCount)/llFrequency;

		bIsPaused = true;
	}

}


void HighPrecisionTimer::Reset() {

	if (bIsPaused) {
		dTime = 0.0;
		bIsReset = true;
	}
	else {
		/*
		一時停止中でないと動作しません
		*/
	}
}


double HighPrecisionTimer::GetTime() {

	if (bIsPaused == false) {

		// タイムを更新
		dTime = (double)(_GetNowCount() - llStartCount)/llFrequency;
	}

	return dTime;
}




long long HighPrecisionTimer::_GetNowCount() {
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
