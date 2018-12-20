#pragma once
#include <boost/chrono.hpp>
#include <queue>
#include <fstream>

class ResourcesManager {
	ResourcesManager() {}
	ResourcesManager(const ResourcesManager&) {}
	ResourcesManager& operator=(const ResourcesManager&) {}

	static boost::chrono::high_resolution_clock::time_point prevTime;
	static float lastFrameTime;
	static std::queue<float> frameTime;

	static void AddFrameTimeToQueue(float time);

public:
	static const int targetFPS;
	static const float targetFrameTime;
	static float timeResidue;

	static int GetFrameCount();

	static void InitTimer() { prevTime = boost::chrono::high_resolution_clock::now(); }

	static void StartFrameTimeMeasurment() { prevTime = boost::chrono::high_resolution_clock::now(); }
	static void StopFrameTimeMeasurment();

	static void WriteFPSToFile();
};