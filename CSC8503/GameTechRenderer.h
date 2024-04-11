#pragma once
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"
#include "Light.h"
#include "Particle.h"

#include "GameWorld.h"
#include "UI.h"

#include "Frustum.h"

namespace NCL {
    class Maths::Vector3;
    class Maths::Vector4;
    namespace CSC8503 {
        class RenderObject;

        class GameTechRenderer : public OGLRenderer {
        public:
            GameTechRenderer(GameWorld& world);
            ~GameTechRenderer();

            Mesh* LoadMesh(const std::string& name);
            OBJMesh* LoadOBJMesh(const std::string& name);
            Texture* LoadTexture(const std::string& name);
            GLuint LoadGLTexture(const std::string& name);
            Shader* LoadShader(const std::string& vertex, const std::string& fragment);
            void       ToggleNight();
            UI* GetUI() { return ui; };
            ParticleGenerator* GetParticle() { return particle; };
            void Update(float dt) override;

        protected:
            void NewRenderLines();
            void NewRenderText();

            void RenderFrame()	override;

            OGLShader* defaultShader;

            GameWorld& gameWorld;
            UI* ui;

            void InitBuffers();
            void ClearAllBuffers();

            void GenerateShadowTexture();
            void GenerateCombinedTexture();
            void GenerateScreenTexture(GLuint& into, bool depth = false);

            void BuildObjectList();
            void SortObjectList();
            void RenderShadowMap();
            void RenderCamera();
            void RenderSkybox();
            void DrawLightBuffer();
            void SetShaderLight(const Light& l);
            void DrawPointLights();
            void DrawParticle();

            void DrawProcess(int count);
            void ProcessCombine();

            void CombineBuffers();

            void LoadSkybox();

            void SetDebugStringBufferSizes(size_t newVertCount);
            void SetDebugLineBufferSizes(size_t newVertCount);

            void Draw(Mesh* mesh, bool multilayer = true);
            void OBJDraw(OBJMesh* mesh);

            //sort and frustum
            vector<const RenderObject*> activeObjects;
            vector<const RenderObject*> transparentActiveObjects;
            void CleanObjectList();
            Frustum frameFrustum;

            //World Buffer
            GLuint worldFBO;
            GLuint worldColourTex;
            GLuint worldNormalTex;
            GLuint worldSpecTex;
            GLuint worldEmisTex;
            GLuint worldDepthTex;
            GLuint worldShadowTex;

            //Light Buffer
            GLuint lightFBO;
            GLuint lightDiffuseTex;
            GLuint lightSpecularTex;

            //Combine Buffer
            GLuint combinedFBO;
            GLuint combinedTex;

            //Process Buffer
            GLuint processFBO;
            GLuint hdrColourTex;
            GLuint pausedScreenTex;
            GLuint blurColourTex[2];

            //Skybox Buffer
            OGLShader* debugShader;
            OGLShader* skyboxShader;
            OGLMesh* skyboxMesh;
            GLuint		daySkyboxTex;
            GLuint      nightSkyboxTex;
            GLuint		skyboxBufferTex;
            GLuint      skyboxFBO;

            //shadow mapping things
            OGLShader* shadowShader;
            OGLShader* skinningShadowShader;
            GLuint		shadowTex;
            GLuint		shadowFBO;
            Matrix4     shadowMatrix;

            Light* sunLight;
            Light* stationLight;
            Light* playerLight;
            Light* trainLight;
            Light* robotLight;

            OGLShader* pointLightShader;
            Mesh* sphere;
            Mesh* quad;

            OGLShader* processShader;
            OGLShader* processCombineShader;

            OGLShader* combineShader;

            //Debug data storage things
            vector<Vector3> debugLineData;

            vector<Vector3> debugTextPos;
            vector<Vector4> debugTextColours;
            vector<Vector2> debugTextUVs;

            GLuint lineVAO;
            GLuint lineVertVBO;
            size_t lineCount;

            GLuint textVAO;
            GLuint textVertVBO;
            GLuint textColourVBO;
            GLuint textTexVBO;
            size_t textCount;

            bool   isNight;

            //Particle
            OGLShader* particleShader;
            GLuint particleTex;
            ParticleGenerator* particle;
        };
    }
}

