#pragma once
#include "../CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Obstacle : public GameObject
		{
		public:
			Obstacle(string name = "");
			~Obstacle();

			void Update(float dt);
			//void OnCollisionBegin(GameObject* otherObject);
			void OnCollisionEnd(GameObject* otherObject);
			Vector3 force;
			bool initalised = false;
		};
	}
}

