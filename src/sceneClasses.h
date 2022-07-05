#ifndef SCENES_H
#define SCENES_H


#include "scene.h"

class ArenaScene : public Scene
{
public:
	ArenaScene();
	virtual void render();
	virtual void update(double seconds_elapsed);
	virtual void reset();
	void renderAllElements();
	void renderElement(struct Entity element);
	void buildLevel();
	virtual Character* getCharacters(int i);
	virtual void createUI();

	void createQuad(int i, float x, float y, float w, float h);


	Mesh quads[2];
	Shader* uiShader;

	struct Entity temp;
	Shader* current_shader;
	Mesh* bg = NULL;
	Texture* sky = NULL;
	Matrix44 skyPos;
	Mesh* ground = NULL;
	int sWidth, sHeight;

};

class MenuScene : public Scene
{
public:
	MenuScene();
	virtual void render();
	virtual void update(double seconds_elapsed);
	virtual void reset();
	void createQuad(int i, float x, float y, float w, float h);
	

	Mesh quads[1];
	Shader* shader;
	
	

};

class EndScene : public Scene
{
public:
	EndScene();
	virtual void render();
	virtual void update(double seconds_elapsed);
	virtual void reset();
	virtual void setType(int type);
	void createQuad(int i, float x, float y, float w, float h);


	Mesh quads[1];
	Shader* shader;

	int endType = 0;
	

};

#endif