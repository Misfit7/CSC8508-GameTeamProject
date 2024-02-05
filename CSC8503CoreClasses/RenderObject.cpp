#include "RenderObject.h"
#include "Mesh.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader, bool isObj) {
	if (!tex) {
		bool a = true;
	}
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	this->isOBJ     = isObj;
}

RenderObject::RenderObject(Transform* parentTransform, OBJMesh* mesh, Texture* tex, Shader* shader, bool isObj) {
	if (!tex) {
		bool a = true;
	}
	this->transform = parentTransform;
	this->objMesh = mesh;
	this->texture = tex;
	this->shader = shader;
	this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	this->isOBJ  = isObj;
}

RenderObject::~RenderObject() {

}