// main.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "graphicswrapper.h"

#include "config.h"
#include "tinyformat.h"

#include "sotv/sotv.h"
#include "sotv/gui.h"

#include <unistd.h>
#include <deque>

static const double fixedDeltaTimeNs	= 10.0 * 1000.0 * 1000.0;
static const double targetFramerate		= 60.0;
static const double targetFrameTimeNs	= S_TO_NS(1.0) / targetFramerate;

static Sotv::GameState* gameState = 0;

namespace Rx
{
	std::pair<SDL_Event, bool> ProcessEvents()
	{
		bool done = false;

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
				Input::HandleInput(&gameState->inputState, &event);

			ImGui_ImplSdl_ProcessEvent(&event);

			if(event.type == SDL_QUIT)
				done = true;
		}

		return { event, done };
	}
}



static std::pair<double, double> determineCurrentFPS(double previous, double frameBegin, double frameTime)
{
	double currentFps = previous;
	static std::deque<double> prevFps;

	double renderDelta = 0;

	// draw fps
	{
		if(Config::getShowFps())
		{
			// frames per second is (1sec to ns) / 'frametime' (in ns)
			currentFps = S_TO_NS(1.0) / frameTime;

			// smooth fps
			#if 0
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

		renderDelta = frameBegin - previous;
	}

	return { currentFps, renderDelta };
}






int main(int argc, char** argv)
{
	// Setup SDL
	auto r = Rx::Initialise(Config::getResX(), Config::getResY(), Util::Colour(0xC, 0x12, 0x18));
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
		io.FontGlobalScale = 0.25;

		// init.cpp
		Rx::SetupDefaultStyle();
	}


	Rx::Font primaryFont = Rx::getFont("menlo", 32);
	ImFont* menlo = primaryFont.imgui;


	// initialise some things
	// todo: this should go into some savefile-parsing system that does the necessary stuff
	// also should probably be more automatic than this
	gameState = new Sotv::GameState();
	gameState->playerStation = Sotv::Station::makeDefaultSpaceStation("Pioneer XV");


	double accumulator = 0.0;
	double frameTime = S_TO_NS(0.01667);
	double currentFps = 0.0;

	std::deque<double> prevFps;

	double prevTimestamp = Util::Time::ns();
	double renderDelta = 0;

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

		double frameBegin = Util::Time::ns();
		std::tie(currentFps, renderDelta) = determineCurrentFPS(prevTimestamp, frameBegin, frameTime);


		// do updates
		{
			accumulator += frameTime;

			while(accumulator >= fixedDeltaTimeNs)
			{
				Sotv::Update(*gameState, fixedDeltaTimeNs);
				accumulator -= fixedDeltaTimeNs;
			}
		}







		Rx::PreFrame(renderer);
		Rx::BeginFrame(renderer);

		ImGui::PushFont(menlo);
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);


		if(Config::getShowFps())
		{
			renderer->SetColour(Util::Colour::white());

			// static Rx::Texture* pic = new Rx::Texture("test.png", renderer);
			// renderer->RenderTex(pic, { 0, 0 });


			// draw some stats
			{
				std::string fpsstr = tfm::format("%.2f fps", currentFps);

				renderer->RenderString(fpsstr, primaryFont, 12.0, Math::Vector2(5, 5));

				size_t stor = gameState->playerStation->powerSystem->getTotalStorageInJoules();
				size_t cap = gameState->playerStation->powerSystem->getTotalCapacityInJoules();
				size_t prod = gameState->playerStation->powerSystem->getTotalProductionInWatts();
				double percentage = ((double) stor / (double) cap) * 100.0;


				auto str = tfm::format("%s / %s at %s (%.1f%%)", Util::formatWithUnits(stor, 2, "J"),
					Util::formatWithUnits(cap, 2, "J"), Util::formatWithUnits(prod, 2, "W"), percentage);

				// size_t strwidth = renderer->getStringWidthInPixels(str, primaryFont, 24);
				// renderer->RenderString(str, primaryFont, 24, Math::Vector2(Config::getResX() - strwidth - 5, 5));

				renderer->RenderStringRightAligned(str, primaryFont, 16, Math::Vector2(5, 5));

				size_t ofs = 5;
				for(auto batt : gameState->playerStation->powerSystem->storage)
				{
					auto str = tfm::format("%s / %s (%.1f%%)", Util::formatWithUnits(batt->current, 2, "J"),
						Util::formatWithUnits(batt->capacity, 2, "J"), 100.0 * ((double) batt->current / (double) batt->capacity));

					renderer->RenderStringRightAligned(str, primaryFont, 16, Math::Vector2(5, ofs += 15));
				}
			}
		}

		Sotv::Render(*gameState, renderDelta, renderer);









		// ImGui::ShowStyleEditor();
		// ImGui::ShowTestWindow();








































		ImGui::PopFont();
		Rx::EndFrame(renderer);

		// more fps computation
		{
			double end = Util::Time::ns();
			frameTime = end - frameBegin;

			// don't kill the CPU
			{
				double toWait = targetFrameTimeNs - frameTime;

				if(toWait > 1000 * 1000)
				{
					// usleep(NS_TO_US(toWait));
				}
				else
				{
					// todo: we missed our framerate.
				}
			}

			prevTimestamp = frameBegin;
		}
	}

	// Cleanup
	ImGui_ImplSdl_Shutdown();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(renderer->window->sdlWin);
	SDL_Quit();

	return 0;
}
