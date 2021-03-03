#pragma once
#include "../CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Bonus : public GameObject
		{
		public:
			Bonus(int *scoreptr);
			~Bonus();
			void OnCollisionBegin(GameObject* otherObject);
			void OnCollisionEnd(GameObject* otherObject);

			int *score;
		};
	}
}

