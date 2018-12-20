#pragma once

const int keydownSize = 256;

class InputManager {
	InputManager() {}
	InputManager(const InputManager&) {}
	InputManager& operator=(const InputManager&) {}

	static bool keydown[keydownSize];

public:

	static void SetKeydown(unsigned code) { if (code < keydownSize) keydown[code] = true; }
	static void ResetKeydown(unsigned code) { if (code < keydownSize)keydown[code] = false; }

	static bool GetKeyDown(unsigned code) { if (code < keydownSize) return keydown[code]; else return false; }
	static bool GetKeyUp(unsigned code) { if (code < keydownSize) return keydown[code]; else return false; }

	static void ResetAll();
};