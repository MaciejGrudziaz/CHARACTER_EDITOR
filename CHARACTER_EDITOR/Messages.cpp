#include "WinAPIwindowManager.h"

//----------------------CALLBACK----------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		WinAPIwindow* window=(WinAPIwindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		window->CreateGraphicContext(hwnd);
		window->ShowWinAPIwindow();
		return 0;
	}
	case WM_CLOSE: {
		WindowsMap* winMaps = WinAPIwindowManager::GetAllWindows();
		for (WindowsMapIt it = winMaps->begin(); it != winMaps->end(); ++it) {
			if ((*it).second->GetHWND() == hwnd) {
				if (winMaps->size() == 1 || hwnd == WinAPIwindowManager::GetMainWindow()->GetHWND()) {
					PostQuitMessage(0);
					(*it).second->ReleaseGraphicContext();
				} else (*it).second->DeleteGraphicsContext();
				DestroyWindow((*it).second->GetHWND());
				winMaps->erase(it);
				break;
			}
		}
		return 0;
	}
	case WM_SIZE: {
		WindowsMap* winMaps = WinAPIwindowManager::GetAllWindows();
		for (WindowsMapIt it = winMaps->begin(); it != winMaps->end(); ++it) {
			if ((*it).second->GetHWND() == hwnd) {
				(*it).second->SetScreenSize(LOWORD(lParam), HIWORD(lParam));
				//(*it).second->SetupOpenGLScreenSettings();					//zwolnienie kontekstu grafiki aplikacji przez WinAPI
				(*it).second->UpdateProjectionMatrix();
				break;
			}
		}
		return 0;
	}
	case WM_KEYDOWN: {
		switch (wParam) {
		case VK_ESCAPE: {
			WindowsMap* map = WinAPIwindowManager::GetAllWindows();
			for (WindowsMap::iterator it = map->begin(); it != map->end(); ++it)
				DestroyWindow((*it).second->GetHWND());
			PostQuitMessage(0);
			break;
		}
		case 0x57: { //W
			InputManager::SetKeydown(0x57);
			break;
		}
		case 0x53: { //S
			InputManager::SetKeydown(0x53);
			break;
		}
		case 0x41: { //A
			InputManager::SetKeydown(0x41);
			break;
		}
		case 0x44: { //D
			InputManager::SetKeydown(0x44);
			break;
		}
		case VK_RIGHT: {
			InputManager::SetKeydown(VK_RIGHT);
			break;
		}
		case VK_LEFT: {
			InputManager::SetKeydown(VK_LEFT);
			break;
		}
		case VK_SPACE: {
			InputManager::SetKeydown(VK_SPACE);
			break;
		}
		case VK_CONTROL: {
			InputManager::SetKeydown(VK_CONTROL);
			break;
		}
		default:
			break;
		}
		return 0;
	}
	case WM_KEYUP: {
		switch (wParam) {
		case 0x57: { //W
			InputManager::ResetKeydown(0x57);
			break;
		}
		case 0x53: { //S
			InputManager::ResetKeydown(0x53);
			break;
		}
		case 0x41: { //A
			InputManager::ResetKeydown(0x41);
			break;
		}
		case 0x44: { //D
			InputManager::ResetKeydown(0x44);
			break;
		}
		case VK_RIGHT: {
			InputManager::ResetKeydown(VK_RIGHT);
			break;
		}
		case VK_LEFT: {
			InputManager::ResetKeydown(VK_LEFT);
			break;
		}
		case VK_SPACE: {
			InputManager::ResetKeydown(VK_SPACE);
			break;
		}
		case VK_CONTROL: {
			InputManager::ResetKeydown(VK_CONTROL);
			break;
		}
		default:
			break;
		}
		return 0;
	}

	default:
		break;
	}

	return (DefWindowProc(hwnd, message, wParam, lParam));
}