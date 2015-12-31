// main.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "graphicswrapper.h"

#include "config.h"
#include "format.h"

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

	ImGuiIO& io = ImGui::GetIO();
	{
		// setup defaults
		io.FontGlobalScale = 0.50;
		// io.IniFilename = nullptr;

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

		style.Colors[ImGuiCol_MinButton]			= ImVec4(1.00f, 0.77f, 0.19f, 1.00f);
		style.Colors[ImGuiCol_MinButtonHovered]		= ImVec4(1.00f, 0.77f, 0.19f, 1.00f);
		style.Colors[ImGuiCol_MinButtonActive]		= ImVec4(0.75f, 0.56f, 0.14f, 1.00f);

		style.Colors[ImGuiCol_MaxButton]			= ImVec4(0.16f, 0.80f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_MaxButtonHovered]		= ImVec4(0.16f, 0.80f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_MaxButtonActive]		= ImVec4(0.12f, 0.61f, 0.19f, 1.00f);
	}



	Rx::Font primaryFont = Rx::getFont("menlo", 14);
	ImFont* menlo = primaryFont.imgui;


	double frameTime = S_TO_NS(0.01667);
	double currentFps = 0.0;
	std::deque<double> prevFps;


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


		// draw fps
		double frameBegin = Util::Time::ns();
		{
			if(Config::getShowFps())
			{
				// frames per second is (1sec to ns) / 'frametime' (in ns)
				currentFps = S_TO_NS(1.0) / frameTime;

				// smooth fps
				#if 1
				{
					prevFps.push_back(currentFps);

					if(prevFps.size() > 50)
						prevFps.erase(prevFps.begin());

					double totalfps = 0;
					for(auto f : prevFps)
						totalfps += f;

					totalfps /= prevFps.size();
					currentFps = totalfps;
				}
				#endif
			}
		}


		Rx::PreFrame(renderer);
		Rx::getFont("menlo", 32);
		Rx::BeginFrame(renderer);

		ImGui::PushFont(menlo);
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);


		if(Config::getShowFps())
		{
			renderer->SetColour(Util::Colour::white());

			static Rx::Texture* pic = new Rx::Texture("test.png", renderer);
			renderer->RenderTex(pic, { 0, 0 });

			{
				std::string str;
				str = fmt::sprintf("%.2f fps // %d windows // %d verts // %d allocations", currentFps,
					io.MetricsActiveWindows, io.MetricsRenderVertices, io.MetricsAllocs);

				ImGui::BeginMainMenuBar();
				ImGui::Text("%s", str.c_str());
				ImGui::EndMainMenuBar();
			}
		}


























		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 300));

		bool open;
		ImGui::Begin("Terminal", &open, { 500, 600 });
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

		// ImGui::ShowStyleEditor();
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
