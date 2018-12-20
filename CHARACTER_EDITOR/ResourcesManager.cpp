#include "ResourcesManager.h"

boost::chrono::high_resolution_clock::time_point ResourcesManager::prevTime;
std::queue<float> ResourcesManager::frameTime;
const int ResourcesManager::targetFPS = 200;
const float ResourcesManager::targetFrameTime=1/(float)targetFPS;
float ResourcesManager::timeResidue=0.0;
float ResourcesManager::lastFrameTime=0.0f;

int ResourcesManager::GetFrameCount() {
	int frameCount = lastFrameTime/targetFrameTime;
	timeResidue += lastFrameTime - static_cast<float>(frameCount)*targetFrameTime;
	if (timeResidue > targetFrameTime) {
		++frameCount;
		timeResidue -= targetFrameTime;
	}

	return frameCount;
}

 void ResourcesManager::StopFrameTimeMeasurment() {
	boost::chrono::duration<float, boost::micro> dur = boost::chrono::high_resolution_clock::now() - prevTime;
	lastFrameTime = dur.count()/1000000.0;
	AddFrameTimeToQueue(lastFrameTime);
}

void ResourcesManager::WriteFPSToFile() {
	std::string content;
	int size = frameTime.size();
	int fpsTotalVal = 0;
	for (int i = 0; i < size - 10; ++i) {
		int fpsVal = static_cast<int>(1.0 / (frameTime.front()));
		fpsTotalVal += fpsVal;
		frameTime.pop();
		//file << fpsVal << std::endl;
		content += std::to_string(fpsVal);
		content += "\n";
	}
	std::fstream file("LOGS/FPS.txt", std::ios::out | std::ios::trunc);
	assert(file.is_open());
	file << "Average FPS: " << fpsTotalVal / (size - 10) << std::endl;
	file << "FPS values:\n";
	file << content;
	file.close();
}

//---------------------------------------------------------------------------------------
//----------------------------------------PRIVATE----------------------------------------
//---------------------------------------------------------------------------------------

void ResourcesManager::AddFrameTimeToQueue(float time) {
	if (frameTime.size() < 1010.0)
		frameTime.push(time);
	else {
		frameTime.pop();
		frameTime.push(time);
	}
}