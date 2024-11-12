#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"

#include "p2Point.h"

#include <math.h>

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");
	flipperLTexture = LoadTexture("Assets/MapComponents/Flipper.png");
	flipperRTexture = LoadTexture("Assets/MapComponents/Flipper.png");
	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	Flippers();

	
	int bumperPoints[] = { -20, -20, 20, -20, 20, 20, -20, 20 };
	int bumperPointCount = 8;  
																					//change aqui
	
	leftBumper = CreateBumper(bumperPoints, bumperPointCount, 150, 600); 
	rightBumper = CreateBumper(bumperPoints, bumperPointCount, 500, 600);
	
	
	return true;
}

update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureA()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);
	pbody->width = width;
	pbody->height = height;

	return pbody;
}
void ModulePhysics::Flippers()
{
	flipperL = CreateRectangle(212, 835, flipperWidth, flipperHeight);
	flipperLAnch = CreateCircle(212, 835, 2);
	flipperLAnch->body->SetType(b2_staticBody);
	b2RevoluteJointDef flipperLJointDef;

	flipperLJointDef.bodyA = flipperL->body;
	flipperLJointDef.bodyB = flipperLAnch->body;
	flipperLJointDef.localAnchorA.Set(PIXEL_TO_METERS(-30), 0);
	flipperLJointDef.localAnchorB.Set(0, 0);
	flipperLJointDef.enableLimit = true;
	flipperLJointDef.lowerAngle = -30 * DEGTORAD;
	flipperLJointDef.upperAngle = 30 * DEGTORAD;
	b2RevoluteJoint* leftFlipperJoint = (b2RevoluteJoint*)world->CreateJoint(&flipperLJointDef);

	flipperR = CreateRectangle(395, 835, flipperWidth, flipperHeight);
	flipperRAnch = CreateCircle(395, 835, 2);
	flipperRAnch->body->SetType(b2_staticBody);
	b2RevoluteJointDef flipperRJointDef;

	flipperRJointDef.bodyA = flipperR->body;
	flipperRJointDef.bodyB = flipperRAnch->body;
	flipperRJointDef.localAnchorA.Set(PIXEL_TO_METERS(30), 0);
	flipperRJointDef.localAnchorB.Set(0, 0);
	flipperRJointDef.enableLimit = true;
	flipperRJointDef.lowerAngle = -30 * DEGTORAD;
	flipperRJointDef.upperAngle = 30 * DEGTORAD;
	b2RevoluteJoint* rightFlipperJoint = (b2RevoluteJoint*)world->CreateJoint(&flipperRJointDef);

}
void ModulePhysics::RenderFlippers()
{
	int xL, yL;
	flipperL->GetPhysicPosition(xL, yL);
	DrawTexturePro(
		flipperLTexture,
		Rectangle{ 0, 0, -(float)flipperLTexture.width, (float)flipperLTexture.height },
		Rectangle{ (float)xL, (float)yL, (float)flipperLTexture.width, (float)flipperLTexture.height },
		Vector2{ (float)flipperLTexture.width / 2.0f, (float)flipperLTexture.height / 2.0f },
		flipperL->GetRotation() * RAD2DEG,
		WHITE
	);

	int xR, yR;
	flipperR->GetPhysicPosition(xR, yR);
	DrawTexturePro(
		flipperRTexture,
		Rectangle{ 0, 0, (float)flipperRTexture.width, (float)flipperRTexture.height },
		Rectangle{ (float)xR, (float)yR, (float)flipperRTexture.width, (float)flipperRTexture.height },
		Vector2{ (float)flipperRTexture.width / 2.0f, (float)flipperRTexture.height / 2.0f },
		flipperR->GetRotation() * RAD2DEG,
		WHITE
	);
}
PhysBody* ModulePhysics::CreateChain(int x, int y, const int* points, int size)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(int i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	int size_ = size / 2;

	shape.CreateLoop(p, size_);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateBumper(const int* points, int pointCount, int x, int y) {
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* bumperBody = world->CreateBody(&bodyDef);

	b2PolygonShape bumperShape;
	b2Vec2* vertices = new b2Vec2[pointCount / 2];
	for (int i = 0; i < pointCount / 2; ++i) {
		vertices[i].Set(PIXEL_TO_METERS(points[i * 2]), PIXEL_TO_METERS(points[i * 2 + 1]));
	}
	bumperShape.Set(vertices, pointCount / 2);
	delete[] vertices;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &bumperShape;
	fixtureDef.density = 1.0f;
	fixtureDef.restitution = 1.2f;
	bumperBody->CreateFixture(&fixtureDef);

	PhysBody* pBumper = new PhysBody();
	pBumper->body = bumperBody;
	bumperBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(pBumper);
	return pBumper;
}

// 
update_status ModulePhysics::PostUpdate()
{
	RenderFlippers();

	if(IsKeyPressed(KEY_F1))
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	int x, y;
	leftBumper->GetPhysicPosition(x, y); // Bumper aqui
	DrawCircle(x, y, 20, RED);  

	rightBumper->GetPhysicPosition(x, y); // bumper aqui change per lo que hi ha a modulegame
	DrawCircle(x, y, 20, BLUE);
	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					
					DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{0, 0, 0, 128});
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->m_count;
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->m_vertices[i]);
						if(i > 0)
							DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->m_vertices[0]);
					DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), BLUE);
				}
				break;
			}

			// TODO 1: If mouse button 1 is pressed ...
			// App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN
			// test if the current body contains mouse position
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	// TODO 2: If a body was selected, create a mouse joint
	// using mouse_joint class property


	// TODO 3: If the player keeps pressing the mouse button, update
	// target position and draw a red line between both anchor points

	// TODO 4: If the player releases the mouse button, destroy the joint

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

//void PhysBody::GetPosition(int& x, int &y) const
//{
//	b2Vec2 pos = body->GetPosition();
//	x = METERS_TO_PIXELS(pos.x) - (width);
//	y = METERS_TO_PIXELS(pos.y) - (height);
//}

void PhysBody::GetPhysicPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x);
	y = METERS_TO_PIXELS(pos.y);
}

float PhysBody::GetRotation() const
{
	return body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = (float)(x2 - x1);
			float fy = (float)(y2 - y1);
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return (int)(output.fraction * dist);
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

