#include "MovingOpponent.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"

using namespace NCL;
using namespace CSC8503;

MovingOpponent::MovingOpponent(NavigationPath n, string na) {
	counter = 0.0f;
	stateMachine = new StateMachine();
	path = n;
	name = na;

	State* stateA = new State([&](float dt) -> void {
		this->ChangeTarget(dt);
		});

	State* stateB = new State([&](float dt) -> void {
		this->MoveToTarget(dt);
		});

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]() -> bool {
		return GetTransform().GetPosition() != target;
		}));

	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]() -> bool {
		bool x = GetTransform().GetPosition().x < target.x + 5 && GetTransform().GetPosition().x > target.x - 5;
		bool y = GetTransform().GetPosition().y < target.y + 5 && GetTransform().GetPosition().y > target.y - 5;
		bool z = GetTransform().GetPosition().z < target.z + 5 && GetTransform().GetPosition().z > target.z - 5;

		return x && y && z;
		//return this->counter > 3.0f;
		}));
}

MovingOpponent::~MovingOpponent(){
	delete stateMachine;
}

void MovingOpponent::Update(float dt) {
	stateMachine->Update(dt);
}

void MovingOpponent::ChangeTarget(float dt) {		
	GetPhysicsObject()-> ClearForces();
	prior = GetTransform().GetPosition();
	
	if (path.PopWaypoint(target)) {
		//std::cout << "going for: " << target;
		dir = (target - prior);
	}

	else{
		dir = Vector3(0, 0, 0);
	}

	counter = 0;
}

void MovingOpponent::MoveToTarget(float dt) {
	prior = GetTransform().GetPosition();
	dir = (target - prior);
	GetPhysicsObject()->ClearForces();
	dir.Normalise();
	GetPhysicsObject()->AddForce(dir * 10);
	counter += dt;
}
