#pragma once
#include <vector>
#include "Character.h"

class CharacterManager {
	CharacterManager() {}
	CharacterManager(const CharacterManager&) {}
	CharacterManager& operator=(const CharacterManager&) {}

	static std::vector<Character*> characters;

public:

	static Character* RegisterCharacter(Character* character_) { 
		if (character_ != nullptr) { characters.push_back(character_); 
		return character_; return nullptr; } 
	}

	static unsigned GetCharactersCount() { return characters.size(); }

	static Character* GetCharacter(unsigned index) { 
		if (index < characters.size()) return characters[index]; 
		else return nullptr; 
	}

	static void DestroyCharacter(int idx) {
		if (idx >= 0 && idx < characters.size()) {
			delete characters[idx];
			characters.erase(characters.begin() + idx);
		}
	}
};