#pragma once
#include <vector>
#include "Camera.h"

class CameraManager {
	CameraManager() {}
	CameraManager(const CameraManager&) {}
	CameraManager& operator=(const CameraManager&) {}

	static std::vector<Camera*> cameras;

public:
	
	static Camera* RegisterCamera(Camera* camera_) { cameras.push_back(camera_); return camera_; }

	static Camera* GetCamera(unsigned index) {
		if (index < cameras.size()) return cameras[index];
		return nullptr;
	}

};