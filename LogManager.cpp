#include "LogManager.hpp"



LogManager::LogManager() {
}

LogManager::~LogManager() {
}


void LogManager::getTimeStamp(std::string &str) {
	using namespace std;

	char strTime[128];
	SYSTEMTIME time;
	GetLocalTime(&time);
	sprintf_s(strTime, "[%02d/%02d/%02d %02d:%02d:%02d.%03d]",
		time.wYear%100, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	str = strTime;
}


void LogManager::write(const char * msg, ...) {
	va_list list;
	va_start(list, msg);
	writeLog(false, false, msg, list);
	va_end(list);
}

void LogManager::lnwrite(const char * msg, ...) {
	va_list list;
	va_start(list, msg);
	writeLog(false, true, msg, list);
	va_end(list);
}

void LogManager::twrite(const char * msg, ...) {
	va_list list;
	va_start(list, msg);
	writeLog(true, false, msg, list);
	va_end(list);
}

void LogManager::tlnwrite(const char * msg, ...) {
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
	logBuffer.reserve(512);

	tlnwrite("LOG CLEAR");

	return true;
}


void LogBuffer::writeLog(bool isTimeStamp, bool isLF, const char * msg, va_list list) {
	char str[256];

	// 可変長引数を文字列に変換
	vsnprintf(str, 256, msg, list);

	if (isTimeStamp) {
		// タイムスタンプ書き込み
		std::string timeStamp;
		LogManager::getTimeStamp(timeStamp);
		logBuffer += timeStamp;
		logBuffer += ' ';
	}

	logBuffer += str;

	if (isLF) {
		logBuffer += '\n';
	}
}


const char* LogBuffer::get() const {
	return logBuffer.c_str();
}





LogFile::LogFile() {
}

LogFile::~LogFile() {
	name.clear();
}


bool LogFile::init(const char *name) {
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

	tlnwrite("LOG CLEAR");

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

	tlnwrite("LOG CLEAR");

	logFile.close();

	return true;
}


void LogFile::writeLog(bool isTimeStamp, bool isLF, const char * msg, va_list list) {
	char str[256];
	std::ofstream logFile;

	logFile.open(name, std::ios_base::app);

	// 可変長引数を文字列に変換
	vsnprintf(str, 256, msg, list);

	if (isTimeStamp) {
		// タイムスタンプ書き込み
		std::string timeStamp;
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
