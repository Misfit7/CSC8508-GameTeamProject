#pragma once

#include <string.h>
#include "Window.h"
#include "Win32Window.h"
#include "../Plugins/imGui/imgui.h"
#include "../Plugins/imGui/imgui_impl_win32.h"
#include "../Plugins/imGui/imgui_impl_opengl3.h"
#include "../Common/OpenGLRendering/SOIL/stb_image_aug.h"

#include "Assets.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {

		enum UItype
		{

		};

		struct UI_Image
		{
			int img_width;
			int img_height;
			unsigned int img_texture;
		};

		class UI
		{
		public:
			UI(GameWorld* world);
			~UI();
			void Update(float dt);
			void DrawUI();
			void DrawLoading(float dt);
			void DrawMenu(float dt);
			void DrawPlayingUI(float dt);
			void DrawServerPlayingUI(float dt);
			void DrawClientPlayingUI(float dt);
			void DrawChooseServer(float dt);
			void DrawPausedMenu(float dt);
			void DrawFailureMenu(float dt);
			void DrawFinishMenu(float dt);

			void SetLoadingStep(int step) { loadingstep = step; };
			void SetSuccess(bool s) { success = s; }

			float GetPlayTime() { return playtime; };
		protected:
			GameWorld* world;

			ImFont* titlefont;
			ImFont* menufont;
			ImFont* infofont;
			ImFont* normalfont;

			UI_Image pickaxe;
			UI_Image axe;
			UI_Image bucket_empty;
			UI_Image bucket_fill;
			UI_Image plank;
			UI_Image stone;
			UI_Image rail;
			UI_Image assassin;
			UI_Image girl;
			UI_Image evil_genius;
			UI_Image loading;
			UI_Image menu;
			UI_Image fire;

			int loadingstep;
			bool success = false;

			float playtime;
			int fflag = 1;
			float firealpha = 0.0f;
			float counter = 3.0f;

			std::string dist;
			std::string time;
		};
	}
}