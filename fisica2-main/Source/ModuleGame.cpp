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
		: PhysicEntity(physics->CreateCircle(_x, _y, 25), _listener)
		, texture(_texture)
	{
		isInsideTheGame = true;
		hasBeenEjected = false;
	}

	void Launch()
	{
		if (IsKeyPressed(KEY_A))
		{
			
			hasBeenEjected = true;
			this->body->body->ApplyForceToCenter(b2Vec2{ 100, -2500 }, false);
		}
	}

	void Die()
	{
	}

	void EnsureImInside()
	{
		if (this->body->body->GetPosition().x >= 0 && this->body->body->GetPosition().x <= SCREEN_WIDTH &&
			this->body->body->GetPosition().y >= 0 && this->body->body->GetPosition().y <= SCREEN_HEIGHT)
		{
			isInsideTheGame = true;
		}
		else {
			isInsideTheGame = false;
		}

		std::cout << isInsideTheGame;
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
class Rick : public PhysicEntity
{
public:
	static constexpr int table[110] = {
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

	Rick(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(GetMouseX(), GetMouseY(), table, 110), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};


//class Plunger : public PhysicEntity {
//public:
//	Plunger(PhysBody* ball, Module* listener)
//		: PhysicEntity(nullptr, listener), ball(ball) {}
//
//	void Update() override {
//		if (!ball) return; 
//
//		if (IsKeyDown(KEY_SPACE)) {
//			plungerForce += plungerChargeRate;
//			if (plungerForce > maxPlungerForce) {
//				plungerForce = maxPlungerForce;
//			}
//			isCharging = true;
//		}
//		else if (isCharging && IsKeyReleased(KEY_SPACE)) {
//			ball->body->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -plungerForce), true);
//
//			plungerForce = 0.0f;
//			isCharging = false;
//		}
//	}
//
//private:
//	PhysBody* ball;              
//	float plungerForce = 0.0f;   
//	const float maxPlungerForce = 800.0f; 
//	const float plungerChargeRate = 10.0f;
//	bool isCharging = false;
//};


class MapColl : public PhysicEntity
{
public:
	// Pivot 0, 0
	const int* LeftDawner = new const int[17] {
		103, 711,
			103, 792,
			191, 840,
			192, 820,
			212, 818,
			124, 772,
			124, 706,
			113, 700
		};

	const int* RightDawner = new const int[17] {
		394, 819,
			481, 773,
			482, 709,
			493, 700,
			503, 705,
			503, 791,
			412, 841,
			408, 825
		};

	const int* LeftBouncer = new const int[11] {
		168, 675,
			168, 720,
			210, 741,
			217, 731,
			178, 667
		};

	const int* RightBouncer = new const int[11] {
		427, 669,
			438, 677,
			438, 717,
			398, 742,
			389, 734
		};

	const int* LeftStik = new const int[13] {
		238, 470,
			228, 476,
			228, 540,
			238, 547,
			249, 540,
			249, 476
		};

	const int* MiddleStik = new const int[13] {
		304, 470,
			293, 477,
			293, 541,
			304, 547,
			314, 541,
			314, 477
		};

	const int* RightStik = new const int[13] {
		368, 470,
			358, 476,
			358, 540,
			369, 547,
			379, 540,
			379, 476
		};

	const int* LeftBigCollision = new const int[59] {
		98, 308,
			98, 284,
			110, 278,
			124, 289,
			134, 325,
			146, 352,
			164, 364,
			186, 371,
			202, 376,
			214, 385,
			200, 393,
			197, 403,
			202, 415,
			216, 419,
			217, 430,
			143, 497,
			98, 455,
			98, 438,
			97, 429,
			89, 423,
			89, 398,
			98, 391,
			98, 385,
			89, 382,
			90, 353,
			98, 348,
			99, 341,
			89, 337,
			89, 309
		};

	const int* LeftTopBigCollision = new const int[21] {
		138, 51,
			167, 51,
			203, 81,
			203, 117,
			103, 208,
			83, 194,
			83, 157,
			95, 116,
			108, 86,
			120, 70
		};

	const int* RightBigCollision = new const int[37] {
		394, 386,
			408, 396,
			411, 405,
			408, 414,
			401, 422,
			515, 500,
			527, 497,
			538, 486,
			538, 198,
			531, 189,
			513, 189,
			504, 198,
			503, 300,
			498, 318,
			493, 327,
			481, 338,
			468, 347,
			396, 380
		};

	//MapColl(ModulePhysics* physics, int _x, int _y, Module* _listener, int ident):
	//	 PhysicEntity(physics->CreateChain(GetMouseX(), GetMouseY(),GetCollType(), size_), _listener)
	//{

	//}

	//void Update() override
	//{
	//	int x, y;
	//	body->GetPhysicPosition(x, y);;
	//}

	//const int* GetCollType() {

	//	switch (ty)
	//	{
	//	case 0:
	//		size_ = 17;  // LeftDawner
	//		break;
	//	case 1:
	//		size_ = 17;  // RightDawner
	//		break;
	//	case 2:
	//		size_ = 11;  // LeftBouncer
	//		break;
	//	case 3:
	//		size_ = 11;  // RightBouncer
	//		break;
	//	case 4:
	//		size_ = 13;  // LeftStik
	//		break;
	//	case 5:
	//		size_ = 13;  // MiddleStik
	//		break;
	//	case 6:
	//		size_ = 13;  // RightStik
	//		break;
	//	case 7:
	//		size_ = 59;  // LeftBigCollision
	//		break;
	//	case 8:
	//		size_ = 21;  // LeftTopBigCollision
	//		break;
	//	case 9:
	//		size_ = 37;   // RightBigCollision
	//		break;
	//	default:
	//		break;
	//	}

	//	static std::vector<const int*> arrays = {
	//		LeftDawner,
	//		RightDawner,
	//		LeftBouncer,
	//		RightBouncer,
	//		LeftStik,
	//		MiddleStik,
	//		RightStik,
	//		LeftBigCollision,
	//		LeftTopBigCollision,
	//		RightBigCollision
	//	};

	//	if (ty < 10 && ty >= 0) {

	//		return arrays[ty];
	//	}
	//	else {
	//		return nullptr;
	//	}
	//}

private:
	int size_ = 0;
	int ty = 0;
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
	circle = LoadTexture("Assets/wheel.png");
	box = LoadTexture("Assets/crate.png");
	rick = LoadTexture("Assets/MapComponents/Whole Map.png");
	flipperL = LoadTexture("Assets/MapComponents/flipper.png");
	flipperR = LoadTexture("Assets/MapComponents/flipper.png");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	entities.emplace_back(new Rick(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, rick));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 0));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 1));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 2));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 3));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 4));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 5));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 6));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this,7));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 8));
	//entities.emplace_back(new MapColl(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, 9));




	entities.emplace_back(new Death(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, box, ColliderType::DEATH));

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

		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			entities.emplace_back(new Box(App->physics, GetMouseX(), GetMouseY(), this, box, ColliderType::DEATH));
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
		{
			entities.emplace_back(new Rick(App->physics, GetMouseX(), GetMouseY(), this, rick));
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

