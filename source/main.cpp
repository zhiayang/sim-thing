// main.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <glbinding/gl/gl.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "graphicswrapper.h"

#include "model.h"

#include "config.h"
#include "tinyformat.h"

#include "sotv/sotv.h"
#include "sotv/gui.h"

#include <unistd.h>
#include <array>

static const double timeSpeedupFactor	= 20.0;
static const double fixedDeltaTimeNs	= 1.0 * 1000.0 * 1000.0;
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

	static size_t prevIndex = 0;
	static std::array<double, 50> prevFps;

	double renderDelta = 0;

	// draw fps
	{
		if(Config::getShowFps())
		{
			// frames per second is (1sec to ns) / 'frametime' (in ns)
			currentFps = S_TO_NS(1.0) / frameTime;

			// smooth fps
			#if 1
			{
				prevFps[prevIndex++ % 50] = currentFps;
				currentFps = std::accumulate(prevFps.begin(), prevFps.end(), 0) / prevFps.size();
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
	gameState->playerStation = Sotv::Station::makeDefaultSpaceStation("");


	double accumulator = 0.0;
	double frameTime = S_TO_NS(0.01667);
	double currentFps = 0.0;

	double prevTimestamp = Util::Time::ns();
	double renderDelta = 0;


	Input::addHandler(&gameState->inputState, Input::Keys::Space, 0, [](Input::State* s, Input::Keys k) -> bool {

		LOG("Life Support: %s", gameState->playerStation->lifeSupportSystem->toggle() ? "on" : "off");
		return true;

	}, Input::HandlerKind::PressDown);


	Rx::Model* model = Rx::loadModelFromAsset(AssetLoader::Load("models/test/test.obj"));



	double vertices[] = {	0.0, -0.525731, 0.850651,
						0.850651, 0.0, 0.525731,
						0.850651, 0.0, -0.525731,
						-0.850651, 0.0, -0.525731,
						-0.850651, 0.0, 0.525731,
						-0.525731, 0.850651, 0.0,
						0.525731, 0.850651, 0.0,
						0.525731, -0.850651, 0.0,
						-0.525731, -0.850651, 0.0,
						0.0, -0.525731, -0.850651,
						0.0, 0.525731, -0.850651,
						0.0, 0.525731, 0.850651
					};

	auto colours = new double[48];
	for(int i = 0; i < 48; i += 4)
	{
		colours[i + 0] = Util::Random::get(0.3, 0.9);
		colours[i + 1] = Util::Random::get(0.3, 0.9);
		colours[i + 2] = Util::Random::get(0.3, 0.9);
		colours[i + 3] = 1.0;
	}

	double indices[] = {
							1,  2,  6,
							1,  7,  2,
							3,  4,  5,
							4,  3,  8,
							6,  5,  11,
							5,  6,  10,
							9,  10, 2,
							10, 9,  3,
							7,  8,  9,
							8,  7,  0,
							11, 0,  1,
							0,  11, 4,
							6,  2,  10,
							1,  6,  11,
							3,  5,  10,
							5,  4,  11,
							2,  7,  9,
							7,  1,  0,
							3,  9,  8,
							4,  8,  0
						};


	double normals[] = {
						0.000000, -0.417775, 0.675974,
						0.675973, 0.000000, 0.417775,
						0.675973, -0.000000, -0.417775,
						-0.675973, 0.000000, -0.417775,
						-0.675973, -0.000000, 0.417775,
						-0.417775, 0.675974, 0.000000,
						0.417775, 0.675973, -0.000000,
						0.417775, -0.675974, 0.000000,
						-0.417775, -0.675974, 0.000000,
						0.000000, -0.417775, -0.675973,
						0.000000, 0.417775, -0.675974,
						0.000000, 0.417775, 0.675973
					};


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
				// we tell them that 50 ms has passed, when in actual fact only 1 ms has passed.
				// we still do the same number of updates per second.
				Sotv::Update(*gameState, fixedDeltaTimeNs * timeSpeedupFactor);
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
		}

		Sotv::Render(*gameState, renderDelta, renderer);
		if((true))
		{
			std::string fpsstr = tfm::format("%.2f fps", currentFps);

			renderer->RenderString(fpsstr, primaryFont, 12.0, Math::Vector2(5, 5));

			auto psys = gameState->playerStation->powerSystem;
			auto lss = gameState->playerStation->lifeSupportSystem;

			double stor = Units::convertJoulesToWattHours(psys->getTotalStorageInJoules());
			double cap = Units::convertJoulesToWattHours(psys->getTotalCapacityInJoules());
			double prod = psys->getTotalProductionInWatts();

			double percentage = (stor / cap) * 100.0;

			// divide stor and cap by system voltage to get a number in amp-hours.
			// we divide by 3600 to convert from amp-seconds to amp-hours

			auto str = tfm::format("%s / %s (%.1f%%)  |  +%s / -%s", Units::formatWithUnits(stor, 2, "Wh"),
				Units::formatWithUnits(cap, 2, "Wh"), percentage,
				Units::formatWithUnits(prod, 2, "W"), Units::formatWithUnits(psys->getTotalConsumptionInWatts(), 2, "W"));

			renderer->RenderStringRightAligned(str, primaryFont, 14, Math::Vector2(5, 5));


			size_t ofs = 5;

			#if 1
			for(auto batt : psys->storage)
			{
				double cur = Units::convertJoulesToWattHours(batt->getEnergyInJoules());
				double cap = Units::convertJoulesToWattHours(batt->getCapacityInJoules());

				auto str = tfm::format("%s / %s (%.1f%%)", Units::formatWithUnits(cur, 2, "Wh"),
					Units::formatWithUnits(cap, 2, "Wh"), 100.0 * ((double) cur / (double) cap));

				renderer->RenderStringRightAligned(str, primaryFont, 14, Math::Vector2(5, ofs += 15));
			}
			#endif

			str = tfm::format("%s / %s", Units::formatWithUnits(lss->getAtmospherePressure(), 2, "Pa"),
				Units::formatWithUnits(Units::convertKelvinToCelsius(lss->getAtmosphereTemperature()), 1, "°C"));

			renderer->RenderStringRightAligned(str, primaryFont, 14, Math::Vector2(5, ofs += 15));
		}



		{
			// gl::glVertexPointer(3, gl::GL_DOUBLE, 0, vertices);
			// gl::glColorPointer(4, gl::GL_DOUBLE, 0, colours);
			// gl::glNormalPointer(gl::GL_DOUBLE, 0, normals);

			// gl::glDrawElements(gl::GL_TRIANGLES, 60, gl::GL_UNSIGNED_BYTE, (void*) indices);
			// gl::glLoadIdentity();
		}




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

				if(toWait >= 500)
				{
					struct timespec ts;
					ts.tv_nsec = toWait;

					nanosleep(&ts, 0);
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
