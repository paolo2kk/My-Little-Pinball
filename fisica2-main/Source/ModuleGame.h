#pragma once

#include "Globals.h"
#include "Module.h"
#include "ModulePhysics.h"
#include "ModuleAudio.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>


class PhysBody;
class PhysicEntity;

enum class GameState
{
	PLAYING,
	RESTART,
	GAME_OVER,
	PAUSED,
	START_MENU

};

class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void LoseLife();
	void ManageInputs();
	void GameOver();
	void Restart();

public:

	std::vector<PhysicEntity*> entities;

	PhysBody* sensor;
	bool sensed;

	Texture2D circle;
	Texture2D box;
	Texture2D rick;
	Texture2D background;
	Texture2D flipperL;
	Texture2D flipperR;

	uint32 bonus_fx;
	uint32 flipper_fx;
	uint32 game_over_fx;
	uint32 start_fx;
	uint32 background_music;
	uint32 launch_fx;
	uint32 bumper_fx;
	uint32 plunger_fx;

	vec2<int> ray;
	bool ray_on;

	PhysBody* death_trigger;

	ColliderType type;

	GameState game_state = GameState::START_MENU;

	int vidas = 3;

	float leftFlipperForce = 100.0f;
	float rightFlipperForce = 100.0f;
	float maxForce = 700.0f;       // Set a maximum limit for the force
	float forceIncrement = 10.0f;   // Increment rate for holding the key
	float initialForce = 100.0f;    // Initial force when the key is first pressed
};



