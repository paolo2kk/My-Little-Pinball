#pragma once

#include "Module.h"
#include "Globals.h"

#include "box2d\box2d.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -20.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum ColliderType
{
	BALL,
	DEATH,
	SCORE
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
	PhysBody* CreateRectangle(int x, int y, int width, int height);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height);
	void Flippers();
	void RenderFlippers();
	PhysBody* CreateChain(int x, int y, const int* points, int size);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	b2World* world;
	b2Body* ground;


	
	int flipperWidth = 80;
	int flipperHeight = 20;
	PhysBody* flipperL;
	PhysBody* flipperR;
	PhysBody* flipperLAnch;
	PhysBody* flipperRAnch;

	float restitution = 0.4f;
	Texture2D flipperLTexture;
	Texture2D flipperRTexture;
private:

	bool debug;
	b2MouseJoint* mouse_joint;
};