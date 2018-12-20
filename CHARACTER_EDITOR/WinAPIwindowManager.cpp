#include "WinAPIwindowManager.h"

WindowsMap WinAPIwindowManager::windowsMap;
WinAPIwindow* WinAPIwindowManager::mainWindow;
MSG WinAPIwindowManager::msg = { 0 };
HINSTANCE WinAPIwindowManager::hInstance=nullptr;

int WinAPIwindowManager::RegisterWindow(WinAPIwindow* window) {
	if (window != nullptr) {
		std::pair<WindowsMap::iterator, bool> retVal;
		retVal = windowsMap.insert(WindowsMap::value_type(window->GetName(), window));
		window->LoadInstanceHandle(hInstance);
		if (retVal.second == true)
			window->SetupWindowSettings();

		if (window->GetHWND() == nullptr) {
			windowsMap.erase(retVal.first);
			return 0;
		}

		if (windowsMap.size() == 1) {
			mainWindow = window;
			wglMakeCurrent(window->GetHDC(), window->GetHGLRC());
			
			int error=glewInit();
			assert(!error);
		}

		return retVal.second;
	}

	return 0;
}

WinAPIwindow* WinAPIwindowManager::GetWindow(std::string name) {
	WindowsMap::iterator it;
	it = windowsMap.find(name);
	if (it == windowsMap.end())
		return nullptr;
	else return (*it).second;
}

int WinAPIwindowManager::SetCurrentWindowGraphics(std::string windowName) {
	WindowsMap::iterator it = windowsMap.find(windowName);
	if (it != windowsMap.end()) {
		wglMakeCurrent((*it).second->GetHDC(), (*it).second->GetHGLRC());
		glViewport(0, 0, (*it).second->GetScreenWidth(), (*it).second->GetScreenHeight());
		return 1;
	}
	else return 0;
}

int WinAPIwindowManager::SetMainWindow(std::string windowName) {
	WindowsMap::iterator it = windowsMap.find(windowName);
	if (it != windowsMap.end()) {
		mainWindow = (*it).second;
		return 1;
	}
	else return 0;
}

//przetwarzanie komunikatów systemu Windows
void WinAPIwindowManager::MessageProcessing() {
	TranslateMessage(&msg);							//przet³umaczenie wiadomoœci i przes³anie jej do systemu
	DispatchMessage(&msg);
}

void WinAPIwindowManager::SwapBuffers() {
	for (WindowsMap::iterator it = windowsMap.begin(); it != windowsMap.end(); ++it) {
		wglMakeCurrent((*it).second->GetHDC(), (*it).second->GetHGLRC());
		::SwapBuffers((*it).second->GetHDC());
	}
	wglMakeCurrent(mainWindow->GetHDC(), mainWindow->GetHGLRC());
}