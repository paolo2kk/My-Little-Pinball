#pragma once
#include "Module.h"
#include "p2Point.h"
#include "Globals.h"
#include "Application.h"
#include "box2d\box2d.h"


class PhysBody;
class b2MouseJoint;

#define START_POSITION { PIXEL_TO_METERS(449), PIXEL_TO_METERS(850) }

enum GameState
{
	PLAYING,
	RESTART,
	GAME_OVER,
	START_MENU,

};

enum FX
{
	FX_FLIPPER,
	FX_GAME_OVER,
	FX_LAUNCHER,
	FX_START,
};

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update();
	bool CleanUp();

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact);
	//crear más funciones si hace falta para los objetos que den puntos
	void LoseLife();
	void ManageInputs();
	void GameOver();

public:
	PhysBody* ball;
	PhysBody* launcher;
	PhysBody* flipper;
	PhysBody* death_trigger;

	ColliderType type;

	b2MouseJoint* mouse_joint;

	//poner mierdas para conseguir puntos

	int lives = 3;

	bool background_music;
	uint fx[4];

	GameState game_state = START_MENU;
};