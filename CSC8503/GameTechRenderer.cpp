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
    std::cout << std::endl << "--------Initialising Renderer--------" << std::endl;

    glEnable(GL_DEPTH_TEST);

    std::cout << std::endl << "--------Loading Shaders--------" << std::endl;
    debugShader = new OGLShader("debug.vert", "debug.frag");
    shadowShader = new OGLShader("shadow.vert", "shadow.frag");
    skinningShadowShader = new OGLShader("SkinningShadow.vert", "shadow.frag");
    pointLightShader = new OGLShader("PointLight.vert", "PointLight.frag");
    processShader = new OGLShader("process.vert", "process.frag");
    processCombineShader = new OGLShader("processCombine.vert", "processCombine.frag");
    combineShader = new OGLShader("combine.vert", "combine.frag");

    InitBuffers();

    isNight = false;
    isProcess = false;

    //Set up the light properties

    std::cout << std::endl << "--------Initialising Lights--------" << std::endl;
    sunLight = new Light(Vector3(-200.0f, 60.0f, -200.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 550.0f);
    redstoneLight1 = new Light(Vector3(10, 20, 0), Vector4(1, 1, 0, 1), 50.0f);
    redstoneLight2 = new Light(Vector3(30, 20, 40), Vector4(1, 0, 0, 1), 30.0f);
    redstoneLight3 = new Light(Vector3(60, 20, 20), Vector4(0, 1, 0, 1), 40.0f);
    sphere = LoadMesh("sphere.msh");
    quad = OGLMesh::GenerateQuad();

    glGenVertexArrays(1, &lineVAO);
    glGenVertexArrays(1, &textVAO);

    glGenBuffers(1, &lineVertVBO);
    glGenBuffers(1, &textVertVBO);
    glGenBuffers(1, &textColourVBO);
    glGenBuffers(1, &textTexVBO);

    Debug::CreateDebugFont("PressStart2P.fnt", *LoadTexture("PressStart2P.png"));

    SetDebugStringBufferSizes(10000);
    SetDebugLineBufferSizes(1000);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GameTechRenderer::~GameTechRenderer() {
    glDeleteTextures(1, &shadowTex);
    glDeleteFramebuffers(1, &shadowFBO);
}

void GameTechRenderer::InitBuffers() {   
    glGenFramebuffers(1, &worldFBO);
    glGenFramebuffers(1, &lightFBO);
    glGenFramebuffers(1, &shadowFBO);
    glGenFramebuffers(1, &skyboxFBO);
    glGenFramebuffers(1, &combinedFBO);
    glGenFramebuffers(1, &processFBO);

    GLenum buffers[6] = {
        GL_COLOR_ATTACHMENT0 ,
        GL_COLOR_ATTACHMENT1 ,
        GL_COLOR_ATTACHMENT2 ,
        GL_COLOR_ATTACHMENT3 ,
        GL_COLOR_ATTACHMENT4
    };

    LoadSkybox();
    GenerateShadowTexture();
    GenerateScreenTexture(skyboxBufferTex);
    GenerateScreenTexture(worldDepthTex, true);
    GenerateScreenTexture(worldColourTex);
    GenerateScreenTexture(worldNormalTex);
    GenerateScreenTexture(worldSpecTex);
    GenerateScreenTexture(worldEmisTex);
    GenerateScreenTexture(worldShadowTex);
    GenerateScreenTexture(lightDiffuseTex);
    GenerateScreenTexture(lightSpecularTex);
    for (int i = 0; i < 2; ++i) {
        GenerateScreenTexture(processColourTex[i]);
    }
    GenerateCombinedTexture();

    std::cout << std::endl << "--------Initialising Buffers--------" << std::endl;
    //World FBO
    glBindFramebuffer(GL_FRAMEBUFFER, worldFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, worldColourTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_2D, worldNormalTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
        GL_TEXTURE_2D, worldSpecTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
        GL_TEXTURE_2D, worldEmisTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4,
        GL_TEXTURE_2D, worldShadowTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_TEXTURE_2D, worldDepthTex, 0);
    processColourTex[0] = worldColourTex;
    glDrawBuffers(5, buffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "bufferFBO Load Failed!!" << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Light FBO
    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, lightDiffuseTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_2D, lightSpecularTex, 0);
    glDrawBuffers(2, buffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "pointLightFBO Load Failed!!" << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Shadow FBO
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Skybox FBO
    glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, skyboxBufferTex, 0);
    glDrawBuffers(1, buffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "skyboxFBO Load Failed!!" << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Combined FBO
    glBindFramebuffer(GL_FRAMEBUFFER, combinedFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, combinedTex, 0);
    glDrawBuffers(1, buffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "correctionFBO Load Failed!!" << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Process FBO
    glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, processColourTex[1], 0);
    glDrawBuffers(1, buffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE || !processColourTex[1]) {
        std::cout << "processFBO Load Failed!!" << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1, 1, 1, 1);
}

void GameTechRenderer::GenerateShadowTexture() {
    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GameTechRenderer::GenerateScreenTexture(GLuint& into, bool depth) {
    glGenTextures(1, &into);
    glBindTexture(GL_TEXTURE_2D, into);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint format1 = depth ? GL_DEPTH24_STENCIL8 : GL_RGBA8;
    GLuint type = depth ? GL_DEPTH_STENCIL : GL_RGBA;
    GLuint byte = depth ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE;

    glTexImage2D(GL_TEXTURE_2D, 0,
        format1, width, height, 0, type, byte, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void GameTechRenderer::GenerateCombinedTexture() {
    glGenTextures(1, &combinedTex);
    glBindTexture(GL_TEXTURE_2D, combinedTex);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0,
        GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void GameTechRenderer::ClearAllBuffers() {
    glBindFramebuffer(GL_FRAMEBUFFER, worldFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, combinedFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void GameTechRenderer::LoadSkybox() {
    std::cout << std::endl << "--------Loading Skybox--------" << std::endl;
    skyboxShader = new OGLShader("skybox.vert", "skybox.frag");
    skyboxMesh = new OGLMesh();
    skyboxMesh->SetVertexPositions({ Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
    skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
    skyboxMesh->UploadToGPU();

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
        SOIL_free_image_data(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::RenderFrame() {
    glEnable(GL_CULL_FACE);
    glClearColor(1, 1, 1, 1);
    BuildObjectList();
    SortObjectList();
    ClearAllBuffers();
    RenderShadowMap();
    RenderSkybox();
    RenderCamera();

    if (isNight) {
        DrawLightBuffer();
        CombineBuffers();
    }
    if (isProcess)
    {
        DrawProcess();
        ProcessCombine();
    }

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
            /*else {
                std::cout << "1" << std::endl;
            }*/
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

    auto DrawShadowMap = [&](const auto& i)
        {
            if ((*i).GetDrawMode() != 3) {
                BindShader(*shadowShader);
                int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

                Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(sunLight->GetPosition(), Vector3(0, 0, 0), Vector3(0, 1, 0));
                Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

                Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

                shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

                Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
                Matrix4 mvpMatrix = mvMatrix * modelMatrix;
                glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
                if ((*i).GetDrawMode() == 1) {
                   Draw((*i).GetMesh());             
                }
                else if ((*i).GetDrawMode() == 2) {
                    OBJDraw((*i).GetOBJMesh());
                }
            }
            else {
                BindShader(*skinningShadowShader);

                int projLocation = glGetUniformLocation(skinningShadowShader->GetProgramID(), "projMatrix");
                int viewLocation = glGetUniformLocation(skinningShadowShader->GetProgramID(), "viewMatrix");
                int modelLocation = glGetUniformLocation(skinningShadowShader->GetProgramID(), "modelMatrix");

                Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(sunLight->GetPosition(), Vector3(0, 0, 0), Vector3(0, 1, 0));
                Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

                Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

                shadowMatrix = biasMatrix * mvMatrix;

                Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();

                glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);
                glUniformMatrix4fv(viewLocation, 1, false, (float*)&shadowViewMatrix);
                glUniformMatrix4fv(projLocation, 1, false, (float*)&shadowProjMatrix);

                vector<Matrix4> frameMatrices;

                const vector<Matrix4> invBindPose = (*i).GetMesh()->GetInverseBindPose();
                const Matrix4* frameData = (*i).GetAnimationObject()->GetActiveAnim()->GetJointData((*i).GetAnimationObject()->GetCurrentFrame());

                for (unsigned int j = 0; j < (*i).GetMesh()->GetJointCount(); ++j) {
                    frameMatrices.emplace_back(frameData[j] * invBindPose[j]);
                }

                int j = glGetUniformLocation(skinningShadowShader->GetProgramID(), "joints");
                glUniformMatrix4fv(j, frameMatrices.size(), false,
                    (float*)frameMatrices.data());

                BindMesh((OGLMesh&)*(*i).GetMesh());
                for (int j = 0; j < (*i).GetMesh()->GetSubMeshCount(); ++j) {
                    DrawSubMesh(j);
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
    if (isNight) {
        glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
    }
    if (isProcess)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, worldFBO);
    }

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

    glUniform1i(texLocation, 6);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    Draw(skyboxMesh, false);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::RenderCamera() {
    if (isNight || isProcess) {
        glBindFramebuffer(GL_FRAMEBUFFER, worldFBO);
    }

    Matrix4 viewMatrix = gameWorld.GetMainCamera().BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera().BuildProjectionMatrix(hostWindow.GetScreenAspect());

    frameFrustum.FromViewProjMatrix(projMatrix * viewMatrix,
        gameWorld.GetMainCamera().GetNearPlane(), gameWorld.GetMainCamera().GetFarPlane());

    OGLShader* activeShader = nullptr;
    int projLocation = 0;
    int viewLocation = 0;
    int modelLocation = 0;
    int colourLocation = 0;
    int hasVColLocation = 0;
    int hasTexLocation = 0;
    int shadowLocation = 0;
    int cameraLocation = 0;

    //TODO - PUT IN FUNCTION
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    auto DrawObject = [&](const auto& i)
        {
            ShaderGroup* shaderGroup = (*i).GetShaderGroup();
            OGLShader* shader = nullptr;
            if (isNight) shader = (OGLShader*)shaderGroup->GetNightShader();
            else shader = (OGLShader*)shaderGroup->GetDayShader();
            BindShader(*shader);

            if (activeShader != shader) {
                projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
                viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
                modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
                shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
                colourLocation = glGetUniformLocation(shader->GetProgramID(), "objectColour");
                hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
                hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");
                cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");

                Vector3 camPos = gameWorld.GetMainCamera().GetPosition();
                glUniform3fv(cameraLocation, 1, &camPos.x);

                glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
                glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

                if (!isNight) SetShaderLight(*sunLight);
                else glUniform1i(glGetUniformLocation(shader->GetProgramID(), "emissive"), (*i).GetEmissive());

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

            if ((*i).GetDrawMode() == 1) {
                if ((*i).GetDefaultTexture()) {
                    BindTextureToShader(*(OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);
                    if ((*i).GetBumpTexture()) BindTextureToShader(*(OGLTexture*)(*i).GetBumpTexture(), "bumpTex", 2);
                    if ((*i).GetSpecTexture()) BindTextureToShader(*(OGLTexture*)(*i).GetSpecTexture(), "specTex", 3);
                }
                glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);

                glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

                Draw((*i).GetMesh());
            }
            else if ((*i).GetDrawMode() == 2) {
                if ((*i).GetOBJMesh()->GetOBJTexture()) {
                    BindGLuintTextureToShader((*i).GetOBJMesh()->GetOBJTexture(), "mainTex", 0);
                }

                glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetOBJMesh()->GetOBJTexture() ? 1 : 0);

                glUniform1i(hasVColLocation, !(*i).GetOBJMesh()->GetColourData().empty());

                OBJDraw((*i).GetOBJMesh());
            }
            else if ((*i).GetDrawMode() == 3) {
                vector<Matrix4> frameMatrices;

                const vector<Matrix4> invBindPose = (*i).GetMesh()->GetInverseBindPose();
                const Matrix4* frameData = (*i).GetAnimationObject()->GetActiveAnim()->GetJointData((*i).GetAnimationObject()->GetCurrentFrame());

                for (unsigned int j = 0; j < (*i).GetMesh()->GetJointCount(); ++j) {
                    frameMatrices.emplace_back(frameData[j] * invBindPose[j]);
                }

                int j = glGetUniformLocation(activeShader->GetProgramID(), "joints");
                glUniformMatrix4fv(j, frameMatrices.size(), false,
                    (float*)frameMatrices.data());
                BindMesh((OGLMesh&)*(*i).GetMesh());

                for (int j = 0; j < (*i).GetMesh()->GetSubMeshCount(); ++j) {
                    BindGLuintTextureToShader((*i).GetLayerTexture(j), "mainTex", 0);
                    if(!(*i).GetBumpTexturesVector().empty()) 
                        BindGLuintTextureToShader((*i).GetLayerBumpTexture(j), "bumpTex", 2);
                    glUniform1i(hasTexLocation, 1);
                    glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());
                    DrawSubMesh(j);
                }
            }
        };

    for (const auto& i : activeObjects) {
        DrawObject(i);
    }
    for (const auto& i : transparentActiveObjects) {
        DrawObject(i);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::DrawLightBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendFunc(GL_ONE, GL_ONE);
    glCullFace(GL_FRONT);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);

    DrawPointLights();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::DrawPointLights() {
    BindShader(*pointLightShader);

    glUniform1i(glGetUniformLocation(
        activeShader->GetProgramID(), "depthTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, worldDepthTex);

    glUniform1i(glGetUniformLocation(
        activeShader->GetProgramID(), "normTex"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, worldNormalTex);

    glUniform1i(glGetUniformLocation(
        activeShader->GetProgramID(), "specTex"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, worldSpecTex);

    Matrix4 viewMatrix = gameWorld.GetMainCamera().BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera().BuildProjectionMatrix(hostWindow.GetScreenAspect());
    Vector3 camPos = gameWorld.GetMainCamera().GetPosition();

    glUniform3fv(glGetUniformLocation(activeShader->GetProgramID(),
        "cameraPos"), 1, (float*)&camPos);

    glUniform2f(glGetUniformLocation(activeShader->GetProgramID(),
        "pixelSize"), 1.0f / width, 1.0f / height);

    Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
    glUniformMatrix4fv(glGetUniformLocation(
        activeShader->GetProgramID(), "inverseProjView"),
        1, false, (float*)&invViewProj);

    int projLocation = glGetUniformLocation(activeShader->GetProgramID(), "projMatrix");
    int viewLocation = glGetUniformLocation(activeShader->GetProgramID(), "viewMatrix");

    glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
    glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

    SetShaderLight(*redstoneLight1);
    Draw(sphere);

    SetShaderLight(*redstoneLight2);
    Draw(sphere);

    SetShaderLight(*redstoneLight3);
    Draw(sphere);
}

void GameTechRenderer::SetShaderLight(const Light& l) {
    Vector3 lightPosition = l.GetPosition();
    Vector4 lightColour = l.GetColour();

    glUniform3fv(glGetUniformLocation(activeShader->GetProgramID(),
        "lightPos"), 1, (float*)&lightPosition);

    glUniform4fv(glGetUniformLocation(activeShader->GetProgramID(),
        "lightColour"), 1, (float*)&lightColour);

    glUniform1f(glGetUniformLocation(activeShader->GetProgramID(),
        "lightRadius"), l.GetRadius());
}

void GameTechRenderer::DrawProcess() {
    glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processColourTex[1], 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    BindMesh((OGLMesh&)*quad);

    BindShader(*processShader);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "sceneTex"), 0);

    for (int i = 0; i < 1; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processColourTex[1], 0);
        glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "isVertical"), 0);
        glBindTexture(GL_TEXTURE_2D, processColourTex[0]);
        DrawBoundMesh();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processColourTex[0], 0);
        glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "isVertical"), 1);
        glBindTexture(GL_TEXTURE_2D, processColourTex[1]);
        DrawBoundMesh();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::ProcessCombine() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    BindShader(*processCombineShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, processColourTex[0]);
    glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "diffuseTex"), 0);

    SetShaderLight(*sunLight);

    Draw(quad, false);

}

void GameTechRenderer::CombineBuffers() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    BindShader(*combineShader);

    glUniform1i(glGetUniformLocation(
        combineShader->GetProgramID(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, worldColourTex);

    glUniform1i(glGetUniformLocation(
        combineShader->GetProgramID(), "diffuseLight"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

    glUniform1i(glGetUniformLocation(
        combineShader->GetProgramID(), "specularLight"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

    glUniform1i(glGetUniformLocation(
        combineShader->GetProgramID(), "emissiveLight"), 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, worldEmisTex);

    glUniform1i(glGetUniformLocation(
        combineShader->GetProgramID(), "skyboxTex"), 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, skyboxBufferTex);

    glUniform1i(glGetUniformLocation(
        combineShader->GetProgramID(), "depthTex"), 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, worldDepthTex);

    int cameraLocation = glGetUniformLocation(activeShader->GetProgramID(), "cameraPos");
    Vector3 camPos = gameWorld.GetMainCamera().GetPosition();
    glUniform3fv(cameraLocation, 1, &camPos.x);

    SetShaderLight(*sunLight);

    Draw(quad, false);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::Draw(Mesh* mesh, bool multilayer) {
    if (multilayer) {
        BindMesh((OGLMesh&)*mesh);
        size_t layerCount = mesh->GetSubMeshCount();
        for (size_t i = 0; i < layerCount; ++i) {
            DrawBoundMesh((uint32_t)i);
        }
    }
    else {
        BindMesh((OGLMesh&)*mesh);
        DrawBoundMesh();
    }
}

void GameTechRenderer::OBJDraw(OBJMesh* mesh) {
    BindOBJMesh((OGLOBJMesh&)*mesh);
    DrawBoundOBJMesh();
    for (unsigned int j = 0; j < mesh->GetChildrenList().size(); ++j) {
        BindOBJMesh((OGLOBJMesh&)*mesh->GetChildrenList().at(j));
        DrawBoundOBJMesh();
    }
}

void GameTechRenderer::ToggleNight() {
    isNight = !isNight;
}

void GameTechRenderer::ToggleProcess() {
    isProcess = !isProcess;
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