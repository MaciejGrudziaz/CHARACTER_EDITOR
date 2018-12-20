#pragma once
#include <vector>
#include <string>
#include <map>
#include <assert.h>
#include <glm/glm.hpp>
#include "WinAPIwindow.h"
#include "CameraManager.h"
#include "ResourcesManager.h"
#include "InputManager.h"

typedef std::map<std::string, WinAPIwindow*> WindowsMap;
typedef WindowsMap::iterator WindowsMapIt;

class WinAPIwindowManager {
	WinAPIwindowManager() {}
	WinAPIwindowManager(const WinAPIwindowManager&) {}
	WinAPIwindowManager& operator=(const WinAPIwindowManager&) {}

	static WindowsMap windowsMap;
	static WinAPIwindow* mainWindow;

	static MSG msg;
	static HINSTANCE hInstance;

public:
	static void LoadInstanceHandle(HINSTANCE hInstance_) { hInstance = hInstance_; }

	static int RegisterWindow(WinAPIwindow* window);

	static WinAPIwindow* GetWindow(std::string name);

	static WindowsMap* GetAllWindows() { return &windowsMap; }

	static int SetCurrentWindowGraphics(std::string windowName);

	static int SetMainWindow(std::string windowName);

	static void MessageProcessing();								//przetwarzanie komunikatów okna WinAPI

	static void SwapBuffers();

	static MSG* GetMSG() { return &msg; }

	static WinAPIwindow* GetMainWindow() { return mainWindow; }
};