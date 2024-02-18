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

            void SetMaterial(MeshMaterial* m) {
                material = m;
            }
            MeshMaterial* GetMaterial() const {
                return material;
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

            float GetCameraDistance() const { return distanceFromCamera; }
            void SetCameraDistance(float f) { distanceFromCamera = f; }

        protected:
            Mesh*             mesh;
            OBJMesh*          objMesh;
            Texture*          texture;
            Texture*          bumpTexture;
            Texture*          specTexture;
            ShaderGroup*      shaderGroup;
            MeshMaterial*     material;
            vector<GLuint>    textures;
            Transform*        transform;
            Vector4	          colour;
            AnimationObject*  animationObject;
            int               drawMode;
            bool              emissive;
            float             distanceFromCamera = 0.0f;
        };
    }
}
