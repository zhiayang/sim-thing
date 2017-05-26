// main.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <glbinding/gl/gl.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/rx.h"

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
static Rx::Renderer* theRenderer = 0;

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

			// ImGui_ImplSdl_ProcessEvent(&event);

			if(event.type == SDL_QUIT)
			{
				done = true;
			}
			else if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				assert(theRenderer);
				// auto sdlw = theRenderer->window->sdlWin;

				// int w = 0;
				// int h = 0;
				// SDL_GetWindowSize(sdlw, &w, &h);

				LOG("resize");
				theRenderer->updateWindowSize(event.window.data1, event.window.data2);
			}
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
	Config::setResX(1024);
	Config::setResY(640);







	// Setup SDL
	auto r = Rx::Initialise(Config::getResX(), Config::getResY());
	SDL_GLContext glcontext = r.first;
	Rx::Window* window = r.second;


	auto primaryFont = Rx::getFont("menlo", 64, ' ', 0xFF - ' ', 2, 2);


	// initialise some things
	// todo: this should go into some savefile-parsing system that does the necessary stuff
	// also should probably be more automatic than this
	gameState = new Sotv::GameState();
	gameState->playerStation = Sotv::Station::makeDefaultSpaceStation("");


	double accumulator = 0.0;
	double frameTime = S_TO_NS(0.01667);
	double currentFps = 0.0;

	double prevTimestamp = util::Time::ns();
	double renderDelta = 0;


	Input::addHandler(&gameState->inputState, Input::Keys::Space, 0, [](Input::State* s, Input::Keys k) -> bool {

		LOG("Life Support: %s", gameState->playerStation->lifeSupportSystem->toggle() ? "on" : "off");
		return true;

	}, Input::HandlerKind::PressDown);


	// Rx::Model* model = Rx::loadModelFromAsset(AssetLoader::Load("models/test/test.obj"));


	auto progId = AssetLoader::compileAndLinkGLShaderProgram("shaders/simplevert.vert", "shaders/simplefrag.frag");
	auto textProgId = AssetLoader::compileAndLinkGLShaderProgram("shaders/textShader.vert", "shaders/textShader.frag");

	assert(progId >= 0);
	assert(textProgId >= 0);

	// camera matrix: camera at [ 0, 3, 7 ], looking at [ 0, 0, 0 ], rotated right-side up
	{
		int rx = 0; int ry = 0;
		SDL_GetWindowSize(window->sdlWin, &rx, &ry);
		LOG("window is %d x %d", rx, ry);

		int dx = 0; int dy = 0;
		SDL_GL_GetDrawableSize(window->sdlWin, &dx, &dy);

		int sx = dx / rx;
		int sy = dy / ry;

		assert(sx == sy);

		theRenderer = new Rx::Renderer(window, glcontext, util::colour(25, 25, 25, 255),
			glm::lookAt(glm::vec3(0, 3, 7), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)), progId, textProgId, glm::radians(70.0f),
			rx, ry, sx, 0.001, 1000);
	}


	std::vector<glm::vec3> vertices = {
		{ -1.0f,-1.0f,-1.0f, },
		{ -1.0f,-1.0f, 1.0f, },
		{ -1.0f, 1.0f, 1.0f, },
		{  1.0f, 1.0f,-1.0f, },
		{ -1.0f,-1.0f,-1.0f, },
		{ -1.0f, 1.0f,-1.0f, },
		{  1.0f,-1.0f, 1.0f, },
		{ -1.0f,-1.0f,-1.0f, },
		{  1.0f,-1.0f,-1.0f, },
		{  1.0f, 1.0f,-1.0f, },
		{  1.0f,-1.0f,-1.0f, },
		{ -1.0f,-1.0f,-1.0f, },
		{ -1.0f,-1.0f,-1.0f, },
		{ -1.0f, 1.0f, 1.0f, },
		{ -1.0f, 1.0f,-1.0f, },
		{  1.0f,-1.0f, 1.0f, },
		{ -1.0f,-1.0f, 1.0f, },
		{ -1.0f,-1.0f,-1.0f, },
		{ -1.0f, 1.0f, 1.0f, },
		{ -1.0f,-1.0f, 1.0f, },
		{  1.0f,-1.0f, 1.0f, },
		{  1.0f, 1.0f, 1.0f, },
		{  1.0f,-1.0f,-1.0f, },
		{  1.0f, 1.0f,-1.0f, },
		{  1.0f,-1.0f,-1.0f, },
		{  1.0f, 1.0f, 1.0f, },
		{  1.0f,-1.0f, 1.0f, },
		{  1.0f, 1.0f, 1.0f, },
		{  1.0f, 1.0f,-1.0f, },
		{ -1.0f, 1.0f,-1.0f, },
		{  1.0f, 1.0f, 1.0f, },
		{ -1.0f, 1.0f,-1.0f, },
		{ -1.0f, 1.0f, 1.0f, },
		{  1.0f, 1.0f, 1.0f, },
		{ -1.0f, 1.0f, 1.0f, },
		{  1.0f,-1.0f, 1.0f },
	};

	// One color for each vertex. They were generated randomly.
	std::vector<glm::vec4> colours = {
		{ 0.583f, 0.771f, 0.014f, 1.0f },
		{ 0.609f, 0.115f, 0.436f, 1.0f },
		{ 0.327f, 0.483f, 0.844f, 1.0f },
		{ 0.822f, 0.569f, 0.201f, 1.0f },
		{ 0.435f, 0.602f, 0.223f, 1.0f },
		{ 0.310f, 0.747f, 0.185f, 1.0f },
		{ 0.597f, 0.770f, 0.761f, 1.0f },
		{ 0.559f, 0.436f, 0.730f, 1.0f },
		{ 0.359f, 0.583f, 0.152f, 1.0f },
		{ 0.483f, 0.596f, 0.789f, 1.0f },
		{ 0.559f, 0.861f, 0.639f, 1.0f },
		{ 0.195f, 0.548f, 0.859f, 1.0f },
		{ 0.014f, 0.184f, 0.576f, 1.0f },
		{ 0.771f, 0.328f, 0.970f, 1.0f },
		{ 0.406f, 0.615f, 0.116f, 1.0f },
		{ 0.676f, 0.977f, 0.133f, 1.0f },
		{ 0.971f, 0.572f, 0.833f, 1.0f },
		{ 0.140f, 0.616f, 0.489f, 1.0f },
		{ 0.997f, 0.513f, 0.064f, 1.0f },
		{ 0.945f, 0.719f, 0.592f, 1.0f },
		{ 0.543f, 0.021f, 0.978f, 1.0f },
		{ 0.279f, 0.317f, 0.505f, 1.0f },
		{ 0.167f, 0.620f, 0.077f, 1.0f },
		{ 0.347f, 0.857f, 0.137f, 1.0f },
		{ 0.055f, 0.953f, 0.042f, 1.0f },
		{ 0.714f, 0.505f, 0.345f, 1.0f },
		{ 0.783f, 0.290f, 0.734f, 1.0f },
		{ 0.722f, 0.645f, 0.174f, 1.0f },
		{ 0.302f, 0.455f, 0.848f, 1.0f },
		{ 0.225f, 0.587f, 0.040f, 1.0f },
		{ 0.517f, 0.713f, 0.338f, 1.0f },
		{ 0.053f, 0.959f, 0.120f, 1.0f },
		{ 0.393f, 0.621f, 0.362f, 1.0f },
		{ 0.673f, 0.211f, 0.457f, 1.0f },
		{ 0.820f, 0.883f, 0.371f, 1.0f },
		{ 0.982f, 0.099f, 0.879f, 1.0f }
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

		double frameBegin = util::Time::ns();
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



		Rx::PreFrame(theRenderer);
		Rx::BeginFrame(theRenderer);

		// theRenderer->renderVertices(vertices, colours, { }, { });
		theRenderer->renderStringInScreenSpace("3,.94 pqkWh / 43.50 kWh (9.1%)  |  +47.33 kW / -0.00 W", primaryFont, 14, glm::vec2(0, 10));

		// theRenderer->renderStringInScreenSpace("A    B C", primaryFont, 16, glm::vec2(200, 50));

		Rx::EndFrame(theRenderer);


























		#if 0



		Rx::PreFrame(renderer);
		Rx::BeginFrame(renderer);

		ImGui::PushFont(menlo);
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);





		if(Config::getShowFps())
		{
			renderer->SetColour(util::colour::white());
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



		#endif


		// more fps computation
		{
			double end = util::Time::ns();
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
	// ImGui_ImplSdl_Shutdown();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(theRenderer->window->sdlWin);
	SDL_Quit();

	return 0;
}
