#include "ConsoleInterface.h"

CONSOLE_SCREEN_BUFFER_INFO ConsoleInterface::conInfo;
std::vector<ConsoleLayer*> ConsoleInterface::layers;
ConsoleLayer* ConsoleInterface::root=nullptr;
ConsoleLayer* ConsoleInterface::currentLayer=nullptr;

void ConsoleInterface::Init() {
	AllocConsole();

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);
	conInfo.dwSize.Y = 500;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), conInfo.dwSize);

	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stdout);

	InitLayers();
}

void ConsoleInterface::Process() {
	HelloMsg();
	currentLayer->Say();

	std::string message;
	while (message!="quit") {
		message.clear();
		std::cout << ">> ";
		std::cin >> message;
		if (message == "ls") currentLayer->Say();
		//else if (message == "layer") ChangeLayer();
		else if (message == "..") Back();
		else if (message=="root") currentLayer = root;
		else if (message == "help") HelpMessage();
		else if(!ChangeLayer(message)) currentLayer->PerformCommand(message);

		std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	 }

	std::cout << "Interface closed!\n";

	PostMessage(WinAPIwindowManager::GetMainWindow()->GetHWND(), WM_QUIT, NULL, NULL);
}

void ConsoleInterface::HelloMsg() {
	std::cout << "Welcome to MGR engine character editor!\n";
	std::cout << "type help to show help menu\n\n\n";
}

void ConsoleInterface::ChangeLayer() {
	std::string message;
	ConsoleLayer* layer;

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cout << "Choose layer:\n>> ";
	message.clear();
	std::cin >> message;
	layer = currentLayer->GetChildLayer(message);
	if (layer == nullptr) {
		layer = currentLayer->GetParentLayer();

		if(layer==nullptr) std::cout << "Layer does not exist!\n";
		else currentLayer = layer;
	}
	else currentLayer = layer;
}

void ConsoleInterface::HelpMessage() {
	std::cout << "Available commands:\n";
	std::cout << CON::identation <<"help - show help menu\n";
	std::cout << CON::identation << "[command name] - execute command\n";
	std::cout << CON::identation <<"ls - list current layer components\n";
	std::cout << CON::identation <<"layer - change layer\n";
	std::cout << CON::identation << "root - change to root layer\n";
	std::cout << CON::identation <<"quit - quit interface\n";
}

void ConsoleInterface::Back() {
	if (currentLayer->parent != nullptr) {
		currentLayer = currentLayer->parent;
	}
	else std::cout << "Root layer!\n";
}

bool ConsoleInterface::ChangeLayer(std::string msg) {
	if (currentLayer->parent != nullptr) {
		if (currentLayer->parent->name == msg) {
			currentLayer = currentLayer->parent;
			return true;
		}
	}

	for (ConsoleLayer* layer : currentLayer->children) {
		if (layer->name == msg) {
			currentLayer = layer;
			return true;
		}
	}

	return false;
}

void ConsoleInterface::InitLayers() {
	ConsoleLayer* layer = AddLayer(new ConsoleMenu());
	currentLayer = layer;
	root = layer;
}
