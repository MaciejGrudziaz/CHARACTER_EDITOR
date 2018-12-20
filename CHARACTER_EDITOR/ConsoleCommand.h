#pragma once
#include <string>
#include <iostream>
#include <string>
#include <stdio.h>
#include "Graphics.h"

bool CheckIfStringIsInt(std::string str);
bool CheckIfStringIsFloat(std::string str);

bool CheckCurrModelIdx();
bool CheckCurrObjectIdx();
bool CheckCurrHitboxIdx();
bool CheckCurrAnimationIdx();

namespace CON {
	const std::string identation = "____";
}

struct ConsoleCommand {
	std::string name;

	ConsoleCommand(std::string name_) :name(name_) {}
	virtual void Process() = 0;
};

struct LoadModel:public ConsoleCommand {
	LoadModel() :ConsoleCommand("import") {}

	void Process();
};

struct ShowGround :public ConsoleCommand {
	ShowGround() :ConsoleCommand("show_ground") {}

	void Process() { Graphics::SetGroundVisibility(true); std::cout << "Ground enabled!\n"; }
};

struct HideGround :public ConsoleCommand {
	HideGround() :ConsoleCommand("hide_ground") {}

	void Process() { Graphics::SetGroundVisibility(false); std::cout << "Ground disabled!\n"; }
};

struct ShowAxis :public ConsoleCommand {
	ShowAxis() :ConsoleCommand("show_axis") {}

	void Process() { Graphics::SetAxisVisibility(true); std::cout << "Axis enabled!\n"; }
};

struct HideAxis :public ConsoleCommand {
	HideAxis():ConsoleCommand("hide_axis"){}

	void Process() { Graphics::SetAxisVisibility(false); std::cout << "Axis disabled\n"; }
};

struct ShowModels :public ConsoleCommand {
	ShowModels() :ConsoleCommand("show_models") {}

	void Process();
};

struct ChooseModel : public ConsoleCommand {
	ChooseModel() :ConsoleCommand("choose_model") {}

	void Process();
};

struct ShowCurrentModel :public ConsoleCommand {
	ShowCurrentModel() :ConsoleCommand("current_model") {}

	void Process();
};

struct ShowHitboxesData :public ConsoleCommand {
	ShowHitboxesData() :ConsoleCommand("hitbox_data") {}

	void Process();
};

struct ChooseHitbox :public ConsoleCommand {

	ChooseHitbox():ConsoleCommand("choose_hitbox") {}

	void Process();
};

struct ShowCurrentHitbox: public ConsoleCommand {
	ShowCurrentHitbox() :ConsoleCommand("current_hitbox") {}

	void Process();
};

struct ScaleHitbox :public ConsoleCommand {
	ScaleHitbox() :ConsoleCommand("scale_hitbox") {}

	void Process();

	void CalcNewHitboxCoords(int modelIdx, int objectIdx, int hitboxIdx, float scale[]);
};

struct ResetHitboxVertices :public ConsoleCommand {
	ResetHitboxVertices() :ConsoleCommand("reset_hitbox_scale") {}

	void Process();
};

struct LinkHitboxes :public ConsoleCommand {
	LinkHitboxes():ConsoleCommand("link_hitboxes"){}

	void Process();
};

struct UnlinkHitboxes :public ConsoleCommand {
	UnlinkHitboxes() :ConsoleCommand("unlink_hitboxes") {}

	void Process();
};

struct ChangeHitboxName :public ConsoleCommand {
	ChangeHitboxName() :ConsoleCommand("change_name") {}

	void Process();
};

struct ChangeHitboxDamageMultiplier :public ConsoleCommand {
	ChangeHitboxDamageMultiplier() :ConsoleCommand("change_damage") {}

	void Process();
};

struct ShowObjects :public ConsoleCommand {
	ShowObjects() :ConsoleCommand("show_objects") {}

	void Process();
};

struct ChooseObject :public ConsoleCommand {
	ChooseObject() :ConsoleCommand("choose_object") {}

	void Process();
};

struct ShowCurrentObject :public ConsoleCommand {
	ShowCurrentObject() :ConsoleCommand("current_object") {}

	void Process();
};

struct ShowAllAnimationsData : public ConsoleCommand {
	ShowAllAnimationsData() :ConsoleCommand("show_all") {}

	void Process();
};

struct ShowAnimationData :public ConsoleCommand {
	ShowAnimationData() :ConsoleCommand("show") {}

	void Process();
};

struct StartAnimation :public ConsoleCommand {
	StartAnimation() :ConsoleCommand("start") {}

	void Process();
};

struct StopAnimation :public ConsoleCommand {
	StopAnimation() :ConsoleCommand("stop") {}

	void Process();
};

struct ShowActiveAnimations :public ConsoleCommand {
	ShowActiveAnimations() :ConsoleCommand("active_animations") {}

	void Process();
};

struct ResetAnimations :public ConsoleCommand {
	ResetAnimations() :ConsoleCommand("stop_all") {}

	void Process();
};

struct ExportModel :public ConsoleCommand {
	ExportModel() :ConsoleCommand("export") {}

	void Process();
};

struct DeleteModel :public ConsoleCommand {
	DeleteModel() :ConsoleCommand("delete") {}

	void Process();
};
