/******************************************************************************
This file is part of the Newcastle OpenGL Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Mesh.h"
#include "OBJMesh.h"
#include "OGLTexture.h"
#include "glad\gl.h"
#include "SOIL\SOIL.h"

namespace NCL::Rendering {	
	using UniqueOGLMesh		= std::unique_ptr<class OGLMesh>;
	using SharedOGLMesh		= std::shared_ptr<class OGLMesh>;

	class OGLMesh : public Mesh	{
	public:
		friend class OGLRenderer;
		OGLMesh();
		~OGLMesh();

		void RecalculateNormals();

		void UploadToGPU(Rendering::RendererBase* renderer = nullptr) override;
		void UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount);
		static OGLMesh* GenerateQuad();

	protected:
		GLuint	GetVAO()			const { return vao;			}
		void	BindVertexAttribute(int attribSlot, int bufferID, int bindingID, int elementCount, int elementSize, int elementOffset);

		GLuint vao;
		GLuint attributeBuffers[VertexAttribute::MAX_ATTRIBUTES];
		GLuint indexBuffer;
	};

	struct MTLInfo {
		string bump;
		string diffuse;

		GLuint bumpNum;
		GLuint diffuseNum;

		MTLInfo() {
			bumpNum = 0;
			diffuseNum = 0;
		}
	};

	class OGLOBJMesh : public OBJMesh {
	public:
		friend class OGLRenderer;
		OGLOBJMesh();
		~OGLOBJMesh();

		void RecalculateNormals();
		void GenerateTangents();
		Vector4 GenerateTangent(int a, int b, int c);

		void UploadToGPU(Rendering::RendererBase* renderer = nullptr) override;
		void UploadOBJMesh(Rendering::RendererBase* renderer, std::vector<OBJSubMesh*> inputSubMeshes, std::vector<Vector3>inputVertices, std::vector<Vector2>inputTexCoords, std::vector<Vector3>inputNormals);
		void UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount);
	protected:
		GLuint	GetVAO()			const { return vao; }
		void	BindVertexAttribute(int attribSlot, int bufferID, int bindingID, int elementCount, int elementSize, int elementOffset);

		map <string, MTLInfo> materials;
		void SetTexturesFromMTL(string& mtlFile, string& mtlType);
		GLuint vao;
		GLuint attributeBuffers[VertexAttribute::MAX_ATTRIBUTES];
		GLuint indexBuffer;
	};
}