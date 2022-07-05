/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "extra/coldet/coldet.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include "extra/bass.h"
#include "character.h"

struct Entity_type {
	Mesh* mesh;
	Shader* shader;
	Texture* texture;
};

struct Audio_Entity {
	HSAMPLE sample;
	HCHANNEL channel;
};



class Game
{
public:
	static Game* instance;

	struct Entity_type elementType[7];//0:water 1:sand plane 2:wall 3:male 4: female 5:enemy 6:protagonist  
	Audio_Entity samples[3]; //0: hit 1: miss 2: confirmation

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	//some vars
	Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)

	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render( void );
	void update( double dt );

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);
	void loadMeshes();
	void loadTextures();
	void loadShaders();
	void loadSounds();
	void playSound(int i);
	
	void changeScene(int i);
	Character* extractCharacter(int i);
	std::vector<struct Entity> extractElems();

};


#endif 