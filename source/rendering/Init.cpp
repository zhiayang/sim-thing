// Init.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <tuple>

#include "imgui.h"

#include "graphicswrapper.h"
#include "imgui_impl_sdl.h"

#include <SDL2/SDL_ttf.h>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

namespace Rx
{
	std::pair<SDL_GLContext, Rx::Renderer*> Initialise(int width, int height, Util::Colour clear)
	{
		// initialise SDL
		{
			auto err = SDL_Init(SDL_INIT_EVERYTHING);
			if(err)	ERROR("SDL failed to initialise, subsystem flags: %d", SDL_INIT_EVERYTHING);

			// init SDL_Image
			if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
				ERROR("Failed to initialise SDL2_image library");
			TTF_Init();
		}





		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = { (float) width, (float) height };

		// Setup window
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);

		Rx::Window* window = new Rx::Window("connect", io.DisplaySize.x, io.DisplaySize.y, true);
		SDL_GLContext glcontext = SDL_GL_CreateContext(window->sdlWin);
		if(glcontext == 0)
			ERROR("Failed to create OpenGL context. SDL Error: '%s'", SDL_GetError());

		glbinding::Binding::initialize();

		// Setup ImGui binding
		ImGui_ImplSdl_Init(window->sdlWin);

		Rx::Renderer* renderer = new Rx::Renderer(window, glcontext, clear);

		return { glcontext, renderer };
	}



	void SetupDefaultStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding		= 4.0f;
		style.ScrollbarRounding		= 4.0f;
		style.ScrollbarSize			= 12.0f;
		style.WindowTitleAlign		= ImGuiAlign_Center;

		style.Colors[ImGuiCol_FrameBgHovered]		= ImVec4(0.25f, 0.75f, 0.50f, 0.50f);
		style.Colors[ImGuiCol_FrameBgActive]		= ImVec4(0.90f, 0.65f, 0.31f, 0.75f);
		style.Colors[ImGuiCol_TitleBg]				= ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.37f, 0.37f, 0.37f, 0.78f);
		style.Colors[ImGuiCol_TitleBgActive]		= ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg]			= ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
		style.Colors[ImGuiCol_ScrollbarGrab]		= ImVec4(0.50f, 0.50f, 0.50f, 0.30f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.59f, 0.59f, 0.59f, 0.40f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.25f, 0.50f, 0.98f, 0.78f);
		style.Colors[ImGuiCol_CheckMark]			= ImVec4(0.50f, 0.86f, 0.13f, 0.75f);
		style.Colors[ImGuiCol_Button]				= ImVec4(0.25f, 0.50f, 0.98f, 0.86f);
		style.Colors[ImGuiCol_ButtonHovered]		= ImVec4(0.31f, 0.55f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive]			= ImVec4(0.13f, 0.25f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_Header]				= ImVec4(0.25f, 0.50f, 0.98f, 0.45f);
		style.Colors[ImGuiCol_HeaderHovered]		= ImVec4(0.25f, 0.50f, 0.98f, 0.80f);
		style.Colors[ImGuiCol_HeaderActive]			= ImVec4(0.13f, 0.25f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg]		= ImVec4(0.25f, 0.50f, 0.98f, 0.75f);

		style.Colors[ImGuiCol_CloseButton]			= ImVec4(0.98f, 0.24f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonHovered]	= ImVec4(0.98f, 0.24f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonActive]	= ImVec4(0.71f, 0.10f, 0.10f, 1.00f);

		// style.Colors[ImGuiCol_MinButton]			= ImVec4(1.00f, 0.77f, 0.19f, 1.00f);
		// style.Colors[ImGuiCol_MinButtonHovered]		= ImVec4(1.00f, 0.77f, 0.19f, 1.00f);
		// style.Colors[ImGuiCol_MinButtonActive]		= ImVec4(0.75f, 0.56f, 0.14f, 1.00f);

		// style.Colors[ImGuiCol_MaxButton]			= ImVec4(0.16f, 0.80f, 0.26f, 1.00f);
		// style.Colors[ImGuiCol_MaxButtonHovered]		= ImVec4(0.16f, 0.80f, 0.26f, 1.00f);
		// style.Colors[ImGuiCol_MaxButtonActive]		= ImVec4(0.12f, 0.61f, 0.19f, 1.00f);
	}
}






























