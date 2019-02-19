#include "ConsoleLayer.h"

void ConsoleLayer::JoinToParent(ConsoleLayer* parent_) {
	if (parent_->JoinChild(this))
		parent = parent_;
	else throw std::exception("Child already exists!");
}

bool ConsoleLayer::JoinChild(ConsoleLayer* child_) {
	bool exist = false;
	for (ConsoleLayer* child : children) {
		if (typeid(*child) == typeid(*child_)) {
			exist = true;
			break;
		}
	}

	if (!exist) {
		children.push_back(child_);
		child_->parent = this;
	}
	return !exist;
}

void ConsoleLayer::AddCommand(ConsoleCommand* command_) {
	commands.push_back(command_);
}

bool ConsoleLayer::PerformCommand(std::string command_) {
	for (ConsoleCommand* command : commands) {
		if (command->name == command_) {
			command->Process();
			return true;
		}
	}
	
	std::cout << "Command not found!\n";
	return false;
}

ConsoleLayer* ConsoleLayer::GetChildLayer(std::string name_) {
	for (ConsoleLayer* layer : children) {
		if (layer->name == name_) return layer;
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
//--------------------------------CONSOLE MENU---------------------------------
//-----------------------------------------------------------------------------

void ConsoleMenu::InitCommands() {
	//AddCommand(new LoadModel());
	AddCommand(new ShowGround());
	AddCommand(new HideGround());
	AddCommand(new ShowAxis());
	AddCommand(new HideAxis());
}

void ConsoleMenu::InitChildren() {
	JoinChild(new ModelEditMenu());
	JoinChild(new AnimationsMenu());
	JoinChild(new ImportExportMenu());
}

void ConsoleMenu::Say() {
	std::cout << "Current layer: " << name << std::endl << CON::identation << "Commands:\n";
	for (ConsoleCommand* command : commands) {
		std::cout << CON::identation << CON::identation << command->name << std::endl;
	}
	std::cout << CON::identation << "Layers:\n";
	for (ConsoleLayer* layer : children) {
		std::cout << CON::identation << CON::identation << layer->name << std::endl;
	}
}

//-----------------------------------------------------------------------------
//-------------------------------MODEL EDIT MENU-------------------------------
//-----------------------------------------------------------------------------

void ModelEditMenu::InitCommands() {
	AddCommand(new ShowModels());
	AddCommand(new ChooseModel());
	AddCommand(new ShowCurrentModel());
	AddCommand(new ShowObjects());
	AddCommand(new ChooseObject());
	AddCommand(new ShowCurrentObject());
	AddCommand(new DeleteModel());
	AddCommand(new ShowHitboxesData());
	AddCommand(new ChooseHitbox());
	AddCommand(new ChooseMainHitbox());
	AddCommand(new ShowCurrentHitbox());
	AddCommand(new ScaleHitbox());
	AddCommand(new ResetHitboxVertices());
	AddCommand(new LinkHitboxes());
	AddCommand(new UnlinkHitboxes());
	AddCommand(new ChangeHitboxName());
	AddCommand(new ChangeHitboxDamageMultiplier());
}

void ModelEditMenu::Say() {
	std::cout << "Current layer: " << name << std::endl << CON::identation << "Commands:\n";
	for (ConsoleCommand* command : commands) {
		std::cout << CON::identation << CON::identation << command->name << std::endl;
	}
	std::cout << CON::identation << "Layers:\n";
	for (ConsoleLayer* layer : children) {
		std::cout << CON::identation << CON::identation << layer->name << std::endl;
	}
	std::cout << CON::identation << "Parent:\n";
	std::cout << CON::identation << CON::identation << parent->name << std::endl;
}

//-----------------------------------------------------------------------------
//-------------------------------ANIMATIONS MENU-------------------------------
//-----------------------------------------------------------------------------

void AnimationsMenu::InitCommands() {
	AddCommand(new ShowModels());
	AddCommand(new ChooseModel());
	AddCommand(new ShowObjects());
	AddCommand(new ChooseObject());
	AddCommand(new ShowAllAnimationsData());
	AddCommand(new StartAnimation());
	AddCommand(new StopAnimation());
	AddCommand(new ShowAnimationData());
	AddCommand(new ShowActiveAnimations());
	AddCommand(new ResetAnimations());
}

void AnimationsMenu::Say() {
	std::cout << "Current layer: " << name << std::endl << CON::identation << "Commands:\n";
	for (ConsoleCommand* command : commands) {
		std::cout << CON::identation << CON::identation << command->name << std::endl;
	}
	std::cout << CON::identation << "Layers:\n";
	for (ConsoleLayer* layer : children) {
		std::cout << CON::identation << CON::identation << layer->name << std::endl;
	}
	std::cout << CON::identation << "Parent:\n";
	std::cout << CON::identation << CON::identation << parent->name << std::endl;
}


//-----------------------------------------------------------------------------
//-----------------------------IMPORT/EXPORT MENU------------------------------
//-----------------------------------------------------------------------------

void ImportExportMenu::InitCommands() {
	AddCommand(new LoadModel());
	AddCommand(new ExportModel());
}

void ImportExportMenu::Say() {
	std::cout << "Current layer: " << name << std::endl << CON::identation << "Commands:\n";
	for (ConsoleCommand* command : commands) {
		std::cout << CON::identation << CON::identation << command->name << std::endl;
	}
	std::cout << CON::identation << "Layers:\n";
	for (ConsoleLayer* layer : children) {
		std::cout << CON::identation << CON::identation << layer->name << std::endl;
	}
	std::cout << CON::identation << "Parent:\n";
	std::cout << CON::identation << CON::identation << parent->name << std::endl;
}
