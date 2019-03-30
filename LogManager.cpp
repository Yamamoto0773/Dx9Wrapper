#include "LogManager.hpp"



LogManager::LogManager() {
}

LogManager::~LogManager() {
}


void LogManager::getTimeStamp(std::wstring &str) {
	using namespace std;

	wchar_t strTime[128];
	SYSTEMTIME time;
	GetLocalTime(&time);
	swprintf_s(strTime, L"[%02d/%02d/%02d %02d:%02d:%02d.%03d]",
		time.wYear%100, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	str = strTime;
}


void LogManager::write(const wchar_t * msg, ...) {
	va_list list;
	va_start(list, msg);
	writeLog(false, false, msg, list);
	va_end(list);
}

void LogManager::lnwrite(const wchar_t * msg, ...) {
	va_list list;
	va_start(list, msg);
	writeLog(false, true, msg, list);
	va_end(list);
}

void LogManager::twrite(const wchar_t * msg, ...) {
	va_list list;
	va_start(list, msg);
	writeLog(true, false, msg, list);
	va_end(list);
}

void LogManager::tlnwrite(const wchar_t * msg, ...) {
	va_list list;
	va_start(list, msg);
	writeLog(true, true, msg, list);
	va_end(list);
}






LogBuffer::LogBuffer() {
	clear();
}

LogBuffer::~LogBuffer() {
}


bool LogBuffer::clear() {
	logBuffer.clear();
	logBuffer.reserve(1024);

	tlnwrite(L"LOG CLEAR");

	return true;
}


void LogBuffer::writeLog(bool isTimeStamp, bool isLF, const wchar_t * msg, va_list list) {
	wchar_t str[1024];

	// 可変長引数を文字列に変換
	_vsnwprintf(str, 1024, msg, list);

	if (isTimeStamp) {
		// タイムスタンプ書き込み
		std::wstring timeStamp;
		LogManager::getTimeStamp(timeStamp);
		logBuffer += timeStamp;
		logBuffer += ' ';
	}

	logBuffer += str;

	if (isLF) {
		logBuffer += '\n';
	}
}


const wchar_t* LogBuffer::get() const {
	return logBuffer.c_str();
}





LogFile::LogFile() {
}

LogFile::~LogFile() {
	name.clear();
}


bool LogFile::init(const wchar_t *name) {
	if (name == nullptr) {
		return false;
	}

	std::ofstream logFile;

	logFile.open(name, std::ios_base::trunc);
	if (!logFile) {
		return false;
	}

	// ファイル名を保存
	this->name = name;

	tlnwrite(L"LOG CLEAR");

	logFile.close();

	return true;
}


bool LogFile::clear() {
	if (name.empty()) {
		return true;
	}

	// ファイル開き直し
	std::ofstream logFile;
	logFile.open(name, std::ios_base::trunc);
	if (!logFile) {
		return false;
	}

	tlnwrite(L"LOG CLEAR");

	logFile.close();

	return true;
}


void LogFile::writeLog(bool isTimeStamp, bool isLF, const wchar_t * msg, va_list list) {
	wchar_t str[1024];
	std::wofstream logFile;

	logFile.open(name, std::ios_base::app);

	// 可変長引数を文字列に変換
	_vsnwprintf(str, 1024, msg, list);

	if (isTimeStamp) {
		// タイムスタンプ書き込み
		std::wstring timeStamp;
		getTimeStamp(timeStamp);
		logFile << timeStamp;
		logFile << ' ';
	}

	logFile << str;

	if (isLF) {
		logFile << '\n';
	}

	logFile.close();
}
