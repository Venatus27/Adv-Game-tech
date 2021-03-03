#pragma once
#include "..\CSC8503Common\GameObject.h"
#include "../CSC8503Common/NavigationPath.h"

namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class MovingOpponent : public GameObject {
		public:
			MovingOpponent(NavigationPath n, string na = "Enemy");
			~MovingOpponent();

			virtual void Update(float dt);


		protected:
			void ChangeTarget(float dt);
			void MoveToTarget(float dt);

			StateMachine* stateMachine;
			NavigationPath path;
			float counter;
			Vector3 target;
			Vector3 prior;
			Vector3 dir;
		};
	}
}

