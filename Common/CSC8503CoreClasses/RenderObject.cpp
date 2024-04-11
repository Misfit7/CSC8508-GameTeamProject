#include "RenderObject.h"
#include "Mesh.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, ShaderGroup* shader, int mode) {
    if (!tex) {
        bool a = true;
    }
    this->transform = parentTransform;
    this->mesh = mesh;
    this->texture = tex;
    this->shaderGroup = shader;
    this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->drawMode = mode;
    this->emissive = false;
    this->animationObject = nullptr;
}

RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, GLuint tex, ShaderGroup* shader, int mode) {
    if (!tex) {
        bool a = true;
    }
    this->transform = parentTransform;
    this->mesh = mesh;
    this->glTexture = tex;
    this->shaderGroup = shader;
    this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->drawMode = mode;
    this->emissive = false;
    this->animationObject = nullptr;
    this->isGL = true;
}

RenderObject::RenderObject(Transform* parentTransform, OBJMesh* mesh, Texture* tex, ShaderGroup* shader, int mode) {
    if (!tex) {
        bool a = true;
    }
    this->transform = parentTransform;
    this->objMesh = mesh;
    this->texture = tex;
    this->shaderGroup = shader;
    this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->drawMode = mode;
    this->emissive = false;
    this->animationObject = nullptr;
}

RenderObject::~RenderObject() {

}