#include "Orb.h"
using namespace NCL;
using namespace CSC8503;

Orb::Orb() {
	force = Vector3(-20, 0, 0);
	name = "Orb";
}

Orb::~Orb() {
}


void Orb::Update(float dt) {
	if (!initalised) {
		GetPhysicsObject()->AddTorque(force + Vector3(-1000, 0, 0));
		initalised = true;
	}

	GetPhysicsObject()->AddTorque(force);
}