#pragma once



class Noncopyable {
protected:
	Noncopyable() {};
	virtual ~Noncopyable() {};

private:
	void operator =(const Noncopyable& src) = delete; // copy is not permitted
	Noncopyable(const Noncopyable& src) = delete;
};