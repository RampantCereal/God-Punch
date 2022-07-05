


#include "input.h"
#include "scene.h"



Character::Character(Vector3 pos, float rotation, int mode )
{
	id = 0;
	visible = true;

	yaw = rotation;
	initRot = rotation;

	busy = false;
	busyEasy = false;
	busyChoice = &busyEasy;

	state = 0;
	pitch = 0;
	height = 10;

	position= pos;
	initPos = pos;

	health = 100;
	dead = false;
	
	
	stack1 = 0;
	stack2 = 0;

	aTimer = 0;
	aDelay = 0.25;
	canAttack = true;

	

	hitCooldown = true;
	if (mode == 0) {
		if (rand() % 2 > 0.5) {
			type = 3;
		}
		else {
			type = 4;
		}
		idle_name = "data/characters/Cheering.skanim";
		controlled = false;
		enemy = false;
	}
	else {
		
		type = mode;
		if (mode == 6) {
			controlled = true;
			enemy = false;
			idle_name = "data/characters/idle.skanim";
		}
		if (mode == 5) {
			enemy = true;
			controlled = false;
			idle_name = "data/characters/idle_combat.skanim";
		}
	}

}

void Character::updateMatrix()
{
	model.setIdentity();
	model.translate(position.x, position.y, position.z);
	model.rotate( (yaw + 180 ) * DEG2RAD, Vector3(0, 1, 0));
}

void Character::updateCamera(Camera* cam, bool first_person )
{
	Matrix44 yaw_mat;
	yaw_mat.rotate(yaw * DEG2RAD, Vector3(0, 1, 0));

	Vector3 right = yaw_mat * Vector3(1, 0, 0);
	Matrix44 pitch_mat;
	pitch_mat.rotate(pitch * DEG2RAD, right);

	Matrix44 r = yaw_mat * pitch_mat;

	Vector3 front = r * Vector3(0, 0, -1);
	Vector3 eye = position + Vector3(0, height, 0)+ offset;
	if (!first_person)
		eye = eye - front * 10 + Vector3(0,2,0);
	Vector3 center = eye + front;
	cam->lookAt(eye, center, Vector3(0, 1, 0));

	visible = true;// !first_person;
}

void Character::render(Camera* camera)
{

	if (!bone_matrices.size())
		return;
	
	
	if (controlled) {
		updateCamera(camera);
		drawText(Game::instance->window_width*0.2, Game::instance->window_height*0.8, std::to_string(stack1), Vector3(0, 0, 1), 2);
		drawText(Game::instance->window_width*0.8, Game::instance->window_height*0.8, std::to_string(stack2), Vector3(1, 0, 0), 2);
	}

	Matrix44 m = model;
	float s = 0.5 * height / 10.0;

	if(enemy)
		m.scale(s+0.20, s+ 0.20, s+ 0.20);
	else
		m.scale(s, s, s);
	
	

	Game::instance->elementType[type].shader->enable();
	Game::instance->elementType[type].shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	Game::instance->elementType[type].shader->setUniform("u_model", m);
	Game::instance->elementType[type].shader->setUniform("u_texture", Game::instance->elementType[type].texture);
	Game::instance->elementType[type].shader->setUniform("u_color", color);
	Game::instance->elementType[type].shader->setUniform("u_bones", bone_matrices );

	Game::instance->elementType[type].mesh->render(GL_TRIANGLES);
	Game::instance->elementType[type].shader->disable();
	

	

	if (0)
	{
		glDisable(GL_DEPTH_TEST);
		glLineWidth(3);
		sk.renderSkeleton(camera, m);
		glEnable(GL_DEPTH_TEST);
	}
}

void Character::update(float dt)
{
	

	if (animationTimer < animTime) {//animation timer
		if(enemy)
			animationTimer += (dt*0.9);
		else if(controlled)
			animationTimer += (dt*(1.0+(0.1*stack1 -0.05*stack2 )));
		else
			animationTimer += (dt);


		if ((state == 2 || state == 1 || state == 5) && hitCooldown && animationTimer< cancelTime && animationTimer> animTime*(0.3+(0.1*enemy)))//hit condition
			if (hit()){
				if (state == 1 && stack1 < 3) {
					stack1++;
				}
				else if (state == 1 && stack1 >= 3 && stack2 >0) {
					stack2--;
				}
				
				if (state == 2 && stack2 < 3) {
					stack2++;
				}
				else if (state == 2 && stack2 >= 3 && stack1 > 0) {
					stack1--;
				}
					
			}
		if (animationTimer >= cancelTime && state != 6) {//cancel before end
			busy = false;
			hitCooldown = true;
			color = Vector4(1, 1, 1, 1);
		}
		if (animationTimer >= dodgeTime)
			color = Vector4(1, 1, 1, 1);
		if (animationTimer >= animTime ) {
			if (state == 6) {
				if(controlled)
					Game::instance->changeScene(3);
				else if(enemy)
					Game::instance->changeScene(2);
			}
			else if (state == 7) {
				canAttack = true;
			}
			state = 0;
			busyEasy = false;
			canAttack = false;
		}	
	}

	Vector3 move;
	float yaw_delta = 0;
	float pitch_delta = 0;

	if (controlled) {//input player code
		if (health <= 0 && !dead) {//enter death state
			animationTimer = 0;
			animTime = Animation::Get("data/characters/death2.skanim")->duration;
			state = 6;
			cancelTime = animTime * 0.7;
			busy = true;
			dead = true;
		}
		if ((Input::isKeyPressed(SDL_SCANCODE_W)||Input::gamepads[0].hat == PAD_UP)&& !busy)
				move.z -= dt * 30;
		if ((Input::isKeyPressed(SDL_SCANCODE_S) || Input::gamepads[0].hat == PAD_DOWN) && !busy)
				move.z += dt * 20;
		if ((Input::isKeyPressed(SDL_SCANCODE_Q) || Input::wasButtonPressed(4)) && !busy) {
			animationTimer = 0;
			animTime = Animation::Get("data/characters/dodgeLeftFast.skanim")->duration;
			cancelTime = animTime * 0.9;
			dodgeTime = animTime * 0.6;
			color = Vector4(0,0,1,1);
			state = 3;
			busy = true;
		}
		if ((Input::isKeyPressed(SDL_SCANCODE_E) || Input::wasButtonPressed(5)) && !busy) {
			animationTimer = 0;
			animTime = Animation::Get("data/characters/dodgeRightFast.skanim")->duration;
			cancelTime = animTime * 0.9;
			dodgeTime = animTime * 0.6;
			color = Vector4(0, 0, 1, 1);
			state = 4;
			busy = true;
		}
		if ((Input::isKeyPressed(SDL_SCANCODE_A)|| Input::gamepads[0].hat == PAD_LEFT) && !busy) {
			yaw_delta -= dt * 80;
		}	
		if ((Input::isKeyPressed(SDL_SCANCODE_D) || Input::gamepads[0].hat == PAD_RIGHT) && !busy) {
			yaw_delta += dt * 80;
		}
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) || Input::isButtonPressed(1))
			move.z *= 3;
		if ((Input::Input::mouse_state & SDL_BUTTON_LMASK || Input::wasButtonPressed(0)) && !busy) {
			animationTimer = 0;
			animTime = Animation::Get("data/characters/leftPunch.skanim")->duration;
			cancelTime = animTime * 0.7;
			state = 1;
			busy = true;
		}
		if ((Input::Input::mouse_state == SDL_BUTTON_RMASK || Input::wasButtonPressed(3)) && !busy) {
			animationTimer = 0;
			animTime = Animation::Get("data/characters/rightPunch.skanim")->duration;
			cancelTime = animTime * 0.7;
			state = 2;
			busy = true;
		}

		switch (state) {
		case 3:
			yaw_delta += dt * 60; //when in combat
			move.x -= dt * 30;
			break;
		case 4:
			yaw_delta -= dt * 60; //when in combat
			move.x += dt * 30;
		}

		Matrix44 rot;
		rot.rotate(yaw*DEG2RAD, Vector3(0, 1, 0));
		move = rot * move;
		velocity = velocity + move * 3;
		position = position + velocity * dt;
		Matrix44 R;
		R.rotate(yaw * DEG2RAD, Vector3(0, 1, 0));
		front = R * Vector3(0, 0, -1);

		yaw += yaw_delta;
		
		float friction = 1.0 / (1.0 + (dt * 9));
		velocity = velocity * friction;
		
		
	
		//calculamos el centro de la esfera de colisión del player elevandola hasta la cintura
		Vector3 character_center = position + Vector3(0,4,0);
		Vector3 previous_pos = position;
		Vector3 coll, collnorm;
		bool collided = false;
		
		//para cada objecto de la escena...
		Character* charas= Game::instance->extractCharacter(0);
		std::vector<struct Entity> elems = Game::instance->extractElems();

		// if it doesn't collide with the enemy, check the environment
		if (Game::instance->elementType[charas->type].mesh->testSphereCollision(charas->model, character_center, 4, coll, collnorm) == true) { 
			collided = true;
		}
		else {
			for (int i = 1; i < elems.size();i++ ) {
				if (Game::instance->elementType[elems[i].type].mesh->testSphereCollision(elems[i].m, character_center, 4, coll, collnorm) == false) {
					continue;
				}
				collided = true;
				break;
			}
		}
		if(collided) {	//si no colisiona, pasamos al siguiente objeto

			//si la esfera está colisionando muevela a su posicion anterior alejandola del objeto
			Vector3 push_away = normalize(coll - character_center) * dt;
			position = previous_pos - push_away; //move to previous pos but a little bit further

			//cuidado con la Y, si nuestro juego es 2D la ponemos a 0
			position.y = 0;

			//reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
			velocity = reflect(velocity, collnorm) * 0.95;
		}
		
		

	}
	else if(enemy) {// enemy code
		
		if(!canAttack)
			aTimer += dt;
		if (aTimer>=aDelay && !canAttack) {
			aTimer = 0;
			canAttack = true;
		}
		Vector3 to_target = (target->model.getTranslation()) - model.getTranslation();
		Vector3 front = model.frontVector();
		to_target.normalize();
		front.normalize();

		float angle = acos(front.dot(to_target)) / DEG2RAD;

		if (health <= 0 && !dead) {//enter death state
			animationTimer = 0;
			animTime = Animation::Get("data/characters/death2.skanim")->duration;
			state = 6;
			cancelTime = animTime * 0.7;
			*busyChoice = true;
			dead = true;
		}
		else if (angle > 15 && !(*busyChoice)) {//rotate

			yaw_delta += angle * 50 * dt;
		}
		else if (model.getTranslation().distance(target->model.getTranslation()) > 10 && !(*busyChoice)) {//translate

			move = move + (to_target * 30 * dt);

		}
		else if(canAttack) {//decide attack
			float dice = rand() % 3;


			if (dice == 0 && !(*busyChoice)) {//right punch
				animationTimer = 0;
				animTime = Animation::Get("data/characters/rightPunch.skanim")->duration;
				cancelTime = animTime * 0.7;
				state = 2;
				*busyChoice = true;
			}
			else if (dice == 1 && !(*busyChoice)) {// left punch
				animationTimer = 0;
				animTime = Animation::Get("data/characters/leftPunch.skanim")->duration;
				cancelTime = animTime * 0.7;
				state = 1;
				*busyChoice = true;
			}
			else if (dice == 2 && !(*busyChoice)) {
				animationTimer = 0;
				animTime = Animation::Get("data/characters/castTwoHand.skanim")->duration;
				cancelTime = animTime * 0.45;
				state = 5;
				*busyChoice = true;
			}
		}

		Matrix44 rot;
		rot.rotate(yaw*DEG2RAD, Vector3(0, 1, 0));
		move = rot * move;
		velocity = velocity + move * 3;
		position = position + velocity * dt;
		Matrix44 R;
		R.rotate(yaw * DEG2RAD, Vector3(0, 1, 0));
		front = R * Vector3(0, 0, -1);



		yaw += yaw_delta;

		position.y = 0;


		float friction = 1.0 / (1.0 + (dt * 9));
		velocity = velocity * friction;

		//ENEMY COLLISION
		//calculamos el centro de la esfera de colisión del character elevandola hasta la cintura
		Vector3 character_center = position + Vector3(0, 6, 0);
		Vector3 previous_pos = position;
		Vector3 coll, collnorm;

		//para cada objecto de la escena...
		std::vector<struct Entity> elems = Game::instance->extractElems();

		

		for (int i = 1; i < elems.size(); i++) {
			if (Game::instance->elementType[elems[i].type].mesh->testSphereCollision(elems[i].m, character_center, 4, coll, collnorm) == false) {
				continue;//si no colisiona, pasamos al siguiente objeto

				//si la esfera está colisionando muevela a su posicion anterior alejandola del objeto
				Vector3 push_away = normalize(coll - character_center) * dt;
				position = previous_pos - push_away; //move to previous pos but a little bit further

				//cuidado con la Y, si nuestro juego es 2D la ponemos a 0
				position.y = 0;

				//reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
				velocity = reflect(velocity, collnorm) * 0.95;
				break;
			}


		}
	}
	updateMatrix();
	updateSkeleton(dt);
	
	


}

void Character::updateSkeleton(float dt)
{
	
	float t = Game::instance->time + id;
	float speed = velocity.length() * 0.1;
	float w = 0.0;
	Vector3 vel = velocity;
		
	if (velocity.length() > 0.01)
	{
		normalize(velocity); //vel in local space
		Matrix44 R;
		R.rotate(-yaw*DEG2RAD, Vector3(0, 1, 0));
		vel = R * vel;
	}

	Animation* anim = NULL;
	Animation* animB = NULL;
	

	switch (state) {
	case 0:
		if (speed < 0.01) //idle
		{
			anim = Animation::Get(idle_name);
			anim->assignTime(t);
			sk = anim->skeleton;
		}
		else if (speed < 1.0) //walk
		{
			anim = Animation::Get(idle_name);
			anim->assignTime(t);
			w = clamp(speed, 0, 1);
			animB = Animation::Get("data/characters/walking.skanim");
			if (vel.z < 0)
				animB->assignTime(t);
			else
				animB->assignTime(-t);
			blendSkeleton(&anim->skeleton, &animB->skeleton, w, &sk);
		}
		else //run
		{
			w = clamp(speed - 1.0, 0, 1);
			anim = Animation::Get("data/characters/walking.skanim");
			if (vel.z < 0)
				anim->assignTime(t);
			else
				anim->assignTime(-t);
			animB = Animation::Get("data/characters/running.skanim");
			if (vel.z < 0)
				animB->assignTime((t / anim->duration) * animB->duration);
			else
				animB->assignTime(-(t / anim->duration) * animB->duration);
			blendSkeleton(&anim->skeleton, &animB->skeleton, w, &sk);
		}
		break;
	case 1:
		anim = Animation::Get("data/characters/leftPunch.skanim");
		anim->assignTime(animationTimer);
		sk = anim->skeleton;
		break;
	case 2:
		anim = Animation::Get("data/characters/rightPunch.skanim");
		anim->assignTime(animationTimer);
		sk = anim->skeleton;
		break;
	case 3:
		anim = Animation::Get("data/characters/dodgeLeftFast.skanim");
		anim->assignTime(animationTimer);
		sk = anim->skeleton;
		break;
	case 4:
		anim = Animation::Get("data/characters/dodgeRightFast.skanim");
		anim->assignTime(animationTimer);
		sk = anim->skeleton;
		break;
	case 5:
		anim = Animation::Get("data/characters/castTwoHand.skanim");
		anim->assignTime(animationTimer);
		sk = anim->skeleton;
		break;
	case 6:
		anim = Animation::Get("data/characters/death2.skanim");
		anim->assignTime(animationTimer);
		sk = anim->skeleton;
		break;
	case 7:
		anim = Animation::Get("data/characters/reaction1.skanim");
		anim->assignTime(animationTimer);
		sk = anim->skeleton;
		break;

	}
	

	sk.computeFinalBoneMatrices( bone_matrices, Game::instance->elementType[type].mesh );
}

void Character::setControlled(bool status){
	controlled = status;

}

bool Character::isControlled() {
	return controlled;
}

bool Character::hit() {

	//calculamos el centro de la esfera de colisión del player elevandola hasta la cintura
	Matrix44 temp;
	temp.translate(model.getTranslation().x, model.getTranslation().y, model.getTranslation().z);

	Vector3 hitbox_center;
	Vector3 coll, collnorm;

	if (controlled) {
		temp.translate(model.frontVector().x * 5, model.frontVector().y * 5, model.frontVector().z * 5);


		hitbox_center = temp.getTranslation() + Vector3(0, 4, 0);
		
		//para cada objecto de la escena...
		Character* charas = Game::instance->extractCharacter(0);
		
		
		if (Game::instance->elementType[charas->type].mesh->testSphereCollision(charas->model, hitbox_center, 5, coll, collnorm) == true) {
			//std::cout << "HIT \n";
			Game::instance->playSound(0);
			charas->isVulnerable();

			charas->color = Vector4(10 + charas->color.x, 1, 1, 1);
			charas->health -= 1 + (stack2 * 1.5 - stack1*0.5);

			hitCooldown = false;
			return true;
			
		}
		Game::instance->playSound(1);
		return false;
	}
	else if(enemy) {//enemy
		switch (state) {
		case 1://left
			temp.translate(model.frontVector().x * 5, model.frontVector().y * 5, model.frontVector().z * 5);

			hitbox_center = temp.getTranslation() + Vector3(0, 4, 0);
			
			if (Game::instance->elementType[target->type].mesh->testSphereCollision(target->model, hitbox_center, 5, coll, collnorm) && !target->isDodging()) {
				//std::cout << "HIT THE PLAYER \n";
				Game::instance->playSound(0);
			
				target->color = Vector4(10 + target->color.x, 1, 1, 1);
				target->health -= 3;
				target->reduceStack();

				hitCooldown = false;
				return true;

			}
			Game::instance->playSound(1);
			return false;
			break;
		case 2:
			temp.translate(model.frontVector().x * 5, model.frontVector().y * 5, model.frontVector().z * 5);

			hitbox_center = temp.getTranslation() + Vector3(0, 4, 0);

			if (Game::instance->elementType[target->type].mesh->testSphereCollision(target->model, hitbox_center, 5, coll, collnorm) && !target->isDodging()) {
				//std::cout << "HIT THE PLAYER \n";
				Game::instance->playSound(0);

				target->color = Vector4(10 + target->color.x, 1, 1, 1);
				target->health -= 3;
				target->reduceStack();

				hitCooldown = false;
				return true;
			}
			Game::instance->playSound(1);
			return false;
			break;
		case 5:
			temp.translate(model.frontVector().x * 5, model.frontVector().y * 5, model.frontVector().z * 5);

			hitbox_center = temp.getTranslation() + Vector3(0, 4, 0);

			if (Game::instance->elementType[target->type].mesh->testSphereCollision(target->model, hitbox_center, 5, coll, collnorm) && !target->isDodging()) {
				//std::cout << "HIT THE PLAYER \n";
				Game::instance->playSound(0);

				target->color = Vector4(10 + target->color.x, 1, 1, 1);
				target->health -= 10;
				target->reduceStack();

				hitCooldown = false;
				return true;
			}
			Game::instance->playSound(1);
			return false;
			break;
		}
	}
}
bool Character::isDodging() {
	if (animationTimer < dodgeTime) {
		//std::cout << "dodged";
		fillStack();
		return true;
	}
	return false;
}

void Character::isVulnerable() {
	if (!canAttack) {
		state = 7;
		animationTimer = 0;
		animTime = Animation::Get("data/characters/reaction1.skanim")->duration;
		cancelTime = animTime * 0.7;
		canAttack = true;
		*busyChoice = true;
	}
}

void Character::reset() {
	state = 0;
	health = 100;
	stack1 = 0;
	stack2 = 0;
	dead = false;
	busy = false;
	model.setIdentity();
	position = initPos;
	yaw = initRot;
}

void Character::reduceStack() {
	if (stack1 > 0)
		stack1--;
	if (stack2 > 0)
		stack2--;
}

void Character::fillStack() {
	if (stack1 < 3)
		stack1++;
	if (stack2 < 3)
		stack2++;
}



