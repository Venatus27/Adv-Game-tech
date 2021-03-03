#include "Bonus.h"

using namespace NCL;
using namespace CSC8503;

Bonus::Bonus(int *scoreptr) {
	name = "Bonus";
	score = scoreptr;
}

Bonus::~Bonus() {
}

void Bonus::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetName() == "Enemy") {

	}
}

void Bonus::OnCollisionEnd(GameObject* otherObject) {
	if (otherObject->GetName() == "Enemy") {
		dead = true;
		
	}

	if (otherObject->GetName() == "Player") {
		dead = true;
		*score += 100;
	}
}
