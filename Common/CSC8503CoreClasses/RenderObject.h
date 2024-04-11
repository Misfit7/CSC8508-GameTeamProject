#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include "AnimationObject.h"
#include "MeshMaterial.h"
#include "MeshAnimation.h"

using std::vector;

namespace NCL {
    using namespace NCL::Rendering;

    namespace CSC8503 {
        class Transform;
        using namespace Maths;

        class RenderObject
        {
        public:
            RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, ShaderGroup* shaderGroup, int drawMode = 1);
            RenderObject(Transform* parentTransform, Mesh* mesh, GLuint tex, ShaderGroup* shaderGroup, int drawMode = 1);
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
            void SetMesh(Mesh* m) {
                mesh = m;
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
            void SetShaderGroup(ShaderGroup* s) {
                shaderGroup = s;
            }

            void SetTextures(vector<GLuint> t) {
                textures = t;
            }
            GLuint GetLayerTexture(int i) const {
                if (i < 0 || i > textures.size()) {
                    return 0;
                }
                return textures[i];
            }

            void SetBumpTextures(vector<GLuint> t) {
                bumpTextures = t;
            }
            GLuint GetLayerBumpTexture(int i) const {
                if (i < 0 || i > textures.size()) {
                    return 0;
                }
                return bumpTextures[i];
            }
            vector<GLuint> GetBumpTexturesVector() const {
                return bumpTextures;
            }

            void SetGLTexture(GLuint t) {
                glTexture = t;
            }
            GLuint GetGLTexture() const {
                return glTexture;
            }

            void SetIsGL(bool i) {
                isGL = i;
            }
            bool IsGL() const {
                return isGL;
            }

            void SetShadow(bool s) {
                shadow = s;
            }
            bool HasShadow() const {
                return shadow;
            }

            void SetColour(const Vector4& c) {
                colour = c;
            }
            Vector4 GetColour() const {
                return colour;
            }

            void SetAnimationObject(AnimationObject* newObject) {
                animationObject = newObject;
            }
            AnimationObject* GetAnimationObject() const {
                return animationObject;
            }

            void SetEmissive(bool e) {
                emissive = e;
            }
            int GetEmissive() const {
                return emissive;
            }

            float GetCameraDistance() const {
                return distanceFromCamera;
            }
            void SetCameraDistance(float f) {
                distanceFromCamera = f;
            }

            float GetTextureRotate() const {
                return textureRotate;
            }
            void SetTextureRotate(float r) {
                textureRotate = r;
            }

            float GetTextureCycle() const {
                return textureCycle;
            }
            void SetTextureCycle(float c) {
                textureCycle = c;
            }

        protected:
            Mesh* mesh;
            OBJMesh* objMesh;
            Texture* texture;
            Texture* bumpTexture;
            Texture* specTexture;
            GLuint glTexture;
            ShaderGroup* shaderGroup;
            vector<GLuint> textures;
            vector<GLuint> bumpTextures;
            Transform* transform;
            Vector4	          colour;
            AnimationObject* animationObject;
            int               drawMode;
            bool              emissive;
            bool              isGL = false;
            bool              shadow = true;
            float             distanceFromCamera = 0.0f;
            float             textureRotate;
            float             textureCycle;
        };
    }
}
