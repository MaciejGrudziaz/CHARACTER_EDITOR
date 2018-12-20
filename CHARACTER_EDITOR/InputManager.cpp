#include "InputManager.h"
bool InputManager::keydown[256] = { false };

void InputManager::ResetAll() {
	for (int i = 0; i < keydownSize; ++i) {
		keydown[i] = false;
	}
}