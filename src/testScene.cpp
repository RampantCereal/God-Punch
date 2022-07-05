#include "testScene.h"
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

TestScene::TestScene()
{

	sky = new Texture();
	sky->load("data/skybox.tga");
	bg = Mesh::Get("data/sphere.obj");

	temp.type = 0;//water

	elements.push_back(temp);

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			dice = rand() % 100;
			if (dice <= 40)
				this->tileset1(i, j);//village
			else if (dice <= 70) {//sunken
				this->tileset2(i, j);
			}

		}
	}

	//order vector

	std::sort(elements.begin(), elements.end(), sort_func);

	//Add A CHARACTER

	characters.push_back(new Character(Vector3(0, 13, 0), 180));
	characters.push_back(new Character(Vector3(0, 13, 13), 180));
	characters.push_back(new Character(Vector3(13, 13, 0), 180));
	playerCharacter = characters[0];
	playerCharacter->setControlled(true);
}

void TestScene::update(double seconds_elapsed) {
	for (int i = 0; i < characters.size(); i++) {
		characters[i]->update(seconds_elapsed);
	}
}
void TestScene::render() {
	
	renderAllElements();

	for (int i = 0; i < characters.size(); i++) {
		characters[i]->render(Game::instance->camera);
	}
	playerCharacter->render(Game::instance->camera);
}

void TestScene::tileset1(int x, int z) {

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.type = 1;//field

	temp.m.translate(43, 14, 169);
	elements.push_back(temp);

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.type = 2;//dock

	temp.m.translate(32, 0, -2);
	for (int i = 0; i < 12; i++) {
		temp.m.translate(0, 0, 16);
		elements.push_back(temp);
	}

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.type = 3;//barn

	temp.m.translate(79, 13, 170);
	elements.push_back(temp);
	temp.m.setTranslation(x * 200, 0, z * 200);//reset

	temp.m.rotate((float)(180 * DEG2RAD), Vector3(0.0f, 1.0f, 0.0f));
	temp.m.translate(-25, 13, -17);
	for (int i = 0; i < 3; i++) {
		temp.m.translate(0, 0, -33);
		elements.push_back(temp);
	}
	temp.m.setIdentity();

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.type = 4;//house

	temp.m.translate(100, 13, 170);
	elements.push_back(temp);

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.type = 5;//minihouse

	temp.m.rotate((float)(270 * DEG2RAD), Vector3(0.0f, 1.0f, 0.0f));
	temp.m.translate(-5, 13, 160);
	for (int i = 0; i < 5; i++) {
		temp.m.translate(-25, 0, 0);
		elements.push_back(temp);
	}
	temp.m.setRotation(0, Vector3(0.0f, 1.0f, 0.0f));

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.type = 7;//templete

	temp.m.translate(111, 13, 89);
	for (int i = 0; i < 2; i++) {
		temp.m.translate(-10, 0, 0);
		elements.push_back(temp);
	}

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.m.translate(111, 13, 79);
	for (int i = 0; i < 2; i++) {
		temp.m.translate(-10, 0, 0);
		elements.push_back(temp);
	}

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.type = 8;//tower

	temp.m.translate(138, 13, 180);
	elements.push_back(temp);

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.m.translate(100, 13, 100);
	elements.push_back(temp);

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.type = 9;//wall

	temp.m.translate(61, 13, 141);
	elements.push_back(temp);
	temp.m.setTranslation(x * 200, 0, z * 200);//reset

	temp.m.rotate((float)(90 * DEG2RAD), Vector3(0.0f, 1.0f, 0.0f));
	temp.m.translate(152, 13, -71);
	elements.push_back(temp);
	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.m.setRotation(0, Vector3(0.0f, 1.0f, 0.0f));

	temp.m.setTranslation(x * 200, 0, z * 200);//reset
	temp.m.rotate((float)(180 * DEG2RAD), Vector3(0.0f, 1.0f, 0.0f));
	temp.m.translate(-25, 13, -141);
	elements.push_back(temp);
	temp.m.setRotation(0, Vector3(0.0f, 1.0f, 0.0f));
}

//sunken buildings
void TestScene::tileset2(int x, int z) {
	temp.m.setTranslation(x * 200, 0, z * 200);//reset

	temp.type = 8;
	temp.m.translate(rand() % 150, -20, 100);
	temp.m.rotate((float)(10 * DEG2RAD), Vector3(0.0f, 0.0f, 1.0f));
	elements.push_back(temp);

	temp.m.translate(rand() % 100, 0, -25);
	temp.m.rotate((float)(-20 * DEG2RAD), Vector3(0.0f, 0.0f, 1.0f));
	elements.push_back(temp);
}

void TestScene::renderAllElements() {
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

void TestScene::renderElement(struct Entity element) {
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