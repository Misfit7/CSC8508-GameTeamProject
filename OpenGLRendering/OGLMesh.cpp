/******************************************************************************
This file is part of the Newcastle OpenGL Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*/////////////////////////////////////////////////////////////////////////////
#include "OGLMesh.h"
#include "Assets.h"

using namespace NCL;
using namespace NCL::Rendering;
using namespace NCL::Maths;

OGLMesh::OGLMesh() {
	vao			= 0;

	for (int i = 0; i < VertexAttribute::MAX_ATTRIBUTES; ++i) {
		attributeBuffers[i] = 0;
	}
	indexBuffer = 0;
}

OGLMesh::~OGLMesh()	{
	glDeleteVertexArrays(1, &vao);			//Delete our VAO
	glDeleteBuffers(VertexAttribute::MAX_ATTRIBUTES, attributeBuffers);	//Delete our VBOs
	glDeleteBuffers(1, &indexBuffer);	//Delete our indices
}

void CreateVertexBuffer(GLuint& buffer, int byteCount, char* data) {
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, byteCount, data, GL_STATIC_DRAW);
}

void OGLMesh::BindVertexAttribute(int attribSlot, int buffer, int bindingID, int elementCount, int elementSize, int elementOffset) {
	glEnableVertexAttribArray(attribSlot);
	glVertexAttribFormat(attribSlot, elementCount, GL_FLOAT, false, 0);
	glVertexAttribBinding(attribSlot, bindingID);

	glBindVertexBuffer(bindingID, buffer, elementOffset, elementSize);
}

void OGLMesh::UploadToGPU(Rendering::RendererBase* renderer) {
	if (!ValidateMeshData()) {
		return;
	}
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	int numVertices = GetVertexCount();
	int numIndices	= GetIndexCount();

	if (!GetPositionData().empty()) {
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Positions], numVertices * sizeof(Vector3), (char*)GetPositionData().data());
		BindVertexAttribute(VertexAttribute::Positions, attributeBuffers[VertexAttribute::Positions], VertexAttribute::Positions, 3, sizeof(Vector3), 0);
	}

	if (!GetColourData().empty()) {	//buffer colour data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Colours], numVertices * sizeof(Vector4), (char*)GetColourData().data());
		BindVertexAttribute(VertexAttribute::Colours, attributeBuffers[VertexAttribute::Colours], VertexAttribute::Colours, 4, sizeof(Vector4), 0);
	}
	if (!GetTextureCoordData().empty()) {	//Buffer texture data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::TextureCoords], numVertices * sizeof(Vector2), (char*)GetTextureCoordData().data());
		BindVertexAttribute(VertexAttribute::TextureCoords, attributeBuffers[VertexAttribute::TextureCoords], VertexAttribute::TextureCoords, 2, sizeof(Vector2), 0);
	}

	if (!GetNormalData().empty()) {	//Buffer normal data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Normals], numVertices * sizeof(Vector3), (char*)GetNormalData().data());
		BindVertexAttribute(VertexAttribute::Normals, attributeBuffers[VertexAttribute::Normals], VertexAttribute::Normals, 3, sizeof(Vector3), 0);
	}

	if (!GetTangentData().empty()) {	//Buffer tangent data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Tangents], numVertices * sizeof(Vector4), (char*)GetTangentData().data());
		BindVertexAttribute(VertexAttribute::Tangents, attributeBuffers[VertexAttribute::Tangents], VertexAttribute::Tangents, 4, sizeof(Vector4), 0);
	}

	if (!GetSkinWeightData().empty()) {	//Skeleton weights
		CreateVertexBuffer(attributeBuffers[VertexAttribute::JointWeights], numVertices * sizeof(Vector4), (char*)GetSkinWeightData().data());
		BindVertexAttribute(VertexAttribute::JointWeights, attributeBuffers[VertexAttribute::JointWeights], VertexAttribute::JointWeights, 4, sizeof(Vector4), 0);
	}

	if (!GetSkinIndexData().empty()) {	//Skeleton joint indices
		CreateVertexBuffer(attributeBuffers[VertexAttribute::JointIndices], numVertices * sizeof(Vector4), (char*)GetSkinIndexData().data());
		BindVertexAttribute(VertexAttribute::JointIndices, attributeBuffers[VertexAttribute::JointIndices], VertexAttribute::JointIndices, 4, sizeof(Vector4), 0);
	}

	if (!GetIndexData().empty()) {		//buffer index data
		glGenBuffers(1, &attributeBuffers[VertexAttribute::MAX_ATTRIBUTES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attributeBuffers[VertexAttribute::MAX_ATTRIBUTES]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), (int*)GetIndexData().data(), GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

void OGLMesh::UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount) {
	if (!GetPositionData().empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Positions]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector3), vertexCount * sizeof(Vector3), (char*)&GetPositionData()[startVertex]);
	}

	if (!GetColourData().empty()) {	//buffer colour data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Colours]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetColourData()[startVertex]);
	}
	if (!GetTextureCoordData().empty()) {	//Buffer texture data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::TextureCoords]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector2), vertexCount * sizeof(Vector2), (char*)&GetTextureCoordData()[startVertex]);
	}

	if (!GetNormalData().empty()) {	//Buffer normal data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Normals]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector3), vertexCount * sizeof(Vector3), (char*)&GetNormalData()[startVertex]);
	}

	if (!GetTangentData().empty()) {	//Buffer tangent data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Tangents]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetTangentData()[startVertex]);
	}

	if (!GetSkinWeightData().empty()) {	//Skeleton weights
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::JointWeights]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetSkinWeightData()[startVertex]);
	}

	if (!GetSkinIndexData().empty()) {	//Skeleton joint indices
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::TextureCoords]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4i), vertexCount * sizeof(Vector4i), (char*)&GetSkinIndexData()[startVertex]);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void OGLMesh::RecalculateNormals() {
	normals.clear();

	if (indices.size() > 0) {
		for (size_t i = 0; i < positions.size(); i++) {
			normals.emplace_back(Vector3());
		}

		for (size_t i = 0; i < indices.size(); i += 3) {
			Vector3& a = positions[indices[i+0]];
			Vector3& b = positions[indices[i+1]];
			Vector3& c = positions[indices[i+2]];

			Vector3 normal = Vector3::Cross(b - a, c - a);
			normal.Normalise();

			normals[indices[i + 0]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}
		for (size_t i = 0; i < normals.size(); ++i) {
			normals[i].Normalise();
		}
	}
	else {

	}
}

OGLOBJMesh::OGLOBJMesh() {
	vao = 0;

	for (int i = 0; i < VertexAttribute::MAX_ATTRIBUTES; ++i) {
		attributeBuffers[i] = 0;
	}
	indexBuffer = 0;
}

OGLOBJMesh::~OGLOBJMesh() {
	glDeleteVertexArrays(1, &vao);			//Delete our VAO
	glDeleteBuffers(VertexAttribute::MAX_ATTRIBUTES, attributeBuffers);	//Delete our VBOs
	glDeleteBuffers(1, &indexBuffer);	//Delete our indices
}

void OGLOBJMesh::BindVertexAttribute(int attribSlot, int buffer, int bindingID, int elementCount, int elementSize, int elementOffset) {
	glEnableVertexAttribArray(attribSlot);
	glVertexAttribFormat(attribSlot, elementCount, GL_FLOAT, false, 0);
	glVertexAttribBinding(attribSlot, bindingID);

	glBindVertexBuffer(bindingID, buffer, elementOffset, elementSize);
}

void OGLOBJMesh::UploadToGPU(Rendering::RendererBase* renderer) {
	if (!ValidateMeshData()) {
		return;
	}
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	int numVertices = GetVertexCount();
	int numIndices = GetIndexCount();

	if (!GetPositionData().empty()) {
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Positions], numVertices * sizeof(Vector3), (char*)GetPositionData().data());
		BindVertexAttribute(VertexAttribute::Positions, attributeBuffers[VertexAttribute::Positions], VertexAttribute::Positions, 3, sizeof(Vector3), 0);
	}

	if (!GetColourData().empty()) {	//buffer colour data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Colours], numVertices * sizeof(Vector4), (char*)GetColourData().data());
		BindVertexAttribute(VertexAttribute::Colours, attributeBuffers[VertexAttribute::Colours], VertexAttribute::Colours, 4, sizeof(Vector4), 0);
	}
	if (!GetTextureCoordData().empty()) {	//Buffer texture data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::TextureCoords], numVertices * sizeof(Vector2), (char*)GetTextureCoordData().data());
		BindVertexAttribute(VertexAttribute::TextureCoords, attributeBuffers[VertexAttribute::TextureCoords], VertexAttribute::TextureCoords, 2, sizeof(Vector2), 0);
	}

	if (!GetNormalData().empty()) {	//Buffer normal data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Normals], numVertices * sizeof(Vector3), (char*)GetNormalData().data());
		BindVertexAttribute(VertexAttribute::Normals, attributeBuffers[VertexAttribute::Normals], VertexAttribute::Normals, 3, sizeof(Vector3), 0);
	}

	if (!GetTangentData().empty()) {	//Buffer tangent data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Tangents], numVertices * sizeof(Vector4), (char*)GetTangentData().data());
		BindVertexAttribute(VertexAttribute::Tangents, attributeBuffers[VertexAttribute::Tangents], VertexAttribute::Tangents, 4, sizeof(Vector4), 0);
	}

	if (!GetSkinWeightData().empty()) {	//Skeleton weights
		CreateVertexBuffer(attributeBuffers[VertexAttribute::JointWeights], numVertices * sizeof(Vector4), (char*)GetSkinWeightData().data());
		BindVertexAttribute(VertexAttribute::JointWeights, attributeBuffers[VertexAttribute::JointWeights], VertexAttribute::JointWeights, 4, sizeof(Vector4), 0);
	}

	if (!GetSkinIndexData().empty()) {	//Skeleton joint indices
		CreateVertexBuffer(attributeBuffers[VertexAttribute::JointIndices], numVertices * sizeof(Vector4), (char*)GetSkinIndexData().data());
		BindVertexAttribute(VertexAttribute::JointIndices, attributeBuffers[VertexAttribute::JointIndices], VertexAttribute::JointIndices, 4, sizeof(Vector4), 0);
	}

	if (!GetIndexData().empty()) {		//buffer index data
		glGenBuffers(1, &attributeBuffers[VertexAttribute::MAX_ATTRIBUTES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attributeBuffers[VertexAttribute::MAX_ATTRIBUTES]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), (int*)GetIndexData().data(), GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

void OGLOBJMesh::UploadOBJMesh(Rendering::RendererBase* renderer, std::vector<OBJSubMesh*> inputSubMeshes, std::vector<Vector3>inputVertices, std::vector<Vector2>inputTexCoords, std::vector<Vector3>inputNormals) {
	//We now have all our mesh data loaded in... Now to convert it into OpenGL vertex buffers!
	for (unsigned int i = 0; i < inputSubMeshes.size(); ) {
		OBJSubMesh* sm = inputSubMeshes[i];

		/*
		We're going to be lazy and turn the indices into an absolute list
		of vertex attributes (it makes handling the submesh list easier)
		*/
		if (sm->vertIndices.empty()) {
			delete sm;
			inputSubMeshes.erase(inputSubMeshes.begin() + i);
			continue;
		}
		else {
			OGLOBJMesh* m;

			if (i == 0) {
				m = this;
			}
			else {
				m = new OGLOBJMesh();
			}

			m->SetTexturesFromMTL(sm->mtlSrc, sm->mtlType);

			for (unsigned int j = 0; j < sm->vertIndices.size(); ++j) {
				m->positions.push_back(inputVertices[sm->vertIndices[j] - 1]);
			}

			if (!sm->texIndices.empty()) {
				for (unsigned int j = 0; j < sm->texIndices.size(); ++j) {
					m->texCoords.push_back(inputTexCoords[sm->texIndices[j] - 1]);
				}
			}

			if (!sm->normIndices.empty()) {
				for (unsigned int j = 0; j < sm->normIndices.size(); ++j) {
					m->normals.push_back(inputNormals[sm->normIndices[j] - 1]);
				}
			}

			//m->GenerateTangents();

			m->UploadToGPU();

			if (i != 0) {
				OBJAddChild(m);
			}
		}
		delete inputSubMeshes[i];
		++i;
	}
}

void OGLOBJMesh::SetTexturesFromMTL(string& mtlFile, string& mtlType) {
	if (mtlType.empty() || mtlFile.empty()) {
		return;
	}

	map <string, MTLInfo>::iterator i = materials.find(mtlType);

	if (i != materials.end()) {
		if (!i->second.diffuse.empty()) {
			texture = i->second.diffuseNum;
		}

		//if (!i->second.bump.empty()) {
		//	bumpTexture = i->second.bumpNum;
		//}

		return;
	}

	std::ifstream f(string(Assets::MESHDIR + mtlFile).c_str(), std::ios::in);

	if (!f) {//Oh dear, it can't find the file :(
		return;
	}

	MTLInfo currentMTL;
	string  currentMTLName;

	int mtlCount = 0;

	while (!f.eof()) {
		std::string currentLine;
		f >> currentLine;

		if (currentLine == MTLNEW) {
			if (mtlCount > 0) {
				materials.insert(std::make_pair(currentMTLName, currentMTL));
			}
			currentMTL.diffuse = "";
			currentMTL.bump = "";

			f >> currentMTLName;

			mtlCount++;
		}
		else if (currentLine == MTLDIFFUSEMAP) {
			f >> currentMTL.diffuse;

			if (currentMTL.diffuse.find_last_of('/') != string::npos) {
				int at = currentMTL.diffuse.find_last_of('/');
				currentMTL.diffuse = currentMTL.diffuse.substr(at + 1);
			}
			else if (currentMTL.diffuse.find_last_of('\\') != string::npos) {
				int at = currentMTL.diffuse.find_last_of('\\');
				currentMTL.diffuse = currentMTL.diffuse.substr(at + 1);
			}

			if (!currentMTL.diffuse.empty()) {
				currentMTL.diffuseNum = OGLTexture::TextureFromFile(string(Assets::TEXTUREDIR + currentMTL.diffuse).c_str()).release();
			}
		}
		//else if (currentLine == MTLBUMPMAP || currentLine == MTLBUMPMAPALT) {
		//	f >> currentMTL.bump;
		//
		//	if (currentMTL.bump.find_last_of('/') != string::npos) {
		//		int at = currentMTL.bump.find_last_of('/');
		//		currentMTL.bump = currentMTL.bump.substr(at + 1);
		//	}
		//	else if (currentMTL.bump.find_last_of('\\') != string::npos) {
		//		int at = currentMTL.bump.find_last_of('\\');
		//		currentMTL.bump = currentMTL.bump.substr(at + 1);
		//	}
		//
		//	if (!currentMTL.bump.empty()) {
		//		currentMTL.bumpNum = SOIL_load_OGL_texture(string(TEXTUREDIR + currentMTL.bump).c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
		//	}
		//}
		else {
			std::cout << "OBJMesh::LoadOBJMesh Unknown file data:" << currentLine << std::endl;
		}
	}

	materials.insert(std::make_pair(currentMTLName, currentMTL));

	SetTexturesFromMTL(mtlFile, mtlType);
}

void OGLOBJMesh::UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount) {
	if (!GetPositionData().empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Positions]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector3), vertexCount * sizeof(Vector3), (char*)&GetPositionData()[startVertex]);
	}

	if (!GetColourData().empty()) {	//buffer colour data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Colours]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetColourData()[startVertex]);
	}
	if (!GetTextureCoordData().empty()) {	//Buffer texture data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::TextureCoords]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector2), vertexCount * sizeof(Vector2), (char*)&GetTextureCoordData()[startVertex]);
	}

	if (!GetNormalData().empty()) {	//Buffer normal data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Normals]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector3), vertexCount * sizeof(Vector3), (char*)&GetNormalData()[startVertex]);
	}

	if (!GetTangentData().empty()) {	//Buffer tangent data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Tangents]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetTangentData()[startVertex]);
	}

	if (!GetSkinWeightData().empty()) {	//Skeleton weights
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::JointWeights]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetSkinWeightData()[startVertex]);
	}

	if (!GetSkinIndexData().empty()) {	//Skeleton joint indices
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::TextureCoords]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4i), vertexCount * sizeof(Vector4i), (char*)&GetSkinIndexData()[startVertex]);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void OGLOBJMesh::RecalculateNormals() {
	normals.clear();

	if (indices.size() > 0) {
		for (size_t i = 0; i < positions.size(); i++) {
			normals.emplace_back(Vector3());
		}

		for (size_t i = 0; i < indices.size(); i += 3) {
			Vector3& a = positions[indices[i + 0]];
			Vector3& b = positions[indices[i + 1]];
			Vector3& c = positions[indices[i + 2]];

			Vector3 normal = Vector3::Cross(b - a, c - a);
			normal.Normalise();

			normals[indices[i + 0]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}
		for (size_t i = 0; i < normals.size(); ++i) {
			normals[i].Normalise();
		}
	}
	else {

	}
}