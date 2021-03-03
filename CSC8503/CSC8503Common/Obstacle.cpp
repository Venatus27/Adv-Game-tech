#include "Obstacle.h"

using namespace NCL;
using namespace CSC8503;

Obstacle::Obstacle(string objectName) {
	force = Vector3(20, 0, 0);
	name = objectName;
}

Obstacle::~Obstacle() {
}


void Obstacle::Update(float dt) {
	if (!initalised) {
		GetPhysicsObject()->AddTorque(force + Vector3(1000, 0, 0));
		initalised = true;
	}

	GetPhysicsObject()->AddTorque(force);
}

void Obstacle::OnCollisionEnd(GameObject* otherObject) {
	if (otherObject->GetName() == "Enemy") {
		GetPhysicsObject()->AddForce(Vector3(10, 0, 0));
	}

	if (otherObject->GetName() == "Bonus") {
		GetPhysicsObject()->AddForce(Vector3(0, 100, 0));
	}

}
