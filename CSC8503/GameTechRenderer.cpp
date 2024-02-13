#include "GameTechRenderer.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "MshLoader.h"
#include "Assets.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

GameTechRenderer::GameTechRenderer(GameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world) {
    glEnable(GL_DEPTH_TEST);

    debugShader = new OGLShader("debug.vert", "debug.frag");
    shadowShader = new OGLShader("shadow.vert", "shadow.frag");

    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1, 1, 1, 1);

    //Set up the light properties
    lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
    lightRadius = 1000.0f;
    lightPosition = Vector3(-200.0f, 60.0f, -200.0f);

    //Skybox!
    skyboxShader = new OGLShader("skybox.vert", "skybox.frag");
    skyboxMesh = new OGLMesh();
    skyboxMesh->SetVertexPositions({ Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
    skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
    skyboxMesh->UploadToGPU();

    LoadSkybox();

    glGenVertexArrays(1, &lineVAO);
    glGenVertexArrays(1, &textVAO);

    glGenBuffers(1, &lineVertVBO);
    glGenBuffers(1, &textVertVBO);
    glGenBuffers(1, &textColourVBO);
    glGenBuffers(1, &textTexVBO);

    Debug::CreateDebugFont("PressStart2P.fnt", *LoadTexture("PressStart2P.png"));

    SetDebugStringBufferSizes(10000);
    SetDebugLineBufferSizes(1000);
}

GameTechRenderer::~GameTechRenderer() {
    glDeleteTextures(1, &shadowTex);
    glDeleteFramebuffers(1, &shadowFBO);
}

void GameTechRenderer::LoadSkybox() {
    std::string name[6] = {
        "../Assets/Textures/Cubemap/skyrender0004.png",
        "../Assets/Textures/Cubemap/skyrender0001.png",
        "../Assets/Textures/Cubemap/skyrender0003.png",
        "../Assets/Textures/Cubemap/skyrender0006.png",
        "../Assets/Textures/Cubemap/skyrender0002.png",
        "../Assets/Textures/Cubemap/skyrender0005.png"
    };

    GLuint axis[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    int iWidth, iHeight;

    for (int i = 0; i < 6; ++i) {
        unsigned char* image = SOIL_load_image(name[i].c_str(),
            &iWidth, &iHeight, 0, SOIL_LOAD_RGB);
        glTexImage2D(axis[i], 0, GL_RGB, iWidth, iHeight,
            0, GL_RGB, GL_UNSIGNED_BYTE, image);

        glTexParameteri(GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
        SOIL_free_image_data(image);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::RenderFrame() {
    glEnable(GL_CULL_FACE);
    glClearColor(1, 1, 1, 1);
    BuildObjectList();
    SortObjectList();
    RenderShadowMap();
    RenderSkybox();
    RenderCamera();
    glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    NewRenderLines();
    NewRenderText();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GameTechRenderer::CleanObjectList() {
    activeObjects.clear();
    transparentActiveObjects.clear();
}

void GameTechRenderer::BuildObjectList() {
    CleanObjectList();

    gameWorld.OperateOnContents(
        [&](GameObject* o) {
            if (frameFrustum.InsideFrustum(o->GetTransform().GetPosition(), o->GetBoundingRadius())
                && o->IsActive()) {
                RenderObject* g = o->GetRenderObject();
                Vector3 dir = o->GetTransform().GetPosition() - gameWorld.GetMainCamera().GetPosition();
                g->SetCameraDistance(Vector3::Dot(dir, dir));
                if (g->GetColour().w < 1.0f) {
                    transparentActiveObjects.emplace_back(g);
                }
                else {
                    activeObjects.emplace_back(g);
                }
            }
        }
    );
}

void GameTechRenderer::SortObjectList() {
    std::sort(transparentActiveObjects.rbegin(), transparentActiveObjects.rend(), [&](const auto& a, const auto& b) {
        return (a->GetCameraDistance() < b->GetCameraDistance()) ? true : false; });
    std::sort(activeObjects.begin(), activeObjects.end(), [&](const auto& a, const auto& b) {
        return (a->GetCameraDistance() < b->GetCameraDistance()) ? true : false; });
}

void GameTechRenderer::RenderShadowMap() {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

    glCullFace(GL_FRONT);

    BindShader(*shadowShader);
    int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

    Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0, 1, 0));
    Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

    Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

    shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

    auto DrawShadowMap = [&](const auto& i)
        {
            Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
            Matrix4 mvpMatrix = mvMatrix * modelMatrix;
            glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
            if (!(*i).IsOBJ()) {
                BindMesh((OGLMesh&)*(*i).GetMesh());
                size_t layerCount = (*i).GetMesh()->GetSubMeshCount();
                for (size_t i = 0; i < layerCount; ++i) {
                    DrawBoundMesh((uint32_t)i);
                }
            }
            else {
                BindOBJMesh((OGLOBJMesh&)*(*i).GetOBJMesh());
                DrawBoundOBJMesh();
                for (unsigned int j = 0; j < (*i).GetOBJMesh()->GetChildrenList().size(); ++j) {
                    BindOBJMesh((OGLOBJMesh&)*(*i).GetOBJMesh()->GetChildrenList().at(j));
                    DrawBoundOBJMesh();
                }
            }
        };

    for (const auto& i : activeObjects) {
        DrawShadowMap(i);
    }
    for (const auto& i : transparentActiveObjects) {
        DrawShadowMap(i);
    }

    glViewport(0, 0, windowSize.x, windowSize.y);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
}

void GameTechRenderer::RenderSkybox() {
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    Matrix4 viewMatrix = gameWorld.GetMainCamera().BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera().BuildProjectionMatrix(hostWindow.GetScreenAspect());

    BindShader(*skyboxShader);

    int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
    int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
    int texLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

    glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
    glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

    glUniform1i(texLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    BindMesh(*skyboxMesh);
    DrawBoundMesh();

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera() {
    Matrix4 viewMatrix = gameWorld.GetMainCamera().BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera().BuildProjectionMatrix(hostWindow.GetScreenAspect());

    OGLShader* activeShader = nullptr;
    int projLocation = 0;
    int viewLocation = 0;
    int modelLocation = 0;
    int colourLocation = 0;
    int hasVColLocation = 0;
    int hasTexLocation = 0;
    int shadowLocation = 0;

    int lightPosLocation = 0;
    int lightColourLocation = 0;
    int lightRadiusLocation = 0;

    int cameraLocation = 0;

    //TODO - PUT IN FUNCTION
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    auto DrawObject = [&](const auto& i)
        {
            OGLShader* shader = (OGLShader*)(*i).GetShader();
            BindShader(*shader);

            if (activeShader != shader) {
                projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
                viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
                modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
                shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
                colourLocation = glGetUniformLocation(shader->GetProgramID(), "objectColour");
                hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
                hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

                lightPosLocation = glGetUniformLocation(shader->GetProgramID(), "lightPos");
                lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
                lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");

                cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");

                Vector3 camPos = gameWorld.GetMainCamera().GetPosition();
                glUniform3fv(cameraLocation, 1, &camPos.x);

                glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
                glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

                glUniform3fv(lightPosLocation, 1, (float*)&lightPosition);
                glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
                glUniform1f(lightRadiusLocation, lightRadius);

                int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
                glUniform1i(shadowTexLocation, 1);

                activeShader = shader;
            }

            Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
            glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

            Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
            glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

            Vector4 colour = i->GetColour();
            glUniform4fv(colourLocation, 1, &colour.x);

            if (!(*i).IsOBJ()) {
                if ((*i).GetDefaultTexture()) {
                    BindTextureToShader(*(OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);
                    //SetTextureRepeating((*(OGLTexture*)(*i).GetDefaultTexture()).GetObjectID(), true);
                }
                glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);

                glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

                BindMesh((OGLMesh&)*(*i).GetMesh());
                size_t layerCount = (*i).GetMesh()->GetSubMeshCount();
                for (size_t i = 0; i < layerCount; ++i) {
                    DrawBoundMesh((uint32_t)i);
                }
            }
            else {
                if ((*i).GetOBJMesh()->GetOBJTexture()) {
                    BindOBJTextureToShader((*i).GetOBJMesh()->GetOBJTexture(), "mainTex", 0);
                    //SetTextureRepeating((*i).GetOBJMesh()->GetOBJTexture(), true);
                }

                glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetOBJMesh()->GetOBJTexture() ? 1 : 0);

                glUniform1i(hasVColLocation, !(*i).GetOBJMesh()->GetColourData().empty());

                BindOBJMesh((OGLOBJMesh&)*(*i).GetOBJMesh());
                DrawBoundOBJMesh();
                for (unsigned int j = 0; j < (*i).GetOBJMesh()->GetChildrenList().size(); ++j) {
                    BindOBJMesh((OGLOBJMesh&)*(*i).GetOBJMesh()->GetChildrenList().at(j));
                    DrawBoundOBJMesh();
                }
            }
        };

    for (const auto& i : activeObjects) {
        DrawObject(i);
    }
    for (const auto& i : transparentActiveObjects) {
        DrawObject(i);
    }
}

Mesh* GameTechRenderer::LoadMesh(const std::string& name) {
    OGLMesh* mesh = new OGLMesh();
    MshLoader::LoadMesh(name, *mesh);
    mesh->UploadToGPU();
    mesh->SetPrimitiveType(GeometryPrimitive::Triangles);
    return mesh;
}

OBJMesh* GameTechRenderer::LoadOBJMesh(const std::string& name) {
    OGLOBJMesh* mesh = new OGLOBJMesh();
    mesh->LoadOBJMesh(name);
    mesh->UploadOBJMesh(nullptr, mesh->GetTempSM(), mesh->GetTempV(), mesh->GetTempTC(), mesh->GetTempN());
    mesh->SetPrimitiveType(GeometryPrimitive::Triangles);
    return mesh;
}

void GameTechRenderer::NewRenderLines() {
    const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();
    if (lines.empty()) {
        return;
    }

    Matrix4 viewMatrix = gameWorld.GetMainCamera().BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera().BuildProjectionMatrix(hostWindow.GetScreenAspect());

    Matrix4 viewProj = projMatrix * viewMatrix;

    BindShader(*debugShader);
    int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
    GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
    glUniform1i(texSlot, 0);

    glUniformMatrix4fv(matSlot, 1, false, (float*)viewProj.array);

    debugLineData.clear();

    size_t frameLineCount = lines.size() * 2;

    SetDebugLineBufferSizes(frameLineCount);

    glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(Debug::DebugLineEntry), lines.data());


    glBindVertexArray(lineVAO);
    glDrawArrays(GL_LINES, 0, (GLsizei)frameLineCount);
    glBindVertexArray(0);
}

void GameTechRenderer::NewRenderText() {
    const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();
    if (strings.empty()) {
        return;
    }

    BindShader(*debugShader);

    OGLTexture* t = (OGLTexture*)Debug::GetDebugFont()->GetTexture();

    if (t) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t->GetObjectID());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        BindTextureToShader(*t, "mainTex", 0);
    }
    Matrix4 proj = Matrix4::Orthographic(0.0, 100.0f, 100, 0, -1.0f, 1.0f);

    int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
    glUniformMatrix4fv(matSlot, 1, false, (float*)proj.array);

    GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
    glUniform1i(texSlot, 1);

    debugTextPos.clear();
    debugTextColours.clear();
    debugTextUVs.clear();

    int frameVertCount = 0;
    for (const auto& s : strings) {
        frameVertCount += Debug::GetDebugFont()->GetVertexCountForString(s.data);
    }
    SetDebugStringBufferSizes(frameVertCount);

    for (const auto& s : strings) {
        float size = 20.0f;
        Debug::GetDebugFont()->BuildVerticesForString(s.data, s.position, s.colour, size, debugTextPos, debugTextUVs, debugTextColours);
    }

    glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector3), debugTextPos.data());
    glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector4), debugTextColours.data());
    glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector2), debugTextUVs.data());

    glBindVertexArray(textVAO);
    glDrawArrays(GL_TRIANGLES, 0, frameVertCount);
    glBindVertexArray(0);
}

Texture* GameTechRenderer::LoadTexture(const std::string& name) {
    return OGLTexture::TextureFromFile(name).release();
}

Shader* GameTechRenderer::LoadShader(const std::string& vertex, const std::string& fragment) {
    return new OGLShader(vertex, fragment);
}

void GameTechRenderer::SetDebugStringBufferSizes(size_t newVertCount) {
    if (newVertCount > textCount) {
        textCount = newVertCount;

        glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
        glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
        glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector4), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
        glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector2), nullptr, GL_DYNAMIC_DRAW);

        debugTextPos.reserve(textCount);
        debugTextColours.reserve(textCount);
        debugTextUVs.reserve(textCount);

        glBindVertexArray(textVAO);

        glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
        glVertexAttribBinding(0, 0);
        glBindVertexBuffer(0, textVertVBO, 0, sizeof(Vector3));

        glVertexAttribFormat(1, 4, GL_FLOAT, false, 0);
        glVertexAttribBinding(1, 1);
        glBindVertexBuffer(1, textColourVBO, 0, sizeof(Vector4));

        glVertexAttribFormat(2, 2, GL_FLOAT, false, 0);
        glVertexAttribBinding(2, 2);
        glBindVertexBuffer(2, textTexVBO, 0, sizeof(Vector2));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}

void GameTechRenderer::SetDebugLineBufferSizes(size_t newVertCount) {
    if (newVertCount > lineCount) {
        lineCount = newVertCount;

        glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
        glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(Debug::DebugLineEntry), nullptr, GL_DYNAMIC_DRAW);

        debugLineData.reserve(lineCount);

        glBindVertexArray(lineVAO);

        int realStride = sizeof(Debug::DebugLineEntry) / 2;

        glVertexAttribFormat(0, 3, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, start));
        glVertexAttribBinding(0, 0);
        glBindVertexBuffer(0, lineVertVBO, 0, realStride);

        glVertexAttribFormat(1, 4, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, colourA));
        glVertexAttribBinding(1, 0);
        glBindVertexBuffer(1, lineVertVBO, sizeof(Vector4), realStride);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
}