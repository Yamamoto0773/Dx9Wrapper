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
	virtual void writeLog(bool isTimeStamp, bool isLF, const wchar_t *msg, va_list list) = 0;

	LogManager();
	virtual ~LogManager();

public:
	// ログをクリア
	virtual bool clear() = 0;

	// ログに書き込み
	virtual void write(const wchar_t *msg, ...) final;
	// 改行つきでログに書き込み
	virtual void lnwrite(const wchar_t *msg, ...) final;
	// タイムスタンプつきでログに書き込み
	virtual void twrite(const wchar_t *msg, ...) final;
	// 改行あり,タイムスタンプつきでログに書き込み
	virtual void tlnwrite(const wchar_t *msg, ...) final;

	void getTimeStamp(std::wstring &str);

};




class LogBuffer : public LogManager {
private:
	std::wstring logBuffer;
	
	void writeLog(bool isTimeStamp, bool isLF, const wchar_t *msg, va_list list) override;

public:
	LogBuffer();
	~LogBuffer();

	// ログをクリア
	bool clear() override;

	// ログ取得
	const wchar_t* get() const;

};



class LogFile : public LogManager {
private:
	std::wstring name;
	
	void writeLog(bool isTimeStamp, bool isLF, const wchar_t *msg, va_list list) override;
	
public:
	LogFile();
	~LogFile();

	bool init(const wchar_t *name);

	// ログをクリア
	bool clear() override;


};
