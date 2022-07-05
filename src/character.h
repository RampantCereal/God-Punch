#pragma once

#include "animation.h"



class Character {
public:

	int id;
	std::string name;
	Matrix44 model;

	int type;
	Vector4 color = Vector4(1, 1, 1, 1);

	bool controlled;
	bool enemy;
	
	float height;

	Vector3 offset;

	Vector3 position;
	Vector3 velocity;
	Vector3 front;
	float yaw;
	float pitch;
	
	//animation control stuff
	
	int state; //0 = idle, 1= left attack, 2= right attack, 3= left dodge, 4 = right dodge 5 = strong attack 6 = death
	bool busy;
	float animationTimer;
	float animTime;
	float cancelTime;
	float dodgeTime;
	float aDelay, aTimer;
	bool canAttack;


	bool hitCooldown;
	int stack1;
	int stack2;
	Character* target;
	int health;
	bool busyEasy;
	bool* busyChoice;
	const char* idle_name;
	bool dead;

	Vector3 initPos;
	float initRot;

	//anim
	float anim_time_start;
	Skeleton sk;
	std::vector<Matrix44> bone_matrices;

	Character(Vector3 pos, float rotation, int mode);

	void render(Camera* camera);
	void update(float dt);
	void updateSkeleton(float dt);

	bool visible;

	void updateMatrix();
	void updateCamera(Camera* cam, bool first_person = false);
	void setControlled(bool status);
	bool isControlled();
	bool hit();
	void reset();
	bool isDodging();
	void isVulnerable();
	void reduceStack();
	void fillStack();
	
	
};
