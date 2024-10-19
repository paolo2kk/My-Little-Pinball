#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModulePhysics.h"
#include "ModuleAudio.h"
#include "ModuleScene.h"

//habra que añadir los .h de los objetos que den puntos y el.h de un ModuleScore o algo asi

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ball = NULL;
}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// Load music
	background_music = App->audio->PlayMusic("pinball/Background_Music.ogg");

	// Load FX
	fx[FX_FLIPPER] = App->audio->LoadFx("pinball/Flipper.ogg");
	fx[FX_GAME_OVER] = App->audio->LoadFx("pinball/Game_Over.ogg");
	fx[FX_LAUNCHER] = App->audio->LoadFx("pinball/Launcher.ogg");
	fx[FX_START] = App->audio->LoadFx("pinball/Start.ogg");
		

	//Load textures
	
	//Load mierdas para conseguir puntos
	
	//Crear ball
	
	//Crear death_trigger
	death_trigger = App->physics->CreateRectangleSensor(210, 1000, 229, 35);//poner bien las coordenadas y el tamaño 
	death_trigger->type = DEATH; //crear más si hace falta

	//launcher joint
	b2MouseJointDef def;
	def.bodyA = App->physics->ground;
	def.bodyB = launcher->body;
	def.target = { PIXEL_TO_METERS(449), PIXEL_TO_METERS(935) };
	def.damping = 1.0f;
	def.maxForce = 10000.0f * launcher->body->GetMass();
	mouse_joint = (b2MouseJoint*)App->physics->world->CreateJoint(&def);

	return ret;

}

update_status ModuleScene::Update()
{
	//Renderizar objetos que den puntos y el score

	//if game_state == restart,  resetear el score, la vida, los objetos que den puntos y por ultimo cambiar el game state a jugar
	
	ManageInputs();
	
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	App->audio->Disable();

	return true;
}

void ModuleScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Contact* contact)
{
	//tambien habra que hacer esto con los objetos que den puntos haciendo que actualice el score y que se destruyan
	if (bodyA->type == BALL && bodyB->type == DEATH)
	{
		LoseLife();
	}
}

void ModuleScene::LoseLife()
{
	lives--;
	if (lives == 0)
	{
		GameOver();
	}
	else
	{
		game_state = RESTART;
	}
}

void ModuleScene::ManageInputs()
{
	//if game_state == start_menu, esperar a que se pulse el boton de start y cambiar el game state a playing
	
	//if game_state == playing, mover los flippers y el launcher
	 
	//if game_state == game_over, esperar a que se pulse el boton de start y cambiar el game state a playing o que pulse al boton de exit y cambiar el game state a start_menu

}

void ModuleScene::GameOver()
{
	App->audio->PlayFx(fx[FX_GAME_OVER]);
	game_state = GAME_OVER;
}



