/******************************************************************************
This file is part of the Newcastle OpenGL Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "RendererBase.h"

#ifdef _WIN32
#include "windows.h"

#include "KHR\khrplatform.h"
#include "glad\gl.h"
#include "KHR/WGLext.h"
#endif

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif

namespace NCL {
    namespace Maths {
        class Matrix4;
    }

    namespace Rendering {
        class Mesh;
        class Shader;
        class Texture;

        class OGLMesh;
        class OGLOBJMesh;
        class OGLShader;
        class OGLTexture;

        class SimpleFont;

        class OGLRenderer : public RendererBase {
        public:
            friend class OGLRenderer;
            OGLRenderer(Window& w);
            ~OGLRenderer();

            void OnWindowResize(int w, int h)	override;
            bool HasInitialised()				const override {
                return initState;
            }

            virtual bool SetVerticalSync(VerticalSyncState s);
            static void SetTextureRepeating(GLuint texture, bool repeating);

        protected:
            int  width;
            int  height;

            void BeginFrame()	override;
            void RenderFrame()	override;
            void EndFrame()		override;
            void SwapBuffers()  override;

            void BindShader(const OGLShader& s);
            void BindTextureToShader(const OGLTexture& t, const std::string& uniform, int texUnit) const;
            void BindGLuintTextureToShader(const GLuint& t, const std::string& uniform, int texUnit) const;
            void BindMesh(const OGLMesh& m);
            void BindOBJMesh(const OGLOBJMesh& m);
            void DrawBoundMesh(uint32_t subLayer = 0, uint32_t numInstances = 1);
            void DrawBoundOBJMesh(uint32_t subLayer = 0, uint32_t numInstances = 1);
            void DrawSubMesh(int i, uint32_t numInstances = 1);

#ifdef _WIN32
            void InitWithWin32(Window& w);
            void DestroyWithWin32();
            HDC		deviceContext;		//...Device context?
            HGLRC	renderContext;		//Permanent Rendering Context		
#endif

            const OGLMesh* boundMesh;
            const OGLOBJMesh* objBoundMesh;
            const OGLShader* activeShader;
        private:
            bool initState;
        };
    }
}