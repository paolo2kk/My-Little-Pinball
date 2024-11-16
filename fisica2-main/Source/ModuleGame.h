#pragma once

#include "Globals.h"
#include "Module.h"
#include "ModulePhysics.h"
#include "ModuleAudio.h"

#include "Text.h"
#include "Timer.h"
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

enum class COLLISIONS
{
	LEFT_DOWNER,
	RIGHT_DOWNER,
	LEFT_BOUNCER,
	RIGHT_BOUNCER,
	LEFT_STIK,
	MIDLE_STIK,
	RIGHT_STIK,
	LEFT_WING,
	LEFT_OBSTACLE,
	RIGHT_OBSTACLE,
	LEFT_BALL,
	MIDLE_BALL,
	RIGHT_BALL,
	MAIN_MAP
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

	void UpdateScore();


public:

	std::vector<PhysicEntity*> entities;

	PhysBody* sensor;
	bool sensed;

	Texture2D circle;
	Texture2D box;
	Texture2D BG;
	Texture2D background;
	Texture2D flipperL;
	Texture2D flipperR;
	Texture2D bumperL;
	Texture2D bumperR;
	Texture2D PointBoard;
	Texture2D PointBubble_2;
	Texture2D PointBubble_3;

	Texture2D fruit1;
	Texture2D fruit2;
	Texture2D fruit3;
	Texture2D object1;
	Texture2D object2;
	Texture2D boosterL;
	Texture2D boosterR;

	uint32 bonus_fx;
	uint32 flipper_fx;
	uint32 game_over_fx;
	uint32 start_fx;
	Music background_music;
	uint32 launch_fx;
	uint32 bumper_fx;
	uint32 plunger_fx;
	uint32 BellRing;

	vec2<int> ray;
	bool ray_on;

	PhysBody* death_trigger;

	ColliderType type;
	Text SCORE;
	bool showBubble = false;
	int points;
	Timer BubbleTime;
	b2Vec2 bubblePos;

	GameState game_state = GameState::START_MENU;

	int vidas = 3;
	int score = 0;
	bool ballsInGame = false;

	float leftFlipperForce = 100.0f;
	float rightFlipperForce = 100.0f;
	float maxForce = 2400;       
	float forceIncrement = 10.0f;
	float initialForce = 100.0f; 

	PhysBody* ball;
};



