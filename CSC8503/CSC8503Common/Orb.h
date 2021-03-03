#pragma once
#include "../CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Orb : public GameObject
		{
		public:
			Orb();
			~Orb();

			void Update(float dt);
			//void OnCollisionBegin(GameObject* otherObject);
			//void OnCollisionEnd(GameObject* otherObject);
			Vector3 force;
			bool initalised = false;
		};
	}
}

