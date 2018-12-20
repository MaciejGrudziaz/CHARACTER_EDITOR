#pragma once
#include <string>
#include <vector>
#include "ConsoleCommand.h"

struct ConsoleLayer {
	std::string name;
	ConsoleLayer* parent;
	std::vector<ConsoleLayer*> children;
	std::vector<ConsoleCommand*> commands;

	virtual void InitCommands() = 0;

	ConsoleLayer(std::string name_) :name(name_),parent(nullptr) { }

	void JoinToParent(ConsoleLayer* parent_);

	bool JoinChild(ConsoleLayer* child_);

	void AddCommand(ConsoleCommand* command_);

	bool PerformCommand(std::string command_);

	ConsoleLayer* GetChildLayer(std::string name_);
	ConsoleLayer* GetParentLayer() { return parent; }

	virtual void Say() = 0;

	~ConsoleLayer() {
		for (ConsoleCommand* command : commands)
			delete command;
	}
};


struct ConsoleMenu :public ConsoleLayer {
	ConsoleMenu() :ConsoleLayer("menu"){ InitCommands(); InitChildren(); }

	void InitCommands();

	void InitChildren();

	void Say();
};

struct ModelEditMenu :public ConsoleLayer {
	ModelEditMenu() :ConsoleLayer("edit") { InitCommands(); }

	void InitCommands();

	void Say();
};

struct AnimationsMenu :public ConsoleLayer {
	AnimationsMenu() :ConsoleLayer("anim") { InitCommands(); }

	void InitCommands();

	void Say();
};

struct ImportExportMenu :public ConsoleLayer {
	ImportExportMenu() :ConsoleLayer("file") { InitCommands(); }

	void InitCommands();

	void Say();
};
