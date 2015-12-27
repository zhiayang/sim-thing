// Rendering.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <imgui.h>

#include "imguiwrapper.h"
#include "imgui_impl_sdl.h"

namespace IG
{
	void BeginFrame(SDL::Renderer* renderer)
	{
		ImGui_ImplSdl_NewFrame(renderer->window->sdlWin);
	}

	void EndFrame(SDL::Renderer* renderer)
	{
		// call back to opengl
		glViewport(0, 0, (int) ImGui::GetIO().DisplaySize.x, (int) ImGui::GetIO().DisplaySize.y);
		glClearColor(renderer->clearColour.fr, renderer->clearColour.fg, renderer->clearColour.fb, renderer->clearColour.fa);
		glClear(GL_COLOR_BUFFER_BIT);

		renderer->RenderAll();

		ImGui::Render();

		SDL_GL_SwapWindow(renderer->window->sdlWin);
	}
}
