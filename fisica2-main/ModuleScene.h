#pragma once
#include "Module.h"
#include "p2Point.h"
#include "Globals.h"


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
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update();
	bool CleanUp();

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact);
	void LoseLife();
	void ManageInputs();
	void GameOver();

public:
	PhsyBody* ball;
	PhysBody* launcher;
	PhysBody* flipper;
	PhysBody* death_trigger;

	b2MouseJoint* mouse_joint;

	SDL_Texture* ball_texture;
	SDL_Texture* flipper_texture;
	SDL_Texture* launcher_texture;
	SDL_Texture* background_texture;
	SDL_Texture* game_over_texture;
	SDL_Texture* start_menu_texture;

	int lives = 3;

	char* background_music;

	GameState game_state = START_MENU;
};