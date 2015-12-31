// main.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"

#include "graphicswrapper.h"

#include "config.h"

#include <deque>

// static const double fixedDeltaTimeNs	= 20.0 * 1000.0 * 1000.0;
static const double targetFramerate		= 60.0;
static const double targetFrameTimeNs	= S_TO_NS(1.0) / targetFramerate;


int main(int argc, char** argv)
{
	// Setup SDL
	auto r = Rx::Initialise(1024, 640, Util::Colour(114, 144, 154));
	SDL_GLContext glcontext = r.first;
	Rx::Renderer* renderer = r.second;

	// any lower, and it gets sketchy.
	// for some reason, increasing oversampling doesn't help beyond a certain point.
	// in fact, going beyond 8 for some reason wrecks the font totally.

	// hence, this hack: we pass getFont() the *actual* size we want the font to be, in pixels.
	// however, it multiplies that by 2 internally, so if we say we want font-size 16, it actually
	// loads font-size 32.
	// since our global scale is 0.50, this effectively gives us another *level* of oversampling.
	// it doesn't seem to affect framerate too much, and produces **MUCH** crisper text.

	ImGui::GetIO().FontGlobalScale = 0.50;


	Rx::Font primaryFont = Rx::getFont("menlo", 14);
	ImFont* menlo = primaryFont.imgui;


	double frameTime = S_TO_NS(0.01667);
	std::deque<double> prevFps;

	// Rx::Texture* pic = new Rx::Texture("test.png", renderer);

	// Main loop
	bool done = false;
	while(!done)
	{
		SDL_Event event;
		{
			auto t = Rx::ProcessEvents();

			event = t.first;
			done = t.second;
		}

		Rx::PreFrame(renderer);
		Rx::getFont("menlo", 32);
		Rx::BeginFrame(renderer);

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

					if(prevFps.size() > 50)
						prevFps.erase(prevFps.begin());

					double totalfps = 0;
					for(auto f : prevFps)
						totalfps += f;

					totalfps /= prevFps.size();
					fps = totalfps;
				}
				#endif

				renderer->SetColour(Util::Colour::white());
				// renderer->RenderTex(pic, { 0, 0 });

				// auto c = renderer->GetColour();
				renderer->RenderString(std::string("FPS: ") + std::to_string((int) fps), primaryFont, 14, Math::Vector2(50, 50));
				renderer->RenderRect(Math::Rectangle(100, 100, 400, 400));



				// renderer->SetColour(c);

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


		ImGui::ShowTestWindow();


		ImGui::PopFont();

		Rx::EndFrame(renderer);

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
