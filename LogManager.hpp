#pragma once

// 2017/12/16
// (c) 2017, NanamiYamamoto

////////////////////////////////////
// ログの書き込みを行うクラス
////////////////////////////////////


#include <string>
#include <stdarg.h>
#include <sstream>
#include <Windows.h>
#include <iomanip>
#include <fstream>
#include <ios>


class LogManager {
protected:
	virtual void writeLog(bool isTimeStamp, bool isLF, const char *msg, va_list list) = 0;

	LogManager();
	virtual ~LogManager();

public:
	// ログをクリア
	virtual bool clear() = 0;

	// ログに書き込み
	virtual void write(const char *msg, ...) final;
	// 改行つきでログに書き込み
	virtual void lnwrite(const char *msg, ...) final;
	// タイムスタンプつきでログに書き込み
	virtual void twrite(const char *msg, ...) final;
	// 改行あり,タイムスタンプつきでログに書き込み
	virtual void tlnwrite(const char *msg, ...) final;

	void getTimeStamp(std::string &str);

};




class LogBuffer : public LogManager {
private:
	std::string logBuffer;
	
	void writeLog(bool isTimeStamp, bool isLF, const char *msg, va_list list) override;

public:
	LogBuffer();
	~LogBuffer();

	// ログをクリア
	bool clear() override;

	// ログ取得
	const char* get() const;

};



class LogFile : public LogManager {
private:
	std::string name;
	
	void writeLog(bool isTimeStamp, bool isLF, const char *msg, va_list list) override;
	
public:
	LogFile();
	~LogFile();

	bool init(const char *name);

	// ログをクリア
	bool clear() override;


};
