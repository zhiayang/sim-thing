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
#include "renderer/model.h"

#include "config.h"
#include "tinyformat.h"

#include "sotv/sotv.h"
#include "sotv/gui.h"

#include <unistd.h>
#include <array>

static const double timeSpeedupFactor	= 20.0;
static const double fixedDeltaTimeNs	= 1.0 * 1000.0 * 1000.0;

// static const double targetFramerate		= 120.0;
// static const double targetFrameTimeNs	= S_TO_NS(1.0) / targetFramerate;

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
				Input::handleKeyInput(&gameState->inputState, &event);

			else if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN)
				Input::handleMouseInput(&gameState->inputState, &event);

			if(event.type == SDL_QUIT)
			{
				done = true;
			}
			else if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				assert(theRenderer);
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

	prof::enable();







	// Setup SDL
	auto r = Rx::Initialise(Config::getResX(), Config::getResY());
	SDL_GLContext glcontext = r.first;
	Rx::Window* window = r.second;


	auto primaryFont = Rx::getFont("menlo", 64, ' ', 0xFF - ' ', 2, 2);

	double accumulator = 0.0;
	double frameTime = S_TO_NS(0.01667);
	double currentFps = 0.0;

	double prevTimestamp = util::Time::ns();
	double renderDelta = 0;

	// setup the shader... this is more involved than i'd like, but it's more flexible.
	// sidenote: designated initialisers are amazing, fuck c++ for not having it.
	// constructors do not replace this, especially since it doesn't have named arguments
	// .stupid.

	// 1. simple texture shader
	auto textureProg = Rx::ShaderProgram("simpleTexture", Rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/simpleTexture.vs",
		.fragmentShaderPath = "shaders/simpleTexture.fs",
	});

	// 2. simple colour shader
	auto colourProg = Rx::ShaderProgram("simpleColour", Rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/simpleColour.vs",
		.fragmentShaderPath = "shaders/simpleColour.fs",
	});

	// 3. text shader
	auto textProg = Rx::ShaderProgram("textShader", Rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/textShader.vs",
		.fragmentShaderPath = "shaders/textShader.fs",
	});



	// camera matrix: camera at [ 70, 30, 70 ], looking at [ 0, 0, 0 ], rotated right-side up
	{
		int rx = 0; int ry = 0;
		SDL_GetWindowSize(window->sdlWin, &rx, &ry);
		LOG("window is %d x %d", rx, ry);

		int dx = 0; int dy = 0;
		SDL_GL_GetDrawableSize(window->sdlWin, &dx, &dy);

		int sx = dx / rx;
		int sy = dy / ry;

		assert(sx == sy && "vertical and horizontal display scaling values do not match");


		Rx::Camera cam;
		cam.position = glm::vec3(0, 1, 2);
		cam.yaw = -90;
		cam.pitch = -20;

		// setup the renderer. there's many parameters here...
		theRenderer = new Rx::Renderer(window, glcontext,
			util::colour(0.01, 0.01, 0.01),		// clear colour
			cam,								// camera
			textureProg, colourProg, textProg,	// shader programs for textured objects, coloured objects, and text.
			glm::radians(70.0f),				// FOV, 70 degrees
			rx, ry, sx,							// window res (x, y), and scale (2 for retina, 1 for normal)
			0.001, 1000							// near plane, far plane
		);

		// position, colour, intensity
		theRenderer->setAmbientLighting(util::colour::white(), 0.3);
		theRenderer->addPointLight(Rx::PointLight(glm::vec3(0, 2, 8), util::colour::white(), util::colour::white(),
			2.5, 1.0, 0.022, 0.0019));

		theRenderer->addPointLight(Rx::PointLight(glm::vec3(8, 2, 0), util::colour::white(), util::colour::white(),
			3.0, 1.0, 0.022, 0.0019));
	}


	// initialise some things
	// todo: this should go into some savefile-parsing system that does the necessary stuff
	// also should probably be more automatic than this
	gameState = new Sotv::GameState();
	gameState->playerStation = Sotv::Station::makeDefaultSpaceStation("");

	Input::addKeyHandler(&gameState->inputState, Input::Key::Space, 0, [](Input::State* s, Input::Key k, double) -> bool {

		LOG("Life Support: %s", gameState->playerStation->lifeSupportSystem->toggle() ? "on" : "off");
		return true;

	}, Input::HandlerKind::PressDown);

	Input::addKeyHandler(&gameState->inputState, Input::Key::W, 0, [](Input::State* s, Input::Key k, double) -> bool {

		auto cam = theRenderer->getCamera();
		cam.position += cam.front() * 0.005f;
		theRenderer->updateCamera(cam);

		return true;

	}, Input::HandlerKind::WhileDown);

	Input::addKeyHandler(&gameState->inputState, Input::Key::S, 0, [](Input::State* s, Input::Key k, double) -> bool {

		auto cam = theRenderer->getCamera();
		cam.position -= cam.front() * 0.005f;
		theRenderer->updateCamera(cam);

		return true;

	}, Input::HandlerKind::WhileDown);

	Input::addKeyHandler(&gameState->inputState, Input::Key::A, 0, [](Input::State* s, Input::Key k, double) -> bool {

		auto cam = theRenderer->getCamera();
		cam.position -= cam.right() * 0.005f;
		theRenderer->updateCamera(cam);

		return true;

	}, Input::HandlerKind::WhileDown);

	Input::addKeyHandler(&gameState->inputState, Input::Key::D, 0, [](Input::State* s, Input::Key k, double) -> bool {

		auto cam = theRenderer->getCamera();
		cam.position += cam.right() * 0.005f;
		theRenderer->updateCamera(cam);

		return true;

	}, Input::HandlerKind::WhileDown);

	Input::addKeyHandler(&gameState->inputState, Input::Key::ShiftL, 0, [](Input::State* s, Input::Key k, double) -> bool {

		auto cam = theRenderer->getCamera();
		cam.position.y += 0.005;
		theRenderer->updateCamera(cam);

		return true;

	}, Input::HandlerKind::WhileDown);

	Input::addKeyHandler(&gameState->inputState, Input::Key::SuperL, 0, [](Input::State* s, Input::Key k, double) -> bool {

		auto cam = theRenderer->getCamera();
		cam.position.y -= 0.005;
		theRenderer->updateCamera(cam);

		return true;

	}, Input::HandlerKind::WhileDown);



	auto model = Rx::loadModelFromAsset(AssetLoader::Load("models/test/test.obj"), 1.0 / 20000.0);
	// Rx::Model* cube = Rx::Model::getUnitCube();
	// cube = model;

	auto box = new Rx::Texture("textures/box.png", theRenderer);
	auto cubeModel = Rx::Model::fromMesh(Rx::Mesh::getUnitCube(), Rx::Material(util::colour::white(), box, box, 32.0));


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

				// this requires that we can update at arbitrary deltas, but since we're not doing any real physics,
				// i don't foresee this being a problem.

				Sotv::Update(*gameState, fixedDeltaTimeNs * timeSpeedupFactor);
				accumulator -= fixedDeltaTimeNs;


				// update the camera based on the mouse, for now.
				{
					bool invert = true;

					double sensitivity = 0.5;
					auto md = Input::getMouseChange(&gameState->inputState);
					auto cam = theRenderer->getCamera();

					// fprintf(stderr, "delta = (%.0f, %.0f)\n", md.x, md.y);
					cam.pitch = glm::clamp(cam.pitch + md.y * sensitivity * (invert ? -1 : 1), -89.4, +89.4);
					cam.yaw += md.x * sensitivity;

					theRenderer->updateCamera(cam);


					Input::Update(&gameState->inputState, fixedDeltaTimeNs * timeSpeedupFactor);
				}
			}
		}



		Rx::PreFrame(theRenderer);
		Rx::BeginFrame(theRenderer);



		Sotv::Render(*gameState, renderDelta, theRenderer);

		// theRenderer->renderMesh(Rx::Mesh::getUnitCube(), glm::translate(glm::mat4(), glm::vec3(0, -2, 0)), glm::vec4(0.24, 0.59, 0.77, 1.0));

		// theRenderer->renderModel(model, glm::translate(glm::mat4(), glm::vec3(0, 0, 0)), util::colour(0.83, 0.20, 0.22));
		// theRenderer->renderMesh(Rx::Mesh::getUnitCube(), glm::scale(glm::mat4(), glm::vec3(0.5)), glm::vec4(0.24, 0.59, 0.77, 1.0));
		theRenderer->renderModel(cubeModel, glm::scale(glm::mat4(), glm::vec3(0.5)));

		// theRenderer->renderModel(cube, glm::translate(glm::mat4(), glm::vec3(0, 0, 2)), glm::vec4(0.24, 0.59, 0.77, 1.0));
		// theRenderer->renderModel(cube, glm::translate(glm::scale(glm::mat4(), glm::vec3(0.1)), glm::vec3(0, 20, 0)), util::colour::white());




		if((true))
		{
			std::string fpsstr = tfm::format("%.2f fps / [%.1f, %.1f, %.1f] / [%.0f, %.0f] / (y: %.0f, p: %.0f)", currentFps,
				theRenderer->getCamera().position.x, theRenderer->getCamera().position.y, theRenderer->getCamera().position.z,
				Input::getMousePos(&gameState->inputState).x, Input::getMousePos(&gameState->inputState).y,
				theRenderer->getCamera().yaw, theRenderer->getCamera().pitch);

			theRenderer->renderStringInScreenSpace(fpsstr, primaryFont, 12.0, glm::vec2(5, 5));

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

			theRenderer->renderStringInScreenSpace(str, primaryFont, 14, glm::vec2(5, 5), Rx::TextAlignment::RightAligned);


			size_t ofs = 5;

			for(auto batt : psys->storage)
			{
				double cur = Units::convertJoulesToWattHours(batt->getEnergyInJoules());
				double cap = Units::convertJoulesToWattHours(batt->getCapacityInJoules());

				auto str = tfm::format("%s / %s (%.1f%%)", Units::formatWithUnits(cur, 2, "Wh"),
					Units::formatWithUnits(cap, 2, "Wh"), 100.0 * ((double) cur / (double) cap));

				theRenderer->renderStringInScreenSpace(str, primaryFont, 14, glm::vec2(5, ofs += 15), Rx::TextAlignment::RightAligned);
			}

			str = tfm::format("%s / %s", Units::formatWithUnits(lss->getAtmospherePressure(), 2, "Pa"),
				Units::formatWithUnits(Units::convertKelvinToCelsius(lss->getAtmosphereTemperature()), 1, "°C"));

			theRenderer->renderStringInScreenSpace(str, primaryFont, 14, glm::vec2(5, ofs += 15), Rx::TextAlignment::RightAligned);
		}

		Rx::EndFrame(theRenderer);




















		// more fps computation
		{
			double end = util::Time::ns();
			frameTime = end - frameBegin;

			// don't kill the CPU
			// todo: nanosleep() makes us die and hang.
			// figure out a way...

			#if 0
			{
				double toWait = targetFrameTimeNs - frameTime;

				if(toWait >= 1000 * 1000)
				{
					fprintf(stderr, "fps = %.1f // %.2f ns // %.1f, %.1f\n", currentFps, toWait, targetFrameTimeNs, frameTime);
					struct timespec ts;

					// nanosleep(&ts, 0);
				}
				else
				{
					// todo: we missed our framerate.
				}
			}
			#endif

			prevTimestamp = frameBegin;
		}
	}

	// prof::printResults();

	// Cleanup
	// ImGui_ImplSdl_Shutdown();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(theRenderer->window->sdlWin);
	SDL_Quit();

	return 0;
}


















