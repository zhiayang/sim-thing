// main.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"

#include "sdlwrapper.h"
#include "imguiwrapper.h"

#include "config.h"

#include <deque>

// static const double fixedDeltaTimeNs	= 20.0 * 1000.0 * 1000.0;
static const double targetFramerate		= 60.0;
static const double targetFrameTimeNs	= S_TO_NS(1.0) / targetFramerate;


int main(int argc, char** argv)
{
	// Setup SDL
	SDL::Initialise();

	auto r = IG::Initialise(1024, 640, Util::Colour(114, 144, 154));
	SDL_GLContext glcontext = r.first;
	SDL::Renderer* renderer = r.second;

	ImGuiIO& io = ImGui::GetIO();

	ImFont* menlo = Util::Font::get("monaco", 16);


	double frameTime = 16.6667;
	std::deque<double> prevFps(32);


	// Main loop
	bool done = false;
	while(!done)
	{
		SDL_Event event;
		{
			auto t = IG::ProcessEvents();

			event = t.first;
			done = t.second;
		}


		IG::BeginFrame(renderer);
		ImGui::PushFont(menlo);

		// draw fps
		double frameBegin = Util::Time::ns();
		{
			if(Config::getShowFps())
			{
				// frames per second is (1sec to ns) / 'frametime' (in ns)
				double fps = S_TO_NS(1.0) / frameTime;

				// smooth fps
				#if 1
				{
					prevFps.push_back(fps);

					if(prevFps.size() > 32)
						prevFps.erase(prevFps.begin());

					double totalfps = 0;
					for(auto f : prevFps)
						totalfps += f;

					totalfps /= prevFps.size();
					fps = totalfps;
				}
				#endif

				ImGui::BeginMainMenuBar();
				ImGui::Text("FPS: %.2lf", fps);
				ImGui::EndMainMenuBar();
			}
		}







		ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 300));
		ImGui::Begin("Terminal", 0, { 500, 600 });
		{
			static char text[1024 * 16] =
				"/*\n"
				" The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
				" the hexadecimal encoding of one offending instruction,\n"
				" more formally, the invalid operand with locked CMPXCHG8B\n"
				" instruction bug, is a design flaw in the majority of\n"
				" Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
				" processors (all in the P5 microarchitecture).\n"
				"*/\n\n"
				"label:\n"
				"\tlock cmpxchg8b eax\n";

			ImGui::InputTextMultiline("##source", text, sizeof(text), ImVec2(-1, -1), ImGuiInputTextFlags_AllowTabInput);
		}
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		ImGui::ShowStyleEditor();




		// // 1. Show a simple window
		// // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		// {
		// 	static float f = 0.0f;
		// 	ImGui::Text("Hello, world!");
		// 	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		// 	ImGui::ColorEdit3("clear color", (float*) &clear_color);
		// 	if(ImGui::Button("Test Window")) show_test_window ^= 1;
		// 	if(ImGui::Button("Another Window")) show_another_window ^= 1;
		// 	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		// }

		// // 2. Show another simple window, this time using an explicit Begin/End pair
		// if(show_another_window)
		// {
		// 	ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
		// 	ImGui::Begin("Another Window", &show_another_window);
		// 	ImGui::Text("Hello");
		// 	ImGui::End();
		// }

		// // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		// if(show_test_window)
		// {
		// 	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		// 	ImGui::ShowTestWindow(&show_test_window);
		// }


		ImGui::PopFont();

		IG::EndFrame(renderer);




		// more fps computation
		{
			double end = Util::Time::ns();
			frameTime = end - frameBegin;

			// don't kill the CPU
			{
				double toWait = targetFrameTimeNs - frameTime;
				if(toWait >= 0)
				{
					usleep(NS_TO_US(toWait));
				}
				else
				{
					// todo: we missed our framerate.
				}
			}
		}


	}

	// Cleanup
	ImGui_ImplSdl_Shutdown();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(renderer->window->sdlWin);
	SDL_Quit();

	return 0;
}
