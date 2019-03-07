#include "Window.hpp"
#include "WindowPimpl.hpp"

#include <Windows.h>
#include <tchar.h>

struct WindowPimpl;


LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam) {
	switch (mes) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE: // [ESCAPE]キーでウインドウを閉じる
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
	}

	return DefWindowProc(hWnd, mes, wParam, lParam);
}


Window::Window() : pimpl(std::make_unique<WindowPimpl>()) {
	pimpl->windProcedureFunc = WndProc;
}


Window::Window(void * hInstance, const dx9::Size & size, const wchar_t * title, bool isFull) : Window() {
	create(hInstance, size, title, isFull);
}


Window::~Window() {}


bool Window::create(void* hInstance, const dx9::Size& size, const wchar_t* title, bool isFull) {
	
	WNDCLASSEX wcex = { 
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, 
		(HINSTANCE)(hInstance), NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, title, NULL
	};

	if (!RegisterClassEx(&wcex))
		return false;

	if (isFull) {
		pimpl->hWnd = CreateWindow(
			title, title, WS_POPUPWINDOW, CW_USEDEFAULT, 0,
			0, 0, NULL, NULL, (HINSTANCE)hInstance, NULL
		);
	} else {
		pimpl->hWnd = CreateWindow(
			title, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
			static_cast<int>(size.w), static_cast<int>(size.h), NULL, NULL, (HINSTANCE)hInstance, NULL
		);
	}

	if (!pimpl->hWnd)
		return false;

	ShowWindow(pimpl->hWnd, SW_SHOW);

	return true;
}


bool Window::loop() {
	while (PeekMessage(&pimpl->msg, NULL, 0, 0, PM_REMOVE)) {
		if (pimpl->msg.message == WM_QUIT)
			return false;

		TranslateMessage(&pimpl->msg);
		DispatchMessage(&pimpl->msg);
	}

	return true;
}


const WindowPimpl* const Window::getDetail() const {
	return pimpl.get();
}
