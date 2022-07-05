#ifndef TESTSCENE_H
#define TESTSCENE_H


#include "scene.h"

class TestScene : public Scene
{
public:
	TestScene();
	virtual void render();
	virtual void update(double seconds_elapsed);
	void renderAllElements();
	void tileset1(int x, int z);
	void tileset2(int x, int z);
	void renderElement(struct Entity element);

	struct Entity temp;
};

#endif