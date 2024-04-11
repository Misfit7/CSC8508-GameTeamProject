#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"
#include "NavigationGrid.h"
#include "MeshMaterial.h"
#include "Audio.h"
#include "StateGameObject.h"
#include "CollectableObject.h"
#include "TrainObject.h"
#include "PlayerObject.h"
#include "TreeObject.h"
#include "RockObject.h"
#include "WaterObject.h"
#include "PickaxeObject.h"
#include "AxeObject.h"
#include "BucketObject.h"
#include "PlankObject.h"
#include "StoneObject.h"
#include "RailObject.h"
#include "AnimalObject.h"
#include "RobotObject.h"
#include "DroneObject.h"
#include <cmath>
#include <limits>

namespace NCL {
    namespace CSC8503 {
        class TutorialGame {
        public:
            TutorialGame();
            ~TutorialGame();

            virtual void UpdateGame(float dt);

            GameWorld* GetWorld() const {
                return world;
            }

            static TutorialGame* GetGame() {
                return instance;
            };

            bool IsNetworked() const {
                return networked;
            }
            void SetNetworked(bool n) {
                networked = n;
            }

            void SetRestartFlag(bool r) {
                restartFlag = r;
            }

            int GetLevel() const {
                return level;
            }
            void SetLevel(int l) {
                level = l;
            }

            bool Failure() const {
                return failure;
            }
            void SetFailure(bool f) {
                failure = f;
            }
            bool Success() const {
                return success;
            }
            void SetSuccess(bool s) {
                success = s;
            }

            BucketObject* GetBucket() const {
                return bucket;
            }

            RobotObject* GetRobot() const {
                return robot;
            }

			GameObject* GetStation() const {
				return station;
			}

            NavigationGrid* GetNavigationGrid()const {
                return navGrid;
            }

			bool ShowDebug() const {
				return showDebug;
			}

            void ResetPlaytime() { playtime = 0.0f; };

            PlankObject* AddPlankToWorld(const Vector3& position, bool network = false, int id = 0);
            StoneObject* AddStoneToWorld(const Vector3& position, bool network = false, int id = 0);
            RailObject* AddRailToWorld(const Vector3& position, bool network = false, int id = 0, bool placed = false);

            void InitGameWorld(bool networked = false, int level = 1);
            bool IsExitGame() { return isExit; }
            float GetPlayTime() { return playtime; };
            TrainObject* GetTrain() const { return train; };
            PlayerObject* GetPlayer() const { return player; }
            MaterialCarriage* GetMaterialCarriage() { return carriage1; }
            ProduceCarriage* GetProduceCarriage() { return carriage2; }
            WaterCarriage* GetWaterCarriage() { return carriage3; }

            Audio* GetAudio() { return audio; }

        protected:
            void InitMeshes();
            void InitTextures();
            void InitMaterials();
            void InitAnimations();
            void InitShaders();
            void AssetsLoading();
            int assetsLoadedStep = 0;

            void InitCamera();
            void InitWorld(bool networked = false, int level = 1);

            void DrawPad();

            virtual void UpdateKeys();

            void UpdateLoading(float dt);
            virtual void UpdatePaused(float dt);
            virtual void UpdateFailure(float dt);
            virtual void UpdateFinish(float dt);
            void UpdateMenu(float dt);

            /*
            These are some of the world/object creation functions I created when testing the functionality
            in the module. Feel free to mess around with them to see different objects being created in different
            test scenarios (constraints, collision types, and so on).
            */
            void InitPositions(bool networked = false, int level = 1);
            void InitGameExamples(bool networked = false, int level = 1);

            void InitDefaultFloor();
            bool SelectObject();
            void MoveSelectedObject();
            void LockedObjectMovement();
            void CameraUpdate();

            void AddSceneToWorld(int level = 1);
            void HoldObject();

            GameObject* AddFloorToWorld(const Vector3& position);
            GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
            GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

            GameObject* AddPlayer0ToWorld(const Vector3& position);
            GameObject* AddEnemyToWorld(const Vector3& position);
            TrainObject* AddTrainToWorld(const Vector3& position, bool spawn = true);
            GameObject* AddStationToWorld(const Vector3& position);
            GameObject* AddTestingLightToWorld(const Vector3& position, const Vector4& colour);
            PlayerObject* AddPlayerToWorld(const Vector3& position, std::string name, int num, bool spawn = true);
            TreeObject* AddTreeToWorld(const Vector3& position);
            RockObject* AddRockToWorld(const Vector3& position);
            GameObject* AddDesertRockToWorld(const Vector3& position);
            WaterObject* AddWaterToWorld(const Vector3& position);
            CollectableObject* AddCollectableObjectToGround(int ID);
            PickaxeObject* AddPickaxeToWorld(const Vector3& position, bool spawn = true);
            AxeObject* AddAxeToWorld(const Vector3& position, bool spawn = true);
            BucketObject* AddBucketToWorld(const Vector3& position, bool spawn = true);
            AnimalObject* AddMooseToWorld(const Vector3& position);
            RobotObject* AddRobotToWorld(const Vector3& position);
            DroneObject* AddDroneToWorld(const Vector3& position);
            DetectionSphereObject* AddDetectionSphereToWorld(const Vector3& position, float radius, AnimalObject* animal);

            GameObject* AddPadToWorld();

            CollectableObject* CreateObject(int ID);

            //CollectableObject* AddRailToWorld(int direction,Vector3 position,Vector3 lastRailPosition);
            StateGameObject* AddStateObjectToWorld(const Vector3& position);
            StateGameObject* testStateObject = nullptr;

            vector<std::pair<Vector3, int>> Path;//Ìú¹ìÂ·¾¶

            void BridgeConstraintTest();

#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem* physics;
			GameWorld* world;
			Audio* audio;
			KeyboardMouseController controller;

			Vector3 ObjectPos;
			Vector3 finalObjectPos;
			Vector3 lastRailPositin;
			Vector3 RailPositin;

			bool networked = false;
			bool useGravity;
			bool inSelectionMode;
			int cameraMode = 1;

			bool winFlag = false;
			bool spawnFlag = false;
			bool restartFlag = false;

			int level = 1;
			bool success = false;
			bool failure = false;

			bool showDebug = false;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			Mesh* capsuleMesh = nullptr;
			Mesh* cubeMesh = nullptr;
			Mesh* sphereMesh = nullptr;

			//Coursework Meshes
			Mesh* charMesh = nullptr;
			Mesh* enemyMesh = nullptr;
			Mesh* bonusMesh = nullptr;

			Mesh* maleMesh = nullptr;
			Mesh* femaleMesh = nullptr;
			Mesh* assassinMesh = nullptr;
			Mesh* girlMesh = nullptr;
			Mesh* smurfMesh = nullptr;
			Mesh* mooseMesh = nullptr;
			Mesh* robotMesh = nullptr;
			Mesh* droneMesh = nullptr;
			Mesh* carriageMesh = nullptr;
			Mesh* treeMesh = nullptr;
			Mesh* rockMesh = nullptr;
			Mesh* desertRockMesh = nullptr;
			Mesh* pickaxeMesh = nullptr;
			Mesh* axeMesh = nullptr;
			Mesh* bucketMesh = nullptr;
			Mesh* plankMesh = nullptr;
			Mesh* stoneMesh = nullptr;
			Mesh* railMesh = nullptr;
			Mesh* railTurnMesh = nullptr;
			Mesh* stationMesh = nullptr;

			OBJMesh* trainMesh = nullptr;

			Texture* basicTex = nullptr;
			Texture* floorTex = nullptr;
			Texture* trainTex = nullptr;
			Texture* carriageTex = nullptr;
			Texture* treeTex = nullptr;
			Texture* rockTex = nullptr;
			Texture* desertRockTex = nullptr;
			Texture* waterTex = nullptr;
			Texture* lightTex = nullptr;
			Texture* pickaxeTex = nullptr;
			Texture* axeTex = nullptr;
			Texture* bucketTex = nullptr;
			Texture* plankTex = nullptr;
			Texture* stoneTex = nullptr;
			Texture* railTex = nullptr;
			Texture* railTurnTex = nullptr;

			GLuint stationTex;

			Texture* floorBumpTex = nullptr;
			Texture* rockBumpTex = nullptr;
			Texture* desertRockBumpTex = nullptr;
			Texture* waterBumpTex = nullptr;
			Texture* lightBumpTex = nullptr;
			Texture* pickaxeBumpTex = nullptr;
			Texture* axeBumpTex = nullptr;
			Texture* bucketBumpTex = nullptr;
			Texture* plankBumpTex = nullptr;
			Texture* stoneBumpTex = nullptr;
			Texture* railBumpTex = nullptr;
			Texture* railTurnBumpTex = nullptr;

			Texture* floorSpecTex = nullptr;
			Texture* lightSpecTex = nullptr;

			MeshMaterial* maleMaterial = nullptr;
			MeshMaterial* femaleMaterial = nullptr;
			MeshMaterial* assassinMaterial = nullptr;
			MeshMaterial* girlMaterial = nullptr;
			MeshMaterial* smurfMaterial = nullptr;
			MeshMaterial* mooseMaterial = nullptr;
			MeshMaterial* robotMaterial = nullptr;
			MeshMaterial* droneMaterial = nullptr;

			vector<GLuint> maleTextures;
			vector<GLuint> femaleTextures;
			vector<GLuint> assassinTextures;
			vector<GLuint> girlTextures;
			vector<GLuint> smurfTextures;
			vector<GLuint> mooseTextures;
			vector<GLuint> robotTextures;
			vector<GLuint> droneTextures;

			vector<GLuint> maleBumpTextures;
			vector<GLuint> femaleBumpTextures;
			vector<GLuint> girlBumpTextures;
			vector<GLuint> smurfBumpTextures;
			vector<GLuint> robotBumpTextures;

			AnimationObject* maleAnimation = nullptr;
			AnimationObject* femaleAnimation = nullptr;
			AnimationObject* assassinAnimation = nullptr;
			AnimationObject* girlAnimation = nullptr;
			AnimationObject* smurfAnimation = nullptr;
			AnimationObject* mooseAnimation = nullptr;
			AnimationObject* robotAnimation = nullptr;
			AnimationObject* droneAnimation = nullptr;

			Shader* basicDayShader = nullptr;
			Shader* bumpDayShader = nullptr;
			Shader* specDayShader = nullptr;
			Shader* skinningPerPixelDayShader = nullptr;
			Shader* skinningBumpDayShader = nullptr;
			Shader* skinningBumpDayShader2 = nullptr;
			Shader* reflectDayShader = nullptr;

			Shader* basicNightShader = nullptr;
			Shader* bumpNightShader = nullptr;
			Shader* specNightShader = nullptr;
			Shader* skinningPerPixelNightShader = nullptr;
			Shader* skinningBumpNightShader = nullptr;
			Shader* skinningBumpNightShader2 = nullptr;
			Shader* reflectNightShader = nullptr;

			ShaderGroup* basicShader = nullptr;
			ShaderGroup* bumpShader = nullptr;
			ShaderGroup* specShader = nullptr;
			ShaderGroup* skinningPerPixelShader = nullptr;
			ShaderGroup* skinningBumpShader = nullptr;
			ShaderGroup* skinningBumpShader2 = nullptr;
			ShaderGroup* reflectShader = nullptr;

			vector<Mesh*> meshes;
			vector<vector<GLuint>> textures;
			vector<vector<GLuint>> bumpTextures;
			vector<AnimationObject*> animations;
			vector<ShaderGroup*> shaders;

			//Coursework Additional functionality	
			GameObject* lockedObject = nullptr;
			Vector3 lockedOffset = Vector3(0, 88, 0);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* character = nullptr;
			GameObject* objClosest = nullptr;

			TrainObject* train = nullptr;
			MaterialCarriage* carriage1 = nullptr;
			ProduceCarriage* carriage2 = nullptr;
			WaterCarriage* carriage3 = nullptr;

			PlayerObject* player = nullptr;
			PlayerObject* player2 = nullptr;
			PlayerObject* player3 = nullptr;
			PlayerObject* player4 = nullptr;

			GameObject* pad = nullptr;

			CollectableObject* object = nullptr;
			CollectableObject* groundObject = nullptr;

			PickaxeObject* pickaxe = nullptr;
			AxeObject* axe = nullptr;
			BucketObject* bucket = nullptr;
			AnimalObject* moose = nullptr;
			DroneObject* drone = nullptr;
			DroneObject* drone2 = nullptr;
			RobotObject* robot = nullptr;

			GameObject* station = nullptr;

			Vector3 player1Position;
			Vector3 player2Position;
			Vector3 player3Position;
			Vector3 player4Position;
			Vector3 axePosition;
			Vector3 pickaxePosition;
			Vector3 bucketPosition;
			Vector3 trainPosition;
			Vector3 robotPosition;
			Vector3 moosePosition;
			Vector3 dronePosition;
			Vector3 drone2Position;

			NavigationGrid* navGrid;

			static TutorialGame* instance;

			bool usePad = false;
			bool isExit = false;
			float playtime = 0.0f;
		};

		/* struct AssetsInfo
		 {
			 AssetsInfo(AssetsType t, string n, string p) {
				 type = t;
				 objectname = n;
				 filepath = p;
			 }
			 AssetsType type;
			 string objectname;
			 string filepath;
		 };
		 enum AssetsType
		 {
			 MESH,
			 ObjMESH,
			 TEXTURE,
			 MATERIAL,
			 ANIMATION,
			 SHADER
		 };*/
	}
}

