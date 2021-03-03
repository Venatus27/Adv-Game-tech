#pragma once
#include "../CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Player : public GameObject {
		public:
			Player(string name = "");
			~Player();

			//void Update(float dt);
			void OnCollisionBegin(GameObject* otherObject);
		};
	}
}

