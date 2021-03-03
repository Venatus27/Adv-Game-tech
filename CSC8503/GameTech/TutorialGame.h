#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/StateGameObject.h"
#include "../CSC8503Common/MovingOpponent.h"
#include "../CSC8503Common/Obstacle.h"
#include "../CSC8503Common/Bonus.h"
#include "../CSC8503Common/Orb.h"
#include "../CSC8503Common/Player.h"
#include "../CSC8503Common/NavigationGrid.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);
			void pauseGame();
			int getPoints() {
				return points;
			}

			bool getWin() {
				return win;
			}

			bool getSelect() {
				return inSelectionMode;
			}

			bool getAutoPaused() {
				return autoPause;
			}

			void flipAutoPaused() {
				autoPause = !autoPause;
			}

			bool Qtoggle = true;

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld1();
			void InitWorld2();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
			void BridgeConstraintTest();
			void OrbBridge();
	
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddHalfFloorToWorld(const Vector3& position);
			GameObject* AddRampToWorld(const Vector3& position, int deg);
			Obstacle* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddWallToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);
			
			Obstacle* o = nullptr;
			Obstacle* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			Player* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			Bonus* AddBonusToWorld(const Vector3& position);
			Orb* AddOrbToWorld(const Vector3& position, float radius, float inverseMass);
			
			StateGameObject* AddStateObjectToWorld(const Vector3& position);
			StateGameObject* testStateObject = nullptr;

			MovingOpponent* testAI = nullptr;
			MovingOpponent* addAI(const Vector3& position);
			MovingOpponent* addMazeAI(const Vector3& position);

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset1		= Vector3(0, 14, 50);
			Vector3 lockedOffset2 = Vector3(0, 60, 0);

			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			float dtPointCount = 0;
			int points = 1000;

			bool paused = false;
			bool autoPause = false;
			std::vector<GameObject*> objects;
			float rampDelay;

			float fpsCount;
			int tempFps;
			int fps;

			bool doubleJump = false;

			int level;
			bool win = false;

			Vector3 checkpoint;

		};
	}
}

