#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Module.h"
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
		body->listener = listener;
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
		: PhysicEntity(physics->CreateCircle(_x, _y, 13), _listener)
		, texture(_texture)
	{
		isInsideTheGame = true;
		hasBeenEjected = false;
		posblock = true;
	}

	void Launch()
	{
		if (IsKeyPressed(KEY_SPACE))
		{
			posblock = false;
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
		posblock = true;
	}


	void EnsureImInside()
	{
		b2Vec2 position = this->body->body->GetPosition();
		if (position.x >= 0 && position.x <= PIXEL_TO_METERS(SCREEN_WIDTH) &&
			position.y >= 0 && position.y <= PIXEL_TO_METERS(SCREEN_HEIGHT) + 10)
		{
			isInsideTheGame = true;
		}
		else
		{
			isInsideTheGame = false;
			HandleBallOut();
		}

	}

	void Circle::HandleBallOut()
	{

		ModuleGame* gameModule = dynamic_cast<ModuleGame*>(listener);
		if (gameModule != nullptr) {
			gameModule->LoseLife();
		}
		Die();
	}

	
	void Update() override
	{
		body->isBall = true;

		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
		if (this->body->body->GetPosition().x >= PIXEL_TO_METERS(603))
		{
			hasBeenEjected = false;
		}
		else {
			hasBeenEjected = true;
		}
		if (!hasBeenEjected) Launch();
		EnsureImInside();
		if (!isInsideTheGame)
		{
			Die();
		}

		if (posblock)
		{
			b2Vec2 initialPos = { (float)PIXEL_TO_METERS(625), (float)PIXEL_TO_METERS(630) };
			this->body->body->SetLinearVelocity(b2Vec2{ 0, 0 });
			this->body->body->SetAngularVelocity(0);
			this->body->body->SetTransform(initialPos, 0);
		}
	}


public:

	Texture2D texture;
	bool hasBeenEjected;
	bool isInsideTheGame;
	bool posblock;
};

class Frutica : public PhysicEntity
{
public:
	Frutica(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, ColliderType _type)
		: PhysicEntity(physics->CreateRectangleSensor(_x, _y, SCREEN_WIDTH - 20, SCREEN_WIDTH - 20), _listener)
		, texture(_texture)
	{
		
	}

	
	void Update() override
	{
		this->body->isFrutica = true;

		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
		
	}

public:

	Texture2D texture;
	
};
class Booster : public PhysicEntity
{
public:
	Booster(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, ColliderType _type, bool what)
		: PhysicEntity(physics->CreateRectangleSensor(_x, _y, 20, 20), _listener)
		, texture(_texture)
	{
		this->listener = _listener;
		flag = what;
	}


	void Update() override
	{
		this->body->isFrutica = true;

		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		if (flag)
		{
			DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
		}
		else if (!flag)
		{
			DrawTexturePro(texture, source, dest, origin, -rotation, WHITE);
		}


	}
	void Boost(PhysBody* BodyB)
	{
		BodyB->body->ApplyLinearImpulseToCenter({ 0,-100 }, true);
	}

public:

	Texture2D texture;
	bool flag;
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

	static constexpr int LeftBigCollision[84] = {
		201, 387,
		195, 391,
		190, 399,
		189, 407,
		194, 418,
		202, 424,
		214, 432,
		207, 436,
		197, 444,
		185, 456,
		167, 473,
		153, 488,
		143, 494,
		134, 490,
		111, 469,
		98, 453,
		98, 442,
		99, 430,
		89, 416,
		89, 407,
		91, 399,
		97, 393,
		99, 385,
		94, 380,
		89, 373,
		88, 363,
		92, 354,
		97, 349,
		99, 342,
		90, 335,
		90, 323,
		91, 312,
		98, 303,
		98, 294,
		99, 283,
		106, 278,
		116, 282,
		126, 302,
		134, 328,
		146, 350,
		173, 368,
		212, 383,

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

	MapColl(ModulePhysics* physics, int _x, int _y, Module* _listener, COLLISIONS type):
			PhysicEntity(physics->CreateChain(0, 0,collision, type_, GetCollider_andSize(type)), _listener)
	{
		if (type_ == ColliderType::BUMPER || type_ == ColliderType::BUMPER_STIKS)
		{
			body->body->GetFixtureList()->SetRestitution(1);
		}
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
			type_ = ColliderType::WALL;
			return 16;
			break;
		case COLLISIONS::RIGHT_DOWNER:
			collision = RightDawner;
			type_ = ColliderType::WALL;
			return 16;
			break;
		case COLLISIONS::LEFT_BOUNCER:
			collision = LeftBouncer;
			type_ = ColliderType::BUMPER;
			return 12;
			break;
		case COLLISIONS::RIGHT_BOUNCER:
			collision = RightBouncer;
			type_ = ColliderType::BUMPER;
			return 10;
			break;
		case COLLISIONS::LEFT_STIK:
			collision = LeftStik;
			type_ = ColliderType::BUMPER_STIKS;
			return 12;
			break;
		case COLLISIONS::MIDLE_STIK:
			collision = MiddleStik;
			type_ = ColliderType::BUMPER_STIKS;
			return 12;
			break;
		case COLLISIONS::RIGHT_STIK:
			collision = RightStik;
			type_ = ColliderType::BUMPER_STIKS;
			return 12;
			break;
		case COLLISIONS::LEFT_WING:
			collision = LeftBigCollision;
			type_ = ColliderType::WALL;
			return 84;
			break;
		case COLLISIONS::LEFT_OBSTACLE:
			collision = LeftTopBigCollision;
			type_ = ColliderType::WALL;
			return 26;
			break;
		case COLLISIONS::RIGHT_OBSTACLE:
			collision = RightBigCollision;
			type_ = ColliderType::WALL;
			return 30;
			break;
		case COLLISIONS::LEFT_BALL:
			collision = BallLeft;
			type_ = ColliderType::BUMPER_BALLS;
			return 20;
			break;
		case COLLISIONS::MIDLE_BALL:
			collision = BallMiddle;
			type_ = ColliderType::BUMPER_BALLS;
			return 22;
			break;
		case COLLISIONS::RIGHT_BALL:
			collision = BallRight;
			type_ = ColliderType::BUMPER_BALLS;
			return 26;
			break;
		case COLLISIONS::MAIN_MAP:
			collision = MainMap;
			type_ = ColliderType::WALL;
			return 110;
			break;
		default:
			break;
		}
	}

private:
	const int* collision;
	ColliderType type_;
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
	Module* App;
	Sound plunger_fx = LoadSound("Assets/sfxandsong/Ball release.wav");
};

class Object : public PhysicEntity
{
public:
	Object(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, ColliderType _type)
		: PhysicEntity(physics->CreateCircle(_x, _y, 16), _listener) 
		, texture(_texture)
		, type(_type)
	{
		isDestroyed = false;
	}



	void Update() override
	{
		if (isDestroyed) return;  

		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;

		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}


	void Destroy()
	{
		isDestroyed = true;
		body->body->GetWorld()->DestroyBody(body->body);  // Elimina el cuerpo físico del mundo
	}

public:
	Texture2D texture;
	ColliderType type;
	bool isDestroyed;
	int score = 0;
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
	PointBoard = LoadTexture("Assets/MapComponents/PointBoard.png");
	boosterR = LoadTexture("Assets/MapComponents/boosterR.png");
	boosterL = LoadTexture("Assets/MapComponents/boosterL.png");
	PointBubble_2 = LoadTexture("Assets/MapComponents/PointsExplosion_2digits.png");
	PointBubble_3 = LoadTexture("Assets/MapComponents/PointsExplosion_3digits.png");
	cora1=LoadTexture("Assets/MapComponents/cora1.png");
	cora2 = LoadTexture("Assets/MapComponents/cora2.png");
	cora3 = LoadTexture("Assets/MapComponents/cora3.png");

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
	//entities.emplace_back(new Booster(App->physics, 80,485 , this,boosterL,ColliderType::BOOSTER,true));
	//entities.emplace_back(new Booster(App->physics, 550 , 485, this,boosterR, ColliderType::BOOSTER,false));
	

	/*entities.emplace_back(new Booster(App->physics, 42, 441, this, boosterL, ColliderType::FRUIT2, true));
	entities.emplace_back(new Booster(App->physics, 66, 485, this, boosterR,ColliderType::FRUIT2, false));*/
	//ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, ColliderType _type, bool what


	Texture2D flipperTexture = LoadTexture("Assets/MapComponents/Flipper.png");
	/*entities.emplace_back(new Flipper(App->physics, PIXEL_TO_METERS(210), PIXEL_TO_METERS(765), true, this, flipperTexture));
	entities.emplace_back(new Flipper(App->physics, PIXEL_TO_METERS(315), PIXEL_TO_METERS(765), false, this, flipperTexture));*/

	// Load music
	background_music= LoadMusicStream("Assets/sfxandsong/32 Nightmaren.ogg");
	SetMusicVolume(background_music, 0.5);
	PlayMusicStream(background_music);

	// Load FX
	bonus_fx = LoadSound("Assets/sfxandsong/bonus.wav");
	flipper_fx = App->audio->LoadFx("Assets/sfxandsong/Flipper 1.wav");
	bumper_fx = LoadSound("Assets/sfxandsong/Bumper9.wav");
	plunger_fx = LoadSound("Assets/sfxandsong/Ball release.wav");
	game_over_fx = LoadSound("Assets/sfxandsong/game_over.wav");
	start_fx = LoadSound("Assets/sfxandsong/Flipper 1.wav");
	launch_fx = LoadSound("Assets/sfxandsong/launch.wav");
	BellRing = LoadSound("Assets/sfxandsong/Bell ring1.wav");


	SCORE.Initialise("Assets/MapComponents/Points.png", '0', 36);

	//Load mierdas para conseguir puntos

	fruit1 = LoadTexture("Assets/MapComponents/fruit1.png");
	fruit2 = LoadTexture("Assets/MapComponents/fruit2.png");
	fruit3 = LoadTexture("Assets/MapComponents/fruit3.png");

	//Crear Objetos

/*	entities.emplace_back(new Object(App->physics, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, this, fruit1, ColliderType::FRUIT1));
	entities.emplace_back(new Object(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, this, fruit2, ColliderType::FRUIT2));
	entities.emplace_back(new Object(App->physics, SCREEN_WIDTH / 2 + 100, SCREEN_HEIGHT / 2, this, fruit3, ColliderType::FRUIT3));


	entities.emplace_back(new Frutica(App->physics, 200, 200, this, fruit1, ColliderType::FRUIT1));
	*/
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
	UpdateMusicStream(background_music);
	//DrawBG
	DrawTexture(BG, 0, 0, WHITE);
	
	//Score Managing
	DrawTexture(PointBoard, (SCREEN_WIDTH / 2) - (PointBoard.width / 2), 0, WHITE);
	SCORE.Draw(200, 3, std::to_string(score), WHITE);

	if (vidas == 3) {

		DrawTexture(cora3, 350,7,WHITE);


	}
	else if (vidas == 2) {

		DrawTexture(cora2, 350, 7, WHITE);
	}
	else if (vidas == 1) {

		DrawTexture(cora1, 350, 7, WHITE);
	}



	if (showBubble == true && BubbleTime.ReadSec() <0.2)
	{
		if (points < 100)
		{
			DrawTexture(PointBubble_2, METERS_TO_PIXELS(bubblePos.x - 40), METERS_TO_PIXELS(bubblePos.y - 40), WHITE);
			SCORE.Draw(METERS_TO_PIXELS(bubblePos.x - 25), METERS_TO_PIXELS(bubblePos.y - 10), std::to_string(points), WHITE);
		}
		else
		{
			DrawTexture(PointBubble_3, METERS_TO_PIXELS(bubblePos.x - 40), METERS_TO_PIXELS(bubblePos.y - 40), WHITE);
			SCORE.Draw(METERS_TO_PIXELS(bubblePos.x), METERS_TO_PIXELS(bubblePos.y - 10), std::to_string(points), WHITE);
		}
		
	}
	else
	{
		showBubble=false;
	}
	
	if (game_state == GameState::START_MENU)
	{
		//Draw start menu

		DrawText(TextFormat("Press Enter to Start"), 80, (SCREEN_HEIGHT/3)*2, 40, DARKBLUE);
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
		DrawText("Game Over", 175, 450, 75, WHITE);
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
	if (ballsInGame == true && bodyB != nullptr && bodyA != nullptr)
	{

		if (bodyA->type == ColliderType::BUMPER && bodyB->type == ColliderType::BALL)
		{
			if (showBubble == false)
			{
				bubblePos = bodyB->body->GetPosition();
				points = 100;
				score += points;
				BubbleTime.Start();
				PlaySound(BellRing);
			}
			showBubble = true;
		}
		if (bodyA->type == ColliderType::BUMPER_STIKS && bodyB->type == ColliderType::BALL)
		{
			if (showBubble == false)
			{
				bubblePos = bodyB->body->GetPosition();
				points = 50;
				score += points;
				BubbleTime.Start();
				PlaySound(BellRing);
			}
			showBubble = true;
		}
		if (bodyA->type == ColliderType::BUMPER_BALLS && bodyB->type == ColliderType::BALL)
		{
			if (showBubble == false)
			{
				bubblePos = bodyB->body->GetPosition();
				points = 80;
				score += points;
				BubbleTime.Start();
				PlaySound(BellRing);
			}
			showBubble = true;
		}

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
			PlaySound(plunger_fx);
		}
		if (IsKeyPressed(KEY_A)) {
			App->audio->PlayFx(flipper_fx);
			// Increase the force gradually, but don't exceed maxForce
			leftFlipperForce += forceIncrement;
			if (leftFlipperForce > maxForce) leftFlipperForce = maxForce;
			// Apply the increased force to the left flipper
			App->physics->flipperL->body->ApplyForceToCenter(b2Vec2(0, -maxForce), true);
			App->physics->flipperLa->body->ApplyForceToCenter(b2Vec2(0, -maxForce), true);
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
	PlaySound(game_over_fx);
	game_state = GameState::GAME_OVER;
}

void ModuleGame::Restart()
{
	//hacer que desaparezca la pelota y aparezca en otra posicion, resetear los objetos que den puntos y por ultimo cambiar el game state a jugar
	
	game_state = GameState::PLAYING;


}

