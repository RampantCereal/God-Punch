#ifndef ARENASCENE_H
#define ARENASCENE_H


#include "scene.h"

class ArenaScene : public Scene
{
public:
	ArenaScene();
	virtual void render();
	virtual void update(double seconds_elapsed);
	void renderAllElements();
	void renderElement(struct Entity element);

	struct Entity temp;
};

#endif