#ifndef SCENE_H
#define SCENE_H

#include "character.h"
#include "game.h"

struct Entity {
	Matrix44 m;
	int type;
	Vector4 color = Vector4(1, 1, 1, 1);
};


class Scene
{
public:
	
	Scene();

	Character* playerCharacter;
	std::vector<Character*> characters;
	std::vector<struct Entity> elementsCull;
	std::vector<struct Entity> elements;

	virtual void update(double dt);
	virtual void render();
	virtual void reset();
	virtual Character* getCharacters(int i);
	virtual void setType(int type);
	virtual void createUI();
	
	
	
	
};

#endif