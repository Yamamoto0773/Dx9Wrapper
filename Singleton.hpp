#pragma once


// This class is template class which provide "Singleton Pattern".
// reference: https://qiita.com/narumi_/items/b205e59e7fc81695f380


template<class T>
class Singleton {
public:
	static inline T& GetInstance() {
		static T instance;
		return instance;
	}

protected:
	Singleton() {};
	virtual ~Singleton() {};

private:
	Singleton& operator=(const Singleton& obj) = delete;  // copy is not permitted
	Singleton(const Singleton &obj) = delete; 
};

