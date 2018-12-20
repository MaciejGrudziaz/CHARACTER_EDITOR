#pragma once
#define _CRT_SECURE_NO_WARNINGS
//#define GLEW_STATIC
#include <gl\glew.h>
#include <Windows.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

class WinAPIwindow {
	std::string windowName;
	char* className;

	WNDCLASSEX windowClass;
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	HINSTANCE hInstance;

	double fov;
	bool fullscreen;
	unsigned screenWidth;
	unsigned screenHeight;

	glm::mat4 projection;

	unsigned posX, posY;

	void InitializeWindowClassEx();							//iniclaizacja klasy okna WinAPI
	DWORD GetWindowStyle();									//okreœlenie podstawowych sty³ów okna WinAPI w zale¿noœci od trybu wyœwietlania [pe³noekranowy/okienkowy]
	DWORD GetWindowExStyle();								//okreœlenie dodatkowych stylów okna WinAPI w zale¿noœci od trybu wyœwietlania [pe³noekranowy/okienkowy]
	int Create();											//stworzenie okna WinAPI w zale¿noœci od trybu wyœwietlania [pe³noekranowy/okienkowy]
	void SetupFullscreenSettings();							//ustawianie w³aœciwoœci okna dla trybu pe³noekranowego
	void SetupPixelFormat();								//funkcja okreœlaj¹ca foramt pikseli dla OpenGL

public:

	WinAPIwindow(std::string name_, unsigned screenWidth_, unsigned screenHeight_, unsigned posX_=0,unsigned posY_=0, double fov_ = 90.0, bool fullscreen_ = false);

	int SetupWindowSettings();								//stworzenie ca³ego okna WinAPI [klasa okna, uchwyt, kontekst urz¹dzenia]
	int CreateNewWindow();									//stworzenie nowego okna [stworzenie samego nowego uchwytu i kontekstu urz¹dzenia bez nowej klasy]
	int CreateGraphicContext(HWND hwnd_);					//utworzenie kontekstów grafiki dla okna WinAPI
	void ReleaseGraphicContext();							//funkcja s³u¿¹ca do zwolnienia kontekstu grafiki OpenGL dla kontekstu urz¹dzenia WinAPI
	void DeleteGraphicsContext();
	void ShowWinAPIwindow();								//funkcja wyœwitalj¹ca okno apliakcji WinAPI
	int SetupOpenGLScreenSettings();						

	void UpdateProjectionMatrix();

	std::string GetName() const { return windowName; }
	double GetFOV()const { return fov; }
	bool GetFullScreenMode()const { return fullscreen; }
	int GetScreenWidth()const { return screenWidth; }
	int GetScreenHeight()const { return screenHeight; }
	std::pair<int, int> GetScreenSize()const { return std::pair<int, int>(screenWidth, screenHeight); }
	glm::mat4 GetProjectionMatrix()const { return projection; }
	glm::mat4* GetProjectionMatrixPtr() { return &projection; }

	void SetScreenWidth(unsigned width_) { screenWidth = width_; }
	void SetScreenHeight(unsigned height_) { screenHeight = height_; }
	void SetScreenSize(unsigned width_, unsigned height_) { screenWidth = width_; screenHeight = height_; }

	HWND GetHWND()const { return hwnd; }
	WNDCLASSEX GetWindowClass() const { return windowClass; }
	HDC GetHDC()const { return hDC; }
	HGLRC GetHGLRC() const { return hRC; }
	HINSTANCE GetHINSTANCE()const { return hInstance; }

	void LoadInstanceHandle(HINSTANCE hInstance_) { hInstance = hInstance_; }

	void Cursor(bool show);

	~WinAPIwindow();
};