#include "scene.h"
#include "utils.h"

Scene::Scene()
{
}

void Scene::update(double seconds_elapsed) {
	std::cout << "calling update on base class" << std::endl;
}
void Scene::render() {
	std::cout << "calling render on base class" << std::endl;
}
void Scene::reset() {
	std::cout << "calling render on base class" << std::endl;
}

Character* Scene::getCharacters(int i) {
	std::cout << "calling getCharacters on base class" << std::endl;
	return NULL;
}

void Scene::setType(int type) {
	std::cout << "calling setType on base class" << std::endl;
}

void Scene::createUI() {
	std::cout << "calling creteUI on base class" << std::endl;
}

