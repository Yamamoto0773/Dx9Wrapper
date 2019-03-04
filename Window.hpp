/*

This class manages the window creation, and message handling.

Written by Nnami Yamamoto on 2019-03-01.

*/


#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include "dx9.hpp"
#include "NonCopyable.hpp"

#include <memory>


struct WindowPimpl;


class Window : public Noncopyable {
public:
	Window();
	Window(void * hInstance, const dx9::Size & size, const wchar_t* title, bool isFull);
	~Window();

	bool create(void * hInstance, const dx9::Size & size, const wchar_t* title, bool isFull);

	bool loop();

	const WindowPimpl * const getDetail() const;

	void init();


private:

	std::unique_ptr<WindowPimpl> pimpl;

	
};




#endif // !_WINDOW_HPP_
