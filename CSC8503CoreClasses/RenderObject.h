#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"

namespace NCL {
    using namespace NCL::Rendering;

    namespace CSC8503 {
        class Transform;
        using namespace Maths;

        class RenderObject
        {
        public:
            RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, ShaderGroup* shaderGroup, bool isOBJ = false);
            RenderObject(Transform* parentTransform, OBJMesh* mesh, Texture* tex, ShaderGroup* shaderGroup, bool isOBJ = true);
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

            bool IsOBJ() const {
                return isOBJ;
            }

            Transform* GetTransform() const {
                return transform;
            }

            ShaderGroup* GetShaderGroup() const {
                return shaderGroup;
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

        protected:
            Mesh*         mesh;
            OBJMesh*      objMesh;
            Texture*      texture;
            Texture*      bumpTexture;
            Texture*      specTexture;
            ShaderGroup*  shaderGroup;
            Transform*    transform;
            Vector4	      colour;
            bool          isOBJ;
            bool          emissive;
            float         distanceFromCamera = 0.0f;

        };
    }
}
