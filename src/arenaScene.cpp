#include "arenaScene.h"
#include "character.h"


#include <algorithm>

int dice;
Shader* current_shader;
Mesh* bg = NULL;
Texture* sky = NULL;
Matrix44 skyPos;

bool sort_func(const struct Entity& a, const struct Entity& b) {
	if (a.type < b.type) {
		return true;
	}
	return false;
};

ArenaScene::ArenaScene()
{

	sky = new Texture();
	sky->load("data/skybox.tga");
	bg = Mesh::Get("data/sphere.obj");

	temp.type = 0;//water

	elements.push_back(temp);

	

	//order vector

	std::sort(elements.begin(), elements.end(), sort_func);

	//Add A CHARACTER

	characters.push_back(new Character(Vector3(0, 13, 0), 180));
	characters.push_back(new Character(Vector3(0, 13, 13), 180));
	characters.push_back(new Character(Vector3(13, 13, 0), 180));
	playerCharacter = characters[0];
	playerCharacter->setControlled(true);
}

void ArenaScene::update(double seconds_elapsed) {
	for (int i = 0; i < characters.size(); i++) {
		characters[i]->update(seconds_elapsed);
	}
}
void  ArenaScene::render() {
	
	renderAllElements();

	for (int i = 0; i < characters.size(); i++) {
		characters[i]->render(Game::instance->camera);
	}
	playerCharacter->render(Game::instance->camera);
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
		
		if (Game::instance->camera->testSphereInFrustum(elements[i].m * Game::instance->elementType[elements[i].type].mesh->box.center, Game::instance->elementType[elements[i].type].mesh->radius) == false)
		continue;

		renderElement(elements[i]);

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