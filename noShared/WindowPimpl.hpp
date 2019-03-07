/*

Window class by Pimpl idiom.
This is private header file.

*/


#ifndef _WINDOWPIMPL_HPP_
#define _WINDOWPIMPL_HPP_


#include <Windows.h>
#include <functional>


struct WindowPimpl {
	std::function<LRESULT CALLBACK(HWND, UINT, WPARAM, LPARAM)> windProcedureFunc;
	MSG msg;
	HWND hWnd;
};



#endif // !_WINDOWPIMPL_HPP_
