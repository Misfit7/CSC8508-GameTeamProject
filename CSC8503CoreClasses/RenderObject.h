#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include "MeshMaterial.h"
#include "MeshAnimation.h"

namespace NCL {
    using namespace NCL::Rendering;

    namespace CSC8503 {
        class Transform;
        using namespace Maths;

        class RenderObject
        {
        public:
            RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, ShaderGroup* shaderGroup, int drawMode = 1);
            RenderObject(Transform* parentTransform, OBJMesh* mesh, Texture* tex, ShaderGroup* shaderGroup, int drawMode = 2);
            ~RenderObject();

            void SetDefaultTexture(Texture* t) {
                texture = t;
            }

            Texture* GetDefaultTexture() const {
                return texture;
            }

            Texture* GetBumpTexture() const {
                return bumpTexture;
            }
            void SetBumpTexture(Texture* b) {
                bumpTexture = b;
            }

            Texture* GetSpecTexture() const {
                return specTexture;
            }
            void SetSpecTexture(Texture* s) {
                specTexture = s;
            }

            Mesh* GetMesh() const {
                return mesh;
            }

            OBJMesh* GetOBJMesh() const {
                return objMesh;
            }

            int GetDrawMode() const {
                return drawMode;
            }

            Transform* GetTransform() const {
                return transform;
            }

            ShaderGroup* GetShaderGroup() const {
                return shaderGroup;
            }

            MeshAnimation* GetAnim() const {
                return animation;
            }
            void SetAnim(MeshAnimation* a) {
                animation = a;
            }

            void SetTextures(GLuint t) {
                textures.emplace_back(t);
            }

            GLuint GetLayerTexture(int i) const {
                if (i < 0 || i > textures.size()) {
                    return 0;
                }
                return textures[i];
            }

            void SetColour(const Vector4& c) {
                colour = c;
            }

            Vector4 GetColour() const {
                return colour;
            }

            void SetEmissive(bool e) {
                emissive = e;
            }
            int GetEmissive() const {
                return emissive;
            }

            float GetCameraDistance() const { return distanceFromCamera; }
            void SetCameraDistance(float f) { distanceFromCamera = f; }

            int GetCurrentFrame() const { return currentFrame; }
            void SetCurrentFrame(int f) { currentFrame = f; }

            float GetFrameTime() const { return frameTime; }
            void SetFrameTime(float f) { frameTime = f; }

        protected:
            Mesh*           mesh;
            OBJMesh*        objMesh;
            Texture*        texture;
            Texture*        bumpTexture;
            Texture*        specTexture;
            ShaderGroup*    shaderGroup;
            vector<GLuint>  textures;
            MeshAnimation*  animation;
            Transform*      transform;
            Vector4	        colour;
            int             drawMode;
            bool            emissive;
            float           distanceFromCamera = 0.0f;
            float           currentFrame = 0;
            float           frameTime = 0.0f;
        };
    }
}
