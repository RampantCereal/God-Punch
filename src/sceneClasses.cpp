#include "sceneClasses.h"
#include "character.h"
#include "input.h"
#include "game.h"


#include <algorithm>

int dice;


bool sort_func(const struct Entity& a, const struct Entity& b) {
	if (a.type < b.type) {
		return true;
	}
	return false;
};

ArenaScene::ArenaScene()
{
	createUI();

	sky = new Texture();
	sky->load("data/skybox.tga");
	bg = Mesh::Get("data/sphere.obj");
	

	uiShader = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	

	//order vector

	std::sort(elements.begin(), elements.end(), sort_func);
	std::sort(elementsCull.begin(), elementsCull.end(), sort_func);

	//Add Characters

	characters.push_back(new Character(Vector3(0, 0, 20), 0,5));
	playerCharacter = new Character(Vector3(-1, 0, -20), 180,6);

	for (int i = 0; i < characters.size(); i++) {// set target
		characters[i]->target = playerCharacter;
	}
	buildLevel();
}

void ArenaScene::update(double seconds_elapsed) {
	
	for (int i = 0; i < characters.size(); i++) {
		characters[i]->update(seconds_elapsed);
	}
	playerCharacter->update(seconds_elapsed);
	if (Input::wasKeyPressed(SDL_SCANCODE_Z)) {
		Game::instance->changeScene(2);
	}
	 

	
	createQuad(0, (sWidth*0.5) - 50, sHeight*0.1, (sWidth*0.5) + 50-(100-playerCharacter->health), sHeight*0.1 + 20);//update player health
	createQuad(1, (sWidth*0.5) - 50, sHeight*0.9, (sWidth*0.5) + 50 - (100 - characters[0]->health), sHeight*0.9 + 20);//update enemy health
}
void  ArenaScene::render() {

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderAllElements();

	characters[0]->render(Game::instance->camera);
	
	for (int i = 1; i < characters.size(); i++) {// draw with culling
		if (Game::instance->camera->testSphereInFrustum(characters[i]->model * Game::instance->elementType[characters[i]->type].mesh->box.center, Game::instance->elementType[characters[i]->type].mesh->radius) == false)
			continue;
		characters[i]->render(Game::instance->camera);
	}
	playerCharacter->render(Game::instance->camera);

	

	uiShader->enable();//draw UI
	uiShader->setUniform("u_texture", Texture::Get("data/white.tga"));
	for (int i = 0; i < 2;i++) {

		if(i == 0)
			uiShader->setUniform("u_color", Vector4(0, 1, 0, 1));
		else if(i == 1)
			uiShader->setUniform("u_color", Vector4(1, 0, 0, 1));

		quads[i].render(GL_TRIANGLES);
	}
	uiShader->disable();
	
}




void  ArenaScene::renderAllElements() {
	skyPos.setTranslation(Game::instance->camera->eye.x, Game::instance->camera->eye.y, Game::instance->camera->eye.z);

	glDisable(GL_DEPTH_TEST);//skybox
	current_shader = Game::instance->elementType[2].shader;
	current_shader->enable();
	current_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	current_shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
	current_shader->setUniform("u_texture", sky);

	current_shader->setUniform("u_model", skyPos);
	bg->render(GL_TRIANGLES);
	current_shader->disable();
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < elements.size(); i++) {
		renderElement(elements[i]);
	}

	for (int i = 0; i < elementsCull.size(); i++) {
		
		if (Game::instance->camera->testSphereInFrustum(elementsCull[i].m * Game::instance->elementType[elementsCull[i].type].mesh->box.center, Game::instance->elementType[elementsCull[i].type].mesh->radius) == false)
		continue;

		renderElement(elementsCull[i]);

	}
	if(current_shader->isEnabled())
	current_shader->disable();

	
}

void  ArenaScene::renderElement(struct Entity element) {
	if (!Game::instance->elementType[element.type].shader->isEnabled()){
		if (current_shader->isEnabled())
			current_shader->disable();
		current_shader = Game::instance->elementType[element.type].shader;
		current_shader->enable();

	}
	current_shader->setUniform("u_model", element.m);
	current_shader->setUniform("u_texture", Game::instance->elementType[element.type].texture);
	current_shader->setUniform("u_color", element.color);
	current_shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
	current_shader->setUniform("u_time", Game::instance->time);

	Game::instance->elementType[element.type].mesh->render(GL_TRIANGLES);
}
void ArenaScene::reset() {
	playerCharacter->reset();
	characters[0]->reset();
}

void ArenaScene::buildLevel() {
	temp.type = 1;
	temp.m.setTranslation(0, 0, 0);
	elementsCull.push_back(temp);

	temp.type = 2;//walls
	temp.m.setTranslation(-40, 0, -50);
	elementsCull.push_back(temp);
	temp.m.setTranslation(-20, 0, -50);
	elementsCull.push_back(temp);
	temp.m.setTranslation(0, 0, -50);
	elementsCull.push_back(temp);
	temp.m.setTranslation(40, 0, -50);
	elementsCull.push_back(temp);
	temp.m.setTranslation(20, 0, -50);
	elementsCull.push_back(temp);

	temp.m.setTranslation(-40, 0, 50);
	elementsCull.push_back(temp);
	temp.m.setTranslation(-20, 0, 50);
	elementsCull.push_back(temp);
	temp.m.setTranslation(0, 0, 50);
	elementsCull.push_back(temp);
	temp.m.setTranslation(40, 0, 50);
	elementsCull.push_back(temp);
	temp.m.setTranslation(20, 0, 50);
	elementsCull.push_back(temp);

	temp.m.rotateGlobal(90 * DEG2RAD, Vector3(0, 1, 0)); 
		temp.m.translate(40, 0, 0);
	for (int i = 0; i < 5;i++) {
		temp.m.translate(-20, 0, 0);
		elementsCull.push_back(temp);
		temp.m.translate(0, 0, -100);
		elementsCull.push_back(temp);
		temp.m.translate(0, 0, 100);
	}
	

	characters.push_back(new Character(Vector3(-40, 5, 50), 0, 0));
	characters.push_back(new Character(Vector3(-20, 5, 50), 0 , 0));
	characters.push_back(new Character(Vector3(-10, 5, 50), 0, 0));
	characters.push_back(new Character(Vector3(0, 5, 50), 0 , 0));
	characters.push_back(new Character(Vector3(20, 5, 50), 0, 0));
	characters.push_back(new Character(Vector3(30, 5, 50), 0, 0));
	characters.push_back(new Character(Vector3(40, 5, 50), 0, 0));

	characters.push_back(new Character(Vector3(-40, 5, -50), 180, 0));
	characters.push_back(new Character(Vector3(-20, 5, -50), 180, 0));
	characters.push_back(new Character(Vector3(-10, 5, -50), 180, 0));
	characters.push_back(new Character(Vector3(0, 5, -50), 180, 0));
	characters.push_back(new Character(Vector3(20, 5, -50), 180, 0));
	characters.push_back(new Character(Vector3(30, 5, -50), 180, 0));
	characters.push_back(new Character(Vector3(40, 5, -50), 180, 0));

	characters.push_back(new Character(Vector3(50, 5, -40), 270, 0));
	characters.push_back(new Character(Vector3(50, 5, -30), 270, 0));
	characters.push_back(new Character(Vector3(50, 5, -10), 270, 0));
	characters.push_back(new Character(Vector3(50, 5, 0), 270, 0));
	characters.push_back(new Character(Vector3(50, 5, 10), 270, 0));
	characters.push_back(new Character(Vector3(50, 5, 40), 270, 0));
	characters.push_back(new Character(Vector3(50, 5, 50), 270, 0));

	characters.push_back(new Character(Vector3(-50, 5, -40), 90, 0));
	characters.push_back(new Character(Vector3(-50, 5, -30), 90, 0));
	characters.push_back(new Character(Vector3(-50, 5, -10), 90, 0));
	characters.push_back(new Character(Vector3(-50, 5, 0), 90, 0));
	characters.push_back(new Character(Vector3(-50, 5, 10), 90, 0));
	characters.push_back(new Character(Vector3(-50, 5, 40), 90, 0));
	characters.push_back(new Character(Vector3(-50, 5, 50), 90, 0));


}

Character* ArenaScene::getCharacters(int i) {
	return characters[i];
}
void ArenaScene::createQuad(int i, float x, float y, float w, float h) {

	float x_c, y_c, w_c, h_c;

	x_c = ((x / Game::instance->window_width) * 2) - 1;
	y_c = ((y / Game::instance->window_height) * 2) - 1;
	w_c = ((w / Game::instance->window_width) * 2) - 1;
	h_c = ((h / Game::instance->window_height) * 2) - 1;

	quads[i].vertices.clear();
	quads[i].uvs.clear();

	//tres vertices del primer triangulo
	quads[i].vertices.push_back(Vector3(x_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(0, 1));
	quads[i].vertices.push_back(Vector3(x_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(0, 0));
	quads[i].vertices.push_back(Vector3(w_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(1, 0));

	//tres vértices del segundo triángulo
	quads[i].vertices.push_back(Vector3(x_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(0, 1));
	quads[i].vertices.push_back(Vector3(w_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(1, 0));
	quads[i].vertices.push_back(Vector3(w_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(1, 1));
}

void ArenaScene::createUI() {
	sWidth = Game::instance->window_width;
	sHeight = Game::instance->window_height;

	createQuad(0, (sWidth*0.5) - 50, sHeight*0.1, (sWidth*0.5) + 50, sHeight*0.1 + 20);//player health
	createQuad(1, (sWidth*0.5) - 50, sHeight*0.9, (sWidth*0.5) + 50, sHeight*0.9 + 20);//enemy health
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MenuScene::MenuScene() {
	
	
	createQuad(0, 0, 0, Game::instance->window_width, Game::instance->window_height);

	shader = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	

}
void MenuScene::update(double seconds_elapsed) {
	if (Input::wasKeyPressed(SDL_SCANCODE_Z) || Input::wasButtonPressed(1) || Input::mouse_state & SDL_BUTTON_LMASK) {
		Game::instance->playSound(2);
		Game::instance->changeScene(1);
	}
		
}
void MenuScene::render() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	
	shader->enable();
	shader->setUniform("u_color", Vector4(1,1, 1, 1));

	shader->setUniform("u_texture", Texture::Get("data/menu.tga"));
	quads[0].render(GL_TRIANGLES);
	
	shader->disable();
	


}
void MenuScene::reset() {
	
}

void MenuScene::createQuad(int i,float x, float y, float w, float h) {

	float x_c, y_c, w_c, h_c;

	x_c = ((x / Game::instance->window_width)*2)-1;
	y_c = ((y / Game::instance->window_height) * 2) - 1;
	w_c = ((w / Game::instance->window_width) * 2) - 1;
	h_c = ((h / Game::instance->window_height) * 2) - 1;

	quads[i].vertices.clear();
	quads[i].uvs.clear();

	//tres vertices del primer triangulo
	quads[i].vertices.push_back(Vector3(x_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(0, 1));
	quads[i].vertices.push_back(Vector3(x_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(0, 0));
	quads[i].vertices.push_back(Vector3(w_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(1, 0));

	//tres vértices del segundo triángulo
	quads[i].vertices.push_back(Vector3(x_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(0, 1));
	quads[i].vertices.push_back(Vector3(w_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(1, 0));
	quads[i].vertices.push_back(Vector3(w_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(1, 1));


}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EndScene::EndScene() {
	createQuad(0, 0, 0, Game::instance->window_width, Game::instance->window_height);

	shader = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
}
void EndScene::update(double seconds_elapsed) {
	if (Input::wasKeyPressed(SDL_SCANCODE_Z) || Input::wasButtonPressed(1) || Input::mouse_state & SDL_BUTTON_LMASK) {
		Game::instance->playSound(2);
		Game::instance->changeScene(0);
	}
}
void EndScene::render() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->enable();
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	if(endType == 0)	
		shader->setUniform("u_texture", Texture::Get("data/END_GOOD.tga"));
	else if(endType == 1)
		shader->setUniform("u_texture", Texture::Get("data/END_BAD.tga"));
	quads[0].render(GL_TRIANGLES);

	shader->disable();
	
}
void EndScene::reset() {

}

void EndScene::setType(int type) {
	endType = type;
}

void EndScene::createQuad(int i, float x, float y, float w, float h) {

	float x_c, y_c, w_c, h_c;

	x_c = ((x / Game::instance->window_width) * 2) - 1;
	y_c = ((y / Game::instance->window_height) * 2) - 1;
	w_c = ((w / Game::instance->window_width) * 2) - 1;
	h_c = ((h / Game::instance->window_height) * 2) - 1;

	quads[i].vertices.clear();
	quads[i].uvs.clear();

	//tres vertices del primer triangulo
	quads[i].vertices.push_back(Vector3(x_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(0, 1));
	quads[i].vertices.push_back(Vector3(x_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(0, 0));
	quads[i].vertices.push_back(Vector3(w_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(1, 0));

	//tres vértices del segundo triángulo
	quads[i].vertices.push_back(Vector3(x_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(0, 1));
	quads[i].vertices.push_back(Vector3(w_c, y_c, 0));
	quads[i].uvs.push_back(Vector2(1, 0));
	quads[i].vertices.push_back(Vector3(w_c, h_c, 0));
	quads[i].uvs.push_back(Vector2(1, 1));


}