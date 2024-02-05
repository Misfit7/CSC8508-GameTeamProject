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
			RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader, bool isOBJ = false);
			RenderObject(Transform* parentTransform, OBJMesh* mesh, Texture* tex, Shader* shader, bool isOBJ = true);
			~RenderObject();

			void SetDefaultTexture(Texture* t) {
				texture = t;
			}

			Texture* GetDefaultTexture() const {
				return texture;
			}

			Mesh*	GetMesh() const {
				return mesh;
			}

			OBJMesh* GetOBJMesh() const {
				return objMesh;
			}

			bool IsOBJ() const {
				return isOBJ;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			Shader*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

		protected:
			Mesh*		mesh;
			OBJMesh*    objMesh;
			Texture*	texture;
			Shader*		shader;
			Transform*	transform;
			Vector4		colour;
			bool isOBJ;
		};
	}
}
