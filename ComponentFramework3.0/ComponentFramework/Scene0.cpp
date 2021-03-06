#include <GL/glew.h>
#include <SDL.h>
#include "Scene0.h"
#include <iostream>
#include "MMath.h"
#include "QuadSphere.h"
#include "Model0.h"
#include "Trackball.h"

using namespace GAME;
using namespace MATH;

Scene0::Scene0(class Window& windowRef):  Scene(windowRef), model0(nullptr) { 
	trackball = new Trackball();
	projectionMatrix.loadIdentity();
	viewMatrix.loadIdentity();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

Scene0::~Scene0(){ 
	OnDestroy();
}

bool Scene0::OnCreate() {
	OnResize(windowPtr->GetWidth(),windowPtr->GetHeight());

	/// Load Assets: as needed 
	lightPos = Vec3(10.0f, 3.0f, 10.0f);
	
	prim.reset(new Primitive("grid.obj"));
	
	model0 = new Model0();
	model0->SetVel(Vec3(0.0f, 0.0f, 0.0f));
	model0->SetPos(Vec3(am.pos.x, am.pos.y, 0.0f));

	prim->SetPos(Vec3(0.0f, 0.0f, 0.0f));

	am.PrintInfo();//Print game info once in the beginning
	return true;
}


void Scene0::OnResize(int w_, int h_){
	windowPtr->SetWindowSize(w_,h_);
	glViewport(0,0,windowPtr->GetWidth(),windowPtr->GetHeight());
	float aspect = float(windowPtr->GetWidth()) / float(windowPtr->GetHeight());
	
	projectionMatrix = MMath::perspective(45.0f, aspect, 1.0f, 100.0f);

	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 10.0f), 
							   Vec3(0.0f, 0.0f, 0.0f), 
							   Vec3(0.0f, 1.0f, 0.0f));
	
}

void Scene0::OnDestroy(){
	/// Cleanup Assets
	if(model0) delete model0;//ModelBody
	model0 = nullptr;

	if(trackball) delete trackball;
	trackball = nullptr;


	/*
	if (prim) delete prim;//PrimitiveBody
	prim = nullptr;
	if (am) delete am;//ActionManager
	am = nullptr;
	*/
}

void Scene0::Update(const float deltaTime){

	model0->Update(deltaTime);	
	prim->Update(deltaTime);
}

void Scene0::Render() const{
	/// Draw your scene here
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	model0->SetLightPos(viewMatrix * lightPos);
	model0->Render(projectionMatrix , trackball->GetMatrix4() *  viewMatrix, trackball->GetMatrix3());
	
	prim->Render(projectionMatrix, trackball->GetMatrix4() *  viewMatrix, trackball->GetMatrix3());
	
	SDL_GL_SwapWindow(windowPtr->getSDLWindow());
}

void Scene0::HandleEvents(const SDL_Event& SDLEvent){
	/*
	if(SDLEvent.type == SDL_EventType::SDL_MOUSEBUTTONDOWN){
		//trackball->OnLeftMouseDown(SDLEvent.button.x,SDLEvent.button.y);
	}
	if (SDLEvent.type == SDL_EventType::SDL_MOUSEMOTION && 
		SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		trackball->OnMouseMove(SDLEvent.button.x,SDLEvent.button.y);
	}
	*/
	
	if (SDLEvent.type == SDL_KEYDOWN) {
		switch (SDLEvent.key.keysym.sym) {
		//Move the Object
		case SDLK_SPACE:
			am.Undo();
			break;
		case SDLK_LCTRL:
			am.Damage();
			break;
		case SDLK_UP:
			am.MoveObject(1);
			break;
		case SDLK_DOWN:
			am.MoveObject(-1);
			break;
		case SDLK_LEFT:
			am.MoveObject(-2);
			break;
		case SDLK_RIGHT:
			am.MoveObject(2);
			break;
		case SDLK_RETURN:
			am.EndTurn();
			break;
		}
		model0->SetPos(Vec3(am.pos.x, am.pos.y, 0.0f));
		am.PrintInfo();
	}
}