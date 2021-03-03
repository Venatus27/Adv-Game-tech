#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= true;
	inSelectionMode = false;

	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("Male1.msh", &charMeshA);
	loadFunc("courier.msh", &charMeshB);
	loadFunc("security.msh", &enemyMesh);
	loadFunc("coin.msh", &bonusMesh);
	loadFunc("capsule.msh", &capsuleMesh);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld1();
	physics->UseGravity(useGravity);

}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys();

	Qtoggle = true;

	Debug::Print("F1: reset & go back to menu", Vector2(5, 5));
	Debug::Print("P: pause", Vector2(5, 10));
	Debug::Print("ESC: Quit", Vector2(5, 15));

	/*fpsCount += dt;
	if (fpsCount < 1) {
		tempFps += 1;
	}

	else {
		fps = tempFps;
		tempFps = 0;
		fpsCount = 0;
	}

	Debug::Print("FPS: " + std::to_string(fps), Vector2(70, 50));


	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95));
	}*/

	//Debug::Print("Memory fault", Vector2(40, 5));
	//Debug::DrawLine(Vector3(80, 0, 80), Vector3(80, 100, 80), Vector4(1, 0, 1, 1));
	//Debug::DrawLine(Vector3(200, 0, 10), Vector3(200, 100, 10), Vector4(1, 0, 0.5, 1));
	if (level == 1) {
		Debug::DrawLine(Vector3(-25, 135, -690), Vector3(25, 135, -690), Vector4(1, 0, 0.5, 1));
	}
	else {
		Debug::DrawLine(Vector3(170, 2, 200), Vector3(170, 2, 160), Vector4(1, 0, 0.5, 1));
		Debug::DrawLine(Vector3(20, 2, 80), Vector3(20, 100, 80), Vector4(1, 0, 0.5, 1));
	}

	///
	std::vector<GameObject*>::const_iterator f;
	std::vector<GameObject*>::const_iterator l;
	world->GetObjectIterators(f, l);
	std::vector<GameObject*> dead;

	for (auto i = f; i != l; i++) {
		(*i)->Update(dt);

		/*if ((*i)->GetName() == "Player" && firstUpdate) {
			lockedObject = *i;
			firstUpdate = false;
		}*/

		if ((*i)->getDead()) {
			dead.push_back(*i);
		}

		else if (((*i)->GetTransform().GetPosition().y <= -70 && (*i)->GetTransform().GetPosition().z > -500)
					|| ((*i)->GetTransform().GetPosition().y <= 100 && (*i)->GetTransform().GetPosition().z < -500)) { //falling off
			if ((*i)->GetName() == "Player" || (*i)->GetName() == "Enemy") { //respawning
				(*i)->GetTransform().SetPosition(checkpoint);
			}
			else {
				dead.push_back(*i); //deleting
			}
		}

		else if ((*i)->GetName() == "Player" && (*i)->GetTransform().GetPosition().z < -690 && (*i)->GetTransform().GetPosition().y > 130 && level == 1){
			
			selectionObject = nullptr;
			lockedObject = nullptr;
			
			InitCamera();

			inSelectionMode = false;
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
			InitWorld2(); //We can reset the simulation at any time with F1

			//points = 1000;
			break;
		}

		else if ((*i)->GetName() == "Player" && (*i)->GetTransform().GetPosition().x > 170 && (*i)->GetTransform().GetPosition().z > 160 && level == 2) {
			win = true;
			break;
		}

		else if ((*i)->GetName() == "Player" && (*i)->GetTransform().GetPosition().z < -500) {
			checkpoint = Vector3(0, 145, -500);
		}
	}

	if (dead.size() > 0) {
		for (int i = 0; i < dead.size(); i++) {
			world->RemoveGameObject(dead[i]);
		}
	}



	if (points != 0 && !paused) {
		dtPointCount += dt;
		if (dtPointCount > 1) {
			dtPointCount -= 1;
			points -= 10;
		}
	}

	Debug::Print("points: " + std::to_string(points), Vector2(70, 5));
	
	SelectObject();

	if (!paused) {
		MoveSelectedObject();
		physics->Update(dt);
		rampDelay += dt;
	}
	
	
	if (rampDelay > 1.5 && level == 1) {
		int choice = rand() % 2;
		int x = rand() % 190 - 100;
		if (choice == 0) {
			AddCapsuleToWorld(Vector3(x, 170, -400), 10, 5, 2);
		}

		else {
			AddSphereToWorld(Vector3(x, 170, -400), 3, 2);
		}
		
		rampDelay = 0;
	}

	if (lockedObject != nullptr) {
		Vector3 lockedOffset;
		Vector3 up;
		if (level == 1) {
			lockedOffset = lockedOffset1;
			up = Vector3(0, 1, 0);
		}

		else {
			lockedOffset = lockedOffset2;
			up = Vector3(1, 0, 0);
		}

		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, up);

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);

		//Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
	}

	/*if (!paused) {
		if (testStateObject) {
			testStateObject->Update(dt);
		}

		if (testAI) {
			testAI->Update(dt);
		}

		if (o) {
			o->Update(dt);
		}
	}*/

	/*objects = world->GetGameObjects();

	for(int i = 0; i < objects.size(); i++){
		if (objects[i]->getDead()) {
			world->RemoveGameObject(objects[i], true);
		}
	}*/

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		win = false;
		
		selectionObject = nullptr;
		lockedObject	= nullptr;
		
		InitCamera();
		
		inSelectionMode = false;
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);

		points = 1000;
		InitWorld1(); //We can reset the simulation at any time with F1
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		win = false;
		
		selectionObject = nullptr;
		lockedObject = nullptr;

		InitCamera();

		inSelectionMode = false;
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);

		points = 1000;
		InitWorld2(); //We can reset the simulation at any time with F1
	}


	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F3)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (!paused) {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
			useGravity = !useGravity; //Toggle gravity!
			physics->UseGravity(useGravity);
		}
		//Running certain physics updates in a consistent order might cause some
		//bias in the calculations - the same objects might keep 'winning' the constraint
		//allowing the other one to stretch too much etc. Shuffling the order so that it
		//is random every frame can help reduce such bias.
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
			world->ShuffleConstraints(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
			world->ShuffleConstraints(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
			world->ShuffleObjects(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
			world->ShuffleObjects(false);
		}

		if (lockedObject) {
			LockedObjectMovement();
		}
		else {
			DebugObjectMovement();
		}
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 charForward  = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 10.0f;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		Vector3 worldPos = selectionObject->GetTransform().GetPosition();
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
	
	//rotation

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		lockedObject->GetPhysicsObject()->AddTorque(-rightAxis * 2);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		lockedObject->GetPhysicsObject()->AddTorque(rightAxis * 2);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		lockedObject->GetPhysicsObject()->AddTorque(-fwdAxis * 2);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		lockedObject->GetPhysicsObject()->AddTorque(fwdAxis * 2);
	}

	//jumping
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && level == 1) {
		Vector3 spawnPos = lockedObject->GetTransform().GetPosition();
		spawnPos.y = spawnPos.y - 3.0f;
		Ray r = Ray(spawnPos, Vector3(0, -1, 0));
		RayCollision closestCollision;
		
		if (world->Raycast(r, closestCollision, true)) {
			if (abs(closestCollision.rayDistance) < 3.5f){
				lockedObject->GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 20, 0));
				doubleJump = true;
			}
			
			else if (doubleJump) {
				lockedObject->GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 20, 0));
				doubleJump = false;
			}
		}

		else if (doubleJump) {
			lockedObject->GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 20, 0));
			doubleJump = false;
		}
		
	}

}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}

}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld1() {
	level = 1;
	world->ClearAndErase();
	physics->Clear();
	checkpoint = Vector3(0, 5, 0);

	InitDefaultFloor();
	AddRampToWorld(Vector3(0, -10, -190), 30);
	AddRampToWorld(Vector3(0, 90, -363), 30);
	AddHalfFloorToWorld(Vector3(0, 140, -498));
	OrbBridge();

	addAI(Vector3(10, 10, 0));
	AddPlayerToWorld(Vector3(0, 5, 0));

	AddBonusToWorld(Vector3(0, 3, -40));
	AddBonusToWorld(Vector3(40, -30, -140));
	AddBonusToWorld(Vector3(-80, 10, -210));
	AddBonusToWorld(Vector3(-20, 50, -280));
	AddBonusToWorld(Vector3(60, 90, -350));
	AddBonusToWorld(Vector3(10, 125, -410));
	AddBonusToWorld(Vector3(0, 147, -510));


}

void TutorialGame::InitWorld2() {
	
	world->ClearAndErase();
	physics->Clear();
	level = 2;

	AddFloorToWorld(Vector3(100, -8, 100));
	AddPlayerToWorld(Vector3(190, 0, 10));
	addMazeAI(Vector3(10, 0, 10));
	
	//outer walls
	AddWallToWorld(Vector3(100, 2, 0), Vector3(95, 30, 5));
	AddWallToWorld(Vector3(100, 2, 200), Vector3(95, 30, 5));
	AddWallToWorld(Vector3(0, 2, 100), Vector3(5, 30, 100));
	AddWallToWorld(Vector3(200, 2, 100), Vector3(5, 30, 100));

	//inner walls
	AddWallToWorld(Vector3(35, 2, 30), Vector3(30, 30, 5));
	AddWallToWorld(Vector3(165, 2, 30), Vector3(30, 30, 5));

	AddWallToWorld(Vector3(60, 2, 50), Vector3(5, 30, 15));
	AddWallToWorld(Vector3(80, 2, 70), Vector3(60, 30, 5));

	AddWallToWorld(Vector3(80, 2, 130), Vector3(5, 30, 35));
	
	AddStateObjectToWorld(Vector3(70, 2, 110));
	AddStateObjectToWorld(Vector3(70, 2, 130));
	AddStateObjectToWorld(Vector3(70, 2, 150));


	AddWallToWorld(Vector3(140, 2, 160), Vector3(55, 30, 5));
	AddWallToWorld(Vector3(125, 2, 120), Vector3(40, 30, 5));

	//add bonus's
	AddBonusToWorld(Vector3(25, 0, 50));
	AddBonusToWorld(Vector3(100, 0, 140));

}


void TutorialGame::BridgeConstraintTest() {
	
	Vector3 cubeSize = Vector3(8, 8, 8);
	float invCubeMass = 5; // how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 30; // constraint distance
	float cubeDistance = 20; // distance between links
	
	Vector3 startPos = Vector3(200, 0, 0);
	
	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);
	
	GameObject* previous = start;
	
	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world -> AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world -> AddConstraint(constraint);
}

void TutorialGame::OrbBridge() {

	Vector3 cubeSize = Vector3(8, 1, 10);
	float invOrbMass = 0.5; // how heavy the middle pieces are
	int numLinks = 4;
	float maxDistance = 28; // constraint distance
	float cubeDistance = 5; // distance between links

	Vector3 startPos = Vector3(0, 130, -558);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos - Vector3(0, 0, (numLinks + 2) * 22), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddOrbToWorld(startPos - Vector3(0, 0, (i + 1) * 22), 10, invOrbMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("floor");

	Vector3 floorSize	= Vector3(100, 2, 100);
	AABBVolume* volume	= new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position)
		;
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddHalfFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Ice");

	Vector3 floorSize = Vector3(100, 2, 100);
	AABBVolume* volume = new AABBVolume(Vector3(floorSize.x, floorSize.y/2, floorSize.z/2));
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(Vector3(floorSize.x*2, floorSize.y, floorSize.z))
		.SetPosition(position)
		;
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddRampToWorld(const Vector3& position, int deg) {
	GameObject* floor = new GameObject("Ramp");

	Vector3 floorSize = Vector3(100, 2, 100);
	OBBVolume* volume = new OBBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position)
		.SetOrientation(floor->GetTransform().GetOrientation().AxisAngleToQuaterion(Vector3(1, 0, 0), deg))
		;
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
Obstacle* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	Obstacle* sphere = new Obstacle("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

Orb* TutorialGame::AddOrbToWorld(const Vector3& position, float radius, float inverseMass) {
	Orb* sphere = new Orb();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

Obstacle* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	Obstacle* capsule = new Obstacle("Capsule");

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius* 2, halfHeight, radius * 2))
		.SetPosition(position)
		.SetOrientation(capsule->GetTransform().GetOrientation().AxisAngleToQuaterion(Vector3(0, 0, 1), 90));


	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;

}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddWallToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

Player* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 1.0f;

	Player* character = new Player("Player");

	SphereVolume* volume = new SphereVolume(meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();
	character->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

	world->AddGameObject(character);

	selectionObject = character;
	lockedObject = character;

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 1.0f;

	GameObject* character = new GameObject("Enemy");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();
	character->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));


	world->AddGameObject(character);

	return character;
}

Bonus* TutorialGame::AddBonusToWorld(const Vector3& position) {
	Bonus* apple = new Bonus(&points);

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(0.0f);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));

	world->AddGameObject(apple);

	return apple;
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			//paused = true;
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			//paused = false;
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT)) {
			/*if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				lockedObject	= nullptr;
			}*/

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				GameObject* debugObject = (GameObject*)closestCollision.node;
				//selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				std::cout << "Name: " << debugObject->GetName() << '\n';
				std::cout << "Position: " << debugObject->GetTransform().GetPosition();
				std::cout << "Orientation: " << debugObject->GetTransform().GetOrientation() << '\n';
				std::cout << "====================" << '\n';

				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(5, 85));
	}

	if (lockedObject) {
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if(selectionObject){
		renderer->DrawString("Press L to lock to player!", Vector2(5, 80));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}

	}

	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject() {
	//renderer->DrawString(" Click Force :" + std::to_string(forceMagnitude), Vector2(10, 20));
	forceMagnitude += Window::GetMouse() -> GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;// we haven ’t selected anything !
	}

	if (Window::GetMouse() -> ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world -> GetMainCamera());
		RayCollision closestCollision;
		if (world -> Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				//selectionObject -> GetPhysicsObject() -> AddForce(ray.GetDirection() * forceMagnitude);
				selectionObject -> GetPhysicsObject() -> AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(5.0f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(5,5,5))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(0.5f);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	world->AddGameObject(apple);

	return apple;
}

MovingOpponent* TutorialGame::addAI(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 1.0f;

	NavigationPath n = NavigationPath();
	n.PushWaypoint(Vector3(0, 147, -700));
	n.PushWaypoint(Vector3(0, 147, -510));
	n.PushWaypoint(Vector3(10, 125, -410));
	n.PushWaypoint(Vector3(60, 90, -350));
	n.PushWaypoint(Vector3(-20, 50, -280));
	n.PushWaypoint(Vector3(-80, 10, -210));
	n.PushWaypoint(Vector3(40, -30, -140));
	n.PushWaypoint(Vector3(0, 3, -40));
	

	MovingOpponent* apple = new MovingOpponent(n);

	SphereVolume* volume = new SphereVolume(meshSize);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), enemyMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(inverseMass);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	world->AddGameObject(apple);

	return apple;
}

MovingOpponent* TutorialGame::addMazeAI(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 1.0f;

	NavigationGrid grid("cwGrid.txt");

	NavigationPath outPath;

	Vector3 startPos(10, 0, 10);
	Vector3 endPos(180, 0, 180);

	bool found = grid.FindPath(startPos, endPos, outPath);

	MovingOpponent* apple = new MovingOpponent(outPath);

	SphereVolume* volume = new SphereVolume(meshSize);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), enemyMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(inverseMass);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::pauseGame() {
	paused = !paused;
}