#include <boost/thread.hpp>
#include "Graphics.h"
#include "wglext.h"
#include "ConsoleInterface.h"

bool VsyncOff();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInsatnce, LPSTR lpCmdLine, int nShowCmd) {	
	WinAPIwindowManager::LoadInstanceHandle(hInstance);

	Graphics::Init();

	ConsoleInterface::Init();

	boost::thread conIntThread(ConsoleInterface::Process);

	VsyncOff();
	
	bool done = false;
	while (!done) {
		PeekMessage(WinAPIwindowManager::GetMSG(), NULL, 0,0,PM_REMOVE);

		if (WinAPIwindowManager::GetMSG()->message == WM_QUIT)
			done = true;
		else {
			WinAPIwindowManager::MessageProcessing();

			Graphics::Process();

			WinAPIwindowManager::SwapBuffers();
		}
	}

	Graphics::End();

	return (WinAPIwindowManager::GetMSG()->wParam);
}

bool VsyncOff() {
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	_wglGetExtensionsStringEXT= (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (_wglGetExtensionsStringEXT != nullptr) {
		if (strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != nullptr) {
			PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
			PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
			wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

			if (wglSwapIntervalEXT != nullptr) {
				wglSwapIntervalEXT(0);
				return true;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}