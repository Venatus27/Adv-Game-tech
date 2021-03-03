#include "Player.h"
using namespace NCL;
using namespace CSC8503;

Player::Player(string objectName) {
	name = objectName;
}

Player::~Player() {
}

void Player::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetName() == "Ice") {
		GetPhysicsObject()->setFriction(2.0f);
	}

	else {
		GetPhysicsObject()->setFriction(1.0f);
	}

}