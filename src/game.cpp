#include "game.h"
#include "includes.h"
#include "utils.h"
#include "fbo.h"
#include "input.h"
#include "animation.h"


#include "sceneClasses.h"

#include <time.h> 
#include <cmath>


//some globals
Scene* currentScene;
std::vector<Scene*> scenes;

Game* Game::instance = NULL;


Game::Game(int window_width, int window_height, SDL_Window* window)
{

	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	srand(__rdtsc());

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	if (BASS_Init(-1, 44100, 0, 0, NULL) == false)
	{
		std::cout << "ERROR" << std::endl;
	}
	BASS_SetVolume(0.1);

	//El handler para un sample
	HSAMPLE music;

	//El handler para un canal
	HCHANNEL musicChannel;

	//Cargamos un sample del disco duro (memoria, filename, offset, length, max, flags)
	//use BASS_SAMPLE_LOOP in the last param to have a looped sound
	music = BASS_SampleLoad(false, "data/music.mp3", 0, 0, 3, BASS_SAMPLE_LOOP);
	
	{
		std::cout << "ERROR" << std::endl;
	}
	//Creamos un canal para el sample
	musicChannel = BASS_SampleGetChannel(music, false);

	//Lanzamos un sample
	BASS_ChannelPlay(musicChannel, true);



	//OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
	

	//create and load meshes
	loadMeshes();
	
	//load textures
	loadTextures();
	
	// load shaders
	loadShaders();

	//load Sounds
	loadSounds();
	playSound(2);
	
	//create scenes
	scenes.push_back(new MenuScene());
	scenes.push_back(new ArenaScene());
	scenes.push_back(new EndScene());
	currentScene = scenes[0];

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//set the camera as default
	camera->enable();

	currentScene->render();

	//render the FPS, Draw Calls, etc
	//drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	currentScene->update(seconds_elapsed);
	
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();

}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
	scenes[1]->createUI();
}

void Game::loadMeshes() {

	elementType[0].mesh = new Mesh();
	elementType[0].mesh->createSubdividedPlane(1000, 15, true);
	elementType[1].mesh = new Mesh();
	elementType[1].mesh->createPlane(50);
	elementType[2].mesh = Mesh::Get("data/wall.obj");
	elementType[3].mesh = Mesh::Get("data/characters/male.mesh");
	elementType[4].mesh = Mesh::Get("data/characters/female.mesh");
	elementType[5].mesh = elementType[3].mesh;
	elementType[6].mesh = elementType[4].mesh;
}

void Game::loadTextures() {

	elementType[0].texture = Texture::Get("data/water.tga");
	elementType[1].texture = Texture::Get("data/sand.tga");
	elementType[2].texture = Texture::Get("data/materials.tga");
	elementType[3].texture = Texture::Get("data/characters/male.png");
	elementType[4].texture = Texture::Get("data/characters/female.png");
	elementType[5].texture = Texture::Get("data/characters/enemy.png");
	elementType[6].texture = Texture::Get("data/characters/protagonist.png");
}

void Game::loadShaders() {

	elementType[0].shader = Shader::Get("data/shaders/water.vs", "data/shaders/water.fs");
	elementType[1].shader = Shader::Get("data/shaders/basic.vs", "data/shaders/textureLoop.fs");
	elementType[2].shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	elementType[3].shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	elementType[4].shader = elementType[3].shader;
	elementType[5].shader = elementType[3].shader;
	elementType[6].shader = elementType[3].shader;
}

void Game::loadSounds() {
	samples[0].sample = BASS_SampleLoad(false, "data/hit.wav", 0, 0, 3, 0);
	samples[0].channel = BASS_SampleGetChannel(samples[0].sample, false);

	samples[1].sample = BASS_SampleLoad(false, "data/miss.wav", 0, 0, 3, 0);
	samples[1].channel = BASS_SampleGetChannel(samples[1].sample, false);

	samples[2].sample = BASS_SampleLoad(false, "data/confirmation.wav", 0, 0, 3, 0);
	samples[2].channel = BASS_SampleGetChannel(samples[2].sample, false);
		
}

void Game::playSound(int i) {
	BASS_ChannelPlay(samples[i].channel, true);
}

void Game::changeScene(int i) {
	currentScene->reset();
	if (i == 3) {
		currentScene = scenes[2];
		currentScene->setType(1);
	}
	else if (i == 2) {
		currentScene = scenes[i];
		currentScene->setType(0);
	}
	else {
		currentScene = scenes[i];
	}
	
}

Character* Game::extractCharacter(int i) {
	
	return scenes[1]->getCharacters(i);
}

std::vector<struct Entity> Game::extractElems() {
	return (scenes[1]->elementsCull);
}