#pragma once

#include "Module.h"
#include "Globals.h"

#include "box2d\box2d.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -15

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum ColliderType
{
	BALL,
	DEATH,
	FRUIT1,
	FRUIT2,
	FRUIT3,
	BUMPER,
	WALL,
	BUMPER_BALLS,
	BUMPER_STIKS,
	BOOSTER

};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	//void GetPosition(int& x, int& y) const;
	void GetPhysicPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	ColliderType type;
	Module* listener;
	bool isBall;
	bool isFrutica;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius);
	PhysBody* CreateCircleSensor(int x, int y, int radius);
	PhysBody* CreateStaticCircle(int x, int y, int radius);
	PhysBody* CreateRectangle(int x, int y, int width, int height);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height);
	void Flippers();
	void RenderFlippers();
	PhysBody* CreateChain(int x, int y, const int* points, ColliderType type_, int size);
	PhysBody* CreateCircleNew(int x, int y, int radius, b2BodyType static_body);
	PhysBody* CreateBumper(const int* points, int pointCount, int x, int y);
	//PhysBody* CreateMiniBumper(const int* points, int pointCount, int x, int y);
	//PhysBody* CreateBumperChain(int x, int y, const int* points, int size);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	b2World* world;
	b2Body* ground;


	
	int flipperWidth = 92;
	int flipperHeight = 23;
	PhysBody* flipperL;
	PhysBody* flipperR;
	PhysBody* flipperLAnch;
	PhysBody* flipperRAnch;

	PhysBody* flipperLa;
	PhysBody* flipperLAncha;


	PhysBody* leftBumper;
	PhysBody* rightBumper;
	PhysBody* topBumper;
	PhysBody* DownLBumper;
	PhysBody* DownRBumper;
	PhysBody* mini1Bumper;
	PhysBody* mini2Bumper;
	PhysBody* mini12Bumper;
	PhysBody* mini21Bumper;


	float restitution = 0.4f;
	Texture2D flipperLTexture;
	Texture2D flipperRTexture;
	
private:

	bool debug;
	b2MouseJoint* mouse_joint;
};