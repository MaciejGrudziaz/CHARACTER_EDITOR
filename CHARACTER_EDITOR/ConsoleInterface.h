#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include "ConsoleLayer.h"

class ConsoleInterface {
	ConsoleInterface() {}
	ConsoleInterface(const ConsoleInterface&) {}
	ConsoleInterface& operator=(const ConsoleInterface&) {}

	static CONSOLE_SCREEN_BUFFER_INFO conInfo;

	static std::vector<ConsoleLayer*> layers;

	static ConsoleLayer* root;
	static ConsoleLayer* currentLayer;

	static ConsoleLayer* AddLayer(ConsoleLayer* layer_) { layers.push_back(layer_); return layer_; }

	static void HelloMsg();
	static void ChangeLayer();
	static void HelpMessage();
	static void Back();

	static bool ChangeLayer(std::string msg);

	static void InitLayers();	

public:

	static void Init();

	static void Process();
};