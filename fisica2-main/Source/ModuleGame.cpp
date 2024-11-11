#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include <iostream>

#ifdef NDEBUG
#define DEBUG_ONLY(X)
#else
#define DEBUG_ONLY(X) X
#endif


class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body)
		, listener(_listener)
	{

	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

protected:

	PhysBody* body;
	Module* listener;
};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, ColliderType _type)
		: PhysicEntity(physics->CreateCircle(_x, _y, 16), _listener)
		, texture(_texture)
	{
		isInsideTheGame = true;
		hasBeenEjected = false;
	}

	void Launch()
	{
		if (IsKeyPressed(KEY_SPACE))
		{
			isInsideTheGame = true;
			hasBeenEjected = true;
			this->body->body->ApplyForceToCenter(b2Vec2{ 100, -500 }, false);
		}
	}

	void Die()
	{
		b2Vec2 initialPos = { (float)PIXEL_TO_METERS(625), (float)PIXEL_TO_METERS(611) };
		this->body->body->SetTransform(initialPos, 0);
		this->body->body->SetLinearVelocity(b2Vec2{ 0, 0 });
		this->body->body->SetAngularVelocity(0);
		isInsideTheGame = false;
		hasBeenEjected = false;
	}

	void EnsureImInside()
	{
		b2Vec2 position = this->body->body->GetPosition();
		if (position.x >= 0 && position.x <= PIXEL_TO_METERS(SCREEN_WIDTH) &&
			position.y >= 0 && position.y <= PIXEL_TO_METERS(SCREEN_HEIGHT) + 100)
		{
			isInsideTheGame = true;
		}
		else
		{
			isInsideTheGame = false;
		}

	}
	
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);

		if (!hasBeenEjected) Launch();
		EnsureImInside();
		if (!isInsideTheGame)
		{
			Die();
		}
	}

public:

	Texture2D texture;
	bool hasBeenEjected;
	bool isInsideTheGame;
};

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, ColliderType type)
		: PhysicEntity(physics->CreateRectangle(_x, _y, 100, 50), _listener)
		, texture(_texture)
	{
	
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;

};
class Death : public PhysicEntity
{
public:
	Death(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, ColliderType type)
		: PhysicEntity(physics->CreateRectangleSensor(_x, _y, 100, 50), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;

};
class Flipper : public PhysicEntity
{
public:
	Flipper(ModulePhysics* physics, int _x, int _y, bool isLeft, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRectangle(_x, _y, _texture.width, _texture.height), _listener)
		, texture(_texture)
		, isLeft(isLeft)
	{
		b2Body* ground = physics->ground;

		b2RevoluteJointDef jointDef;
		jointDef.bodyA = ground;
		jointDef.bodyB = body->body;
		jointDef.localAnchorA.Set(_x, _y);
		jointDef.localAnchorB.Set(0, 0);
		jointDef.enableMotor = true;
		jointDef.enableLimit = true;

		jointDef.lowerAngle = -0.25f * b2_pi;
		jointDef.upperAngle = 0.0f;

		jointDef.maxMotorTorque = 2000.0f;
		jointDef.motorSpeed = isLeft ? -5.0f : 5.0f;

		flipperJoint = (b2RevoluteJoint*)physics->world->CreateJoint(&jointDef);


	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	void ControlFlipper(bool pressed)
	{
		if (pressed)
		{
			flipperJoint->SetMotorSpeed(isLeft ? 10.0f : -10.0f);
		}
		else
		{
			flipperJoint->SetMotorSpeed(isLeft ? -5.0f : 5.0f);
		}
	}

private:
	b2RevoluteJoint* flipperJoint;
	Texture2D texture;
	bool isLeft;
};



class MapColl : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int LeftDawner[16] = {
	123, 706,
	124, 773,
	209, 816,
	192, 820,
	191, 839,
	103, 789,
	103, 707,
	113, 701
	};

	static constexpr int RightDawner[16] = {
		481, 773,
		483, 707,
		492, 700,
		503, 705,
		504, 793,
		415, 839,
		412, 820,
		397, 816
	};

	static constexpr int LeftBouncer[12] = {
	170, 674,
	181, 668,
	218, 729,
	209, 741,
	169, 719,
	169, 693
	};

	static constexpr int RightBouncer[10] = {
		426, 667,
		438, 676,
		438, 719,
		399, 742,
		389, 733
	};

	static constexpr int LeftStik[12] = {
		248, 479,
		248, 540,
		239, 546,
		228, 540,
		228, 479,
		238, 471
	};

	static constexpr int MiddleStik[12] = {
		313, 477,
		313, 540,
		303, 546,
		293, 539,
		293, 477,
		303, 470
	};

	static constexpr int RightStik[12] = {
		379, 477,
		379, 539,
		369, 546,
		358, 540,
		358, 477,
		368, 470
	};

	static constexpr int LeftBigCollision[58] = {
		118, 476,
		99, 453,
		97, 440,
		101, 429,
		90, 422,
		90, 400,
		100, 391,
		98, 383,
		90, 372,
		90, 358,
		101, 346,
		98, 338,
		89, 328,
		89, 313,
		98, 305,
		99, 282,
		113, 279,
		124, 290,
		132, 321,
		142, 345,
		156, 360,
		177, 369,
		213, 384,
		201, 390,
		197, 401,
		200, 415,
		216, 419,
		216, 431,
		141, 496
	};

	static constexpr int LeftTopBigCollision[26] = {
		95, 205,
		88, 198,
		85, 182,
		85, 168,
		87, 150,
		95, 122,
		105, 95,
		119, 71,
		139, 51,
		167, 51,
		203, 84,
		202, 115,
		105, 208
	};

	static constexpr int RightBigCollision[30] = {
		539, 199,
		539, 486,
		530, 495,
		515, 499,
		400, 422,
		407, 411,
		410, 402,
		406, 392,
		396, 382,
		467, 348,
		487, 334,
		498, 320,
		503, 300,
		504, 197,
		523, 187
	};

	static constexpr int BallLeft[20] = {
	227, 211,
	237, 213,
	243, 220,
	243, 232,
	239, 240,
	230, 244,
	219, 243,
	211, 235,
	211, 223,
	217, 215
	};

	static constexpr int BallMiddle[22] = {
	315, 103,
	323, 104,
	327, 110,
	329, 117,
	327, 126,
	320, 132,
	309, 132,
	302, 125,
	300, 116,
	303, 108,
	308, 104
	};

	static constexpr int BallRight[26] = {
	400, 210,
	410, 212,
	416, 219,
	417, 228,
	416, 234,
	412, 241,
	402, 244,
	393, 243,
	388, 238,
	384, 231,
	384, 224,
	387, 216,
	393, 212
	};


	static constexpr int MainMap[110] = {
	489, 862,
	558, 818,
	558, 654,
	537, 633,
	536, 583,
	552, 567,
	569, 546,
	577, 524,
	580, 500,
	580, 129,
	575, 117,
	565, 106,
	555, 100,
	540, 104,
	489, 131,
	482, 126,
	484, 115,
	526, 73,
	535, 66,
	556, 65,
	575, 71,
	593, 86,
	605, 112,
	608, 126,
	607, 647,
	646, 648,
	647, 140,
	644, 98,
	639, 71,
	628, 48,
	611, 33,
	595, 20,
	580, 14,
	160, 15,
	137, 19,
	106, 39,
	82, 69,
	66, 108,
	56, 157,
	49, 207,
	48, 246,
	32, 269,
	26, 277,
	26, 437,
	36, 477,
	72, 516,
	72, 607,
	49, 641,
	50, 820,
	121, 863,
	0, 863,
	0, 0,
	647, 0,
	647, 863,
	490, 863
	};

	MapColl(ModulePhysics* physics, int _x, int _y, Module* _listener, COLLISIONS type ):
			PhysicEntity(physics->CreateChain(0, 0,collision, GetCollider_andSize(type)), _listener)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);;
	}

	int GetCollider_andSize(COLLISIONS ty) {

		switch (ty)
		{
		case COLLISIONS::LEFT_DOWNER:  
			collision = LeftDawner;
			return 16;
			break;
		case COLLISIONS::RIGHT_DOWNER:
			collision = RightDawner;
			return 16;
			break;
		case COLLISIONS::LEFT_BOUNCER:
			collision = LeftBouncer;
			return 12;
			break;
		case COLLISIONS::RIGHT_BOUNCER:
			collision = RightBouncer;
			return 10;
			break;
		case COLLISIONS::LEFT_STIK:
			collision = LeftStik;
			return 12;
			break;
		case COLLISIONS::MIDLE_STIK:
			collision = MiddleStik;
			return 12;
			break;
		case COLLISIONS::RIGHT_STIK:
			collision = RightStik;
			return 12;
			break;
		case COLLISIONS::LEFT_WING:
			collision = LeftBigCollision;
			return 58;
			break;
		case COLLISIONS::LEFT_OBSTACLE:
			collision = LeftTopBigCollision;
			return 26;
			break;
		case COLLISIONS::RIGHT_OBSTACLE:
			collision = RightBigCollision;
			return 30;
			break;
		case COLLISIONS::LEFT_BALL:
			collision = BallLeft;
			return 20;
			break;
		case COLLISIONS::MIDLE_BALL:
			collision = BallMiddle;
			return 22;
			break;
		case COLLISIONS::RIGHT_BALL:
			collision = BallRight;
			return 26;
			break;
		case COLLISIONS::MAIN_MAP:
			collision = MainMap;
			return 110;
			break;
		default:
			break;
		}
	}

private:
	const int* collision;
};

class Plunger : public PhysicEntity {
public:
	Plunger(PhysBody* ball, Module* listener)
		: PhysicEntity(nullptr, listener), ball(ball) {}

	void Update() override {
		if (!ball) return;  // Ensure ball is valid

		// Charge force while space is held down
		if (IsKeyDown(KEY_SPACE)) {
			plungerForce += plungerChargeRate;
			if (plungerForce > maxPlungerForce) {
				plungerForce = maxPlungerForce;
			}
			isCharging = true;
		}
		else if (isCharging && IsKeyReleased(KEY_SPACE)) {
			// Apply impulse to ball on release
			ball->body->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -plungerForce), true);

			// Reset the charge for next use
			plungerForce = 0.0f;
			isCharging = false;
		}
	}

private:
	PhysBody* ball;              // Reference to the existing ball
	float plungerForce = 0.0f;   // Current charge level for plunger
	const float maxPlungerForce = 800.0f; // Maximum impulse force
	const float plungerChargeRate = 10.0f; // Rate of charge increase
	bool isCharging = false;     // True while the plunger is charging
};

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
	sensed = false;

}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Load textures
	circle = LoadTexture("Assets/MapComponents/wheel.png");
	box = LoadTexture("Assets/crate.png");
	BG = LoadTexture("Assets/MapComponents/Whole Map.png");
	flipperL = LoadTexture("Assets/MapComponents/flipper.png");
	flipperR = LoadTexture("Assets/MapComponents/flipper.png");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::LEFT_DOWNER));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::RIGHT_DOWNER));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::LEFT_BOUNCER));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::RIGHT_BOUNCER));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::LEFT_STIK));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::MIDLE_STIK));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::RIGHT_STIK));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::LEFT_WING));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::LEFT_OBSTACLE));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::RIGHT_OBSTACLE));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::LEFT_BALL));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::MIDLE_BALL));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::RIGHT_BALL));
	entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, COLLISIONS::MAIN_MAP));





	Texture2D flipperTexture = LoadTexture("Assets/MapComponents/Flipper.png");
	/*entities.emplace_back(new Flipper(App->physics, PIXEL_TO_METERS(210), PIXEL_TO_METERS(765), true, this, flipperTexture));
	entities.emplace_back(new Flipper(App->physics, PIXEL_TO_METERS(315), PIXEL_TO_METERS(765), false, this, flipperTexture));*/

	// Load music
	background_music = App->audio->PlayMusic("Assets/music.ogg");

	// Load FX
	bonus_fx = App->audio->LoadFx("Assets/sfxandsong/bonus.wav");
	flipper_fx = App->audio->LoadFx("Assets/sfxandsong/Flipper 1.wav");
	bumper_fx = App->audio->LoadFx("Assets/sfxandsong/Bumper9.wav");
	plunger_fx = App->audio->LoadFx("Assets/sfxandsong/PlungerRusty.wav");
	game_over_fx = App->audio->LoadFx("Assets/sfxandsong/game_over.wav");
	start_fx = App->audio->LoadFx("Assets/sfxandsong/start.wav");
	launch_fx = App->audio->LoadFx("Assets/sfxandsong/launch.wav");


	//Load mierdas para conseguir puntos

	//Load pelotas bien

	//Load paredes

	//Crear death_trigger
	
	//Crear launcher



	return ret;


}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
	//DrawBG
	DrawTexture(BG, 0, 0, WHITE);

	if (game_state == GameState::START_MENU)
	{
		//Draw start menu
		DrawText(TextFormat("Press Enter to Start"), 40, 40, 20, BLACK);
	}

	if (game_state == GameState::PAUSED)
	{
		//Draw pause menu
		DrawText("Press Enter to resume", 10, 10, 20, WHITE);
		DrawText("Press Escape to go to start menu", 10, 30, 20, WHITE);
	}

	if (game_state == GameState::GAME_OVER)
	{
		//Draw game over menu
		DrawText("Game Over", 10, 10, 20, WHITE);
		DrawText("Press Enter to restart", 10, 30, 20, WHITE);
		DrawText("Press Escape to go to start menu", 10, 50, 20, WHITE);
	}

	if (game_state == GameState::RESTART)
	{
		Restart();
	}
	ManageInputs();

	if (IsKeyPressed(KEY_SPACE))
	{
		ray_on = !ray_on;
		ray.x = GetMouseX();
		ray.y = GetMouseY();
	}

	// Prepare for raycast ------------------------------------------------------

	vec2i mouse;
	mouse.x = GetMouseX();
	mouse.y = GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	vec2f normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------


	for (PhysicEntity* entity : entities)
	{
		entity->Update();
		if (ray_on)
		{
			int hit = entity->RayHit(ray, mouse, normal);
			if (hit >= 0)
			{
				ray_hit = hit;
			}
		}
	}


	// ray -----------------
	if (ray_on == true)
	{
		vec2f destination((float)(mouse.x - ray.x), (float)(mouse.y - ray.y));
		destination.Normalize();
		destination *= (float)ray_hit;

		DrawLine(ray.x, ray.y, (int)(ray.x + destination.x), (int)(ray.y + destination.y), RED);

		if (normal.x != 0.0f)
		{
			DrawLine((int)(ray.x + destination.x), (int)(ray.y + destination.y), (int)(ray.x + destination.x + normal.x * 25.0f), (int)(ray.y + destination.y + normal.y * 25.0f), Color{ 100, 255, 100, 255 });
		}
	}



	// Continue with other game update logic...




	/*for (PhysicEntity* entity : entities)
	{
		Flipper* flipper = dynamic_cast<Flipper*>(entity);
		if (flipper)
		{
			flipper->ControlFlipper(IsKeyDown(KEY_SPACE));
		}

		entity->Update();
	}*/
	return UPDATE_CONTINUE;



}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{

	App->audio->PlayFx(bonus_fx);

	/*
	int x, y;
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/

	if (bodyA->type == ColliderType::BALL && bodyB->type == ColliderType::DEATH)
	{
		//hacer que se delete la pelota
		
		LoseLife();
	}

}

void ModuleGame::LoseLife()
{
	//noBallsInGame = true;
	vidas--;
	if (vidas == 0)
	{
		GameOver();
	}
	else
	{
		game_state = GameState::RESTART;
	}
}

void ModuleGame::ManageInputs()
{

	if (game_state == GameState::START_MENU)
	{
		if (IsKeyPressed(KEY_ENTER))
		{
			game_state = GameState::PLAYING;
		}
	}

	if (game_state == GameState::PAUSED)
	{
		if (IsKeyPressed(KEY_ENTER))
		{
			game_state = GameState::PLAYING;
		}
		if (IsKeyPressed(KEY_ESCAPE))
		{
			game_state = GameState::START_MENU;
		}
	}

	if (game_state == GameState::PLAYING && game_state != GameState::GAME_OVER)
	{
		if (IsKeyDown(KEY_SPACE))
		{
			Flipper* flipper = dynamic_cast<Flipper*>(entities[1]);
			if (flipper)
			{
				flipper->ControlFlipper(IsKeyDown(KEY_SPACE));
			}
		}
		if (IsKeyPressed(KEY_A)) {
			App->audio->PlayFx(flipper_fx);
			// Increase the force gradually, but don't exceed maxForce
			leftFlipperForce += forceIncrement;
			if (leftFlipperForce > maxForce) leftFlipperForce = maxForce;

			// Apply the increased force to the left flipper
			App->physics->flipperL->body->ApplyForceToCenter(b2Vec2(0, -maxForce), true);
		}
		else {
			// Reset the force when the key is released
			leftFlipperForce = initialForce;
		}

		// Check if the D key is held down for the right flipper
		if (IsKeyPressed(KEY_D)) {
			App->audio->PlayFx(flipper_fx);
			// Increase the force gradually, but don't exceed maxForce
			rightFlipperForce += forceIncrement;
			if (rightFlipperForce > maxForce) rightFlipperForce = maxForce;

			// Apply the increased force to the right flipper
			App->physics->flipperR->body->ApplyForceToCenter(b2Vec2(0, -maxForce), true);
		}
		else {
			// Reset the force when the key is released
			rightFlipperForce = initialForce;
		}
		if (/*IsMouseButtonPressed(MOUSE_BUTTON_LEFT)  && */!ballsInGame)
		{
			ballsInGame = true;
			entities.emplace_back(new Circle(App->physics, 625, 611, this, circle, ColliderType::BALL));
		
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) )
		{
			ballsInGame = true;
			entities.emplace_back(new Circle(App->physics, GetMouseX(), GetMouseY(), this, circle, ColliderType::BALL));

		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			entities.emplace_back(new Box(App->physics, GetMouseX(), GetMouseY(), this, box, ColliderType::DEATH));
		}


		if (IsKeyDown(KEY_ESCAPE))
		{
			game_state = GameState::PAUSED;
		}
	}

	if (game_state == GameState::GAME_OVER)
	{
		if (IsKeyPressed(KEY_ENTER))
		{
			game_state = GameState::PLAYING;
		}
		if (IsKeyPressed(KEY_ESCAPE))
		{
			game_state = GameState::START_MENU;
		}
	}

}
//github no va
void ModuleGame::GameOver()
{
	App->audio->PlayFx(game_over_fx);
	game_state = GameState::GAME_OVER;
}

void ModuleGame::Restart()
{
	//hacer que desaparezca la pelota y aparezca en otra posicion, resetear los objetos que den puntos y por ultimo cambiar el game state a jugar
	
	game_state = GameState::PLAYING;


}

