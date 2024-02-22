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
#include "PickaxeObject.h"
#include <cmath>
#include <limits>

namespace NCL {
    namespace CSC8503 {
        class TutorialGame {
        public:
            TutorialGame();
            ~TutorialGame();

            virtual void UpdateGame(float dt);

            static TutorialGame* GetGame() {
                return instance;
            };

        protected:
            void InitialiseAssets();

            void InitMaterials();
            void InitAnimations();
            void InitCamera();
            void UpdateKeys();

            void InitWorld();

            /*
            These are some of the world/object creation functions I created when testing the functionality
            in the module. Feel free to mess around with them to see different objects being created in different
            test scenarios (constraints, collision types, and so on).
            */
            void InitGameExamples();

            void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
            void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
            void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);

            void InitDefaultFloor();
            //int SelectRailDir(Vector3 lastRailPosition,Vector3 RailPosition);
            bool SelectObject();
            void MoveSelectedObject();
            void DebugObjectMovement();
            void LockedObjectMovement();

            void AddSceneToWorld();
            void HoldObject();

            GameObject* AddFloorToWorld(const Vector3& position);
            GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
            GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

            GameObject* AddPlayer0ToWorld(const Vector3& position);
            GameObject* AddEnemyToWorld(const Vector3& position);
            TrainObject* AddTrainToWorld(const Vector3& position);
            GameObject* AddTestingLightToWorld(const Vector3& position, const Vector4& colour);
            PlayerObject* AddPlayerToWorld(const Vector3& position);
            TreeObject* AddTreeToWorld(const Vector3& position);
            CollectableObject* AddCollectableObjectToGround(int ID);
            PickaxeObject* AddPickaxeToWorld(const Vector3& position);

            CollectableObject* CreateObject(int ID);

            //CollectableObject* AddRailToWorld(int direction,Vector3 position,Vector3 lastRailPosition);
            StateGameObject* AddStateObjectToWorld(const Vector3& position);
            StateGameObject* testStateObject = nullptr;

            void BridgeConstraintTest();

#ifdef USEVULKAN
            GameTechVulkanRenderer* renderer;
#else
            GameTechRenderer* renderer;
#endif
            PhysicsSystem* physics;
            GameWorld* world;
            Audio* audio;
            std::vector<GameObject*> scene;
            KeyboardMouseController controller;

            Vector3 ObjectPos;
            Vector3 finalObjectPos;
            Vector3 lastRailPositin;
            Vector3 RailPositin;

            bool useGravity;
            bool inSelectionMode;

            float		forceMagnitude;

            GameObject* selectionObject = nullptr;

            Mesh* capsuleMesh = nullptr;
            Mesh* cubeMesh = nullptr;
            Mesh* sphereMesh = nullptr;

            Texture* basicTex = nullptr;
            Texture* floorTex = nullptr;
            Texture* trainTex = nullptr;
            Texture* lightTex = nullptr;

            Texture* lightBumpTex = nullptr;

            Texture* lightSpecTex = nullptr;

            Shader* basicDayShader = nullptr;
            Shader* bumpDayShader = nullptr;
            Shader* specDayShader = nullptr;
            Shader* skinningPerPixelDayShader = nullptr;
            Shader* skinningBumpDayShader = nullptr;

            Shader* basicNightShader = nullptr;
            Shader* bumpNightShader = nullptr;
            Shader* specNightShader = nullptr;
            Shader* skinningPerPixelNightShader = nullptr;
            Shader* skinningBumpNightShader = nullptr;

            ShaderGroup* basicShader = nullptr;
            ShaderGroup* bumpShader = nullptr;
            ShaderGroup* specShader = nullptr;
            ShaderGroup* skinningPerPixelShader = nullptr;
            ShaderGroup* skinningBumpShader = nullptr;

            //Coursework Meshes
            Mesh* charMesh = nullptr;
            Mesh* enemyMesh = nullptr;
            Mesh* bonusMesh = nullptr;

            Mesh* maleMesh = nullptr;
            Mesh* femaleMesh = nullptr;
            Mesh* assassinMesh = nullptr;
            Mesh* girlMesh = nullptr;
            Mesh* smurfMesh = nullptr;

            OBJMesh* trainMesh = nullptr;
            OBJMesh* creeperMesh = nullptr;
            OBJMesh* pickaxeMesh = nullptr;
            OBJMesh* axeMesh = nullptr;

            MeshMaterial* maleMaterial = nullptr;
            MeshMaterial* femaleMaterial = nullptr;
            MeshMaterial* assassinMaterial = nullptr;
            MeshMaterial* girlMaterial = nullptr;
            MeshMaterial* smurfMaterial = nullptr;

            vector<GLuint> maleTextures;
            vector<GLuint> femaleTextures;
            vector<GLuint> assassinTextures;
            vector<GLuint> girlTextures;
            vector<GLuint> smurfTextures;

            vector<GLuint> maleBumpTextures;
            vector<GLuint> femaleBumpTextures;
            vector<GLuint> girlBumpTextures;
            vector<GLuint> smurfBumpTextures;

            AnimationObject* maleAnimation = nullptr;
            AnimationObject* femaleAnimation = nullptr;
            AnimationObject* assassinAnimation = nullptr;
            AnimationObject* girlAnimation = nullptr;
            AnimationObject* smurfAnimation = nullptr;

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

            TrainObject* trainObject = nullptr;

            PlayerObject* player = nullptr;

            CollectableObject* object = nullptr;
            CollectableObject* groundObject = nullptr;

            PickaxeObject* pickaxe = nullptr;

            vector<Vector3> mazeNodes;
            NavigationGrid* navGrid;

            static TutorialGame* instance;
        };
    }
}

