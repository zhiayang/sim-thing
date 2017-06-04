// main.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <glbinding/gl/gl.h>

#include "lx.h"

#include "rx.h"
#include "rx/model.h"

#include "config.h"
#include "platform.h"
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
static rx::Renderer* theRenderer = 0;

namespace rx
{
	// std::pair<SDL_Event, bool> ProcessEvents()
	// {
	// 	bool done = false;

	// 	SDL_Event event;
	// 	while(SDL_PollEvent(&event))
	// 	{
	// 		if(event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
	// 			Input::handleKeyInput(&gameState->inputState, &event);

	// 		else if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN)
	// 			Input::handleMouseInput(&gameState->inputState, &event);

	// 		if(event.type == SDL_QUIT)
	// 		{
	// 			done = true;
	// 		}
	// 		else if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	// 		{
	// 			assert(theRenderer);
	// 			theRenderer->updateWindowSize(event.window.data1, event.window.data2);
	// 		}
	// 	}

	// 	return { event, done };
	// }
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


	// Setup the platform
	auto platformData = platform::Initialise();

	auto primaryFont = rx::getFont("menlo", 64, ' ', 0xFF - ' ', 2, 2);

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
	auto textureProg = rx::ShaderProgram("simpleTexture", rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/simpleTexture.vs",
		.fragmentShaderPath = "shaders/simpleTexture.fs",
	});

	// 2. simple colour shader
	auto colourProg = rx::ShaderProgram("simpleColour", rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/simpleColour.vs",
		.fragmentShaderPath = "shaders/simpleColour.fs",
	});

	// 3. text shader
	auto textProg = rx::ShaderProgram("textShader", rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/textShader.vs",
		.fragmentShaderPath = "shaders/textShader.fs",
	});



	// camera matrix: camera at [ 70, 30, 70 ], looking at [ 0, 0, 0 ], rotated right-side up
	{
		rx::Camera cam;
		cam.position = lx::vec3(0, 1, 2);
		cam.yaw = -90;
		cam.pitch = -20;

		// setup the renderer. there's many parameters here...
		theRenderer = new rx::Renderer(platformData.first,	// the window
			util::colour(0.01, 0.01, 0.01),					// clear colour
			cam,											// camera
			textureProg, colourProg, textProg,				// shader programs for textured objects, coloured objects, and text.
			lx::toRadians(70.0f),							// FOV, 70 degrees
			0.001, 1000										// near plane, far plane
		);

		// position, colour, intensity
		theRenderer->setAmbientLighting(util::colour::white(), 0.2);
		theRenderer->addPointLight(rx::PointLight(lx::vec3(0, 0, 10), util::colour::white(), util::colour::white(), 2.0, 10.0));

		theRenderer->addSpotLight(rx::SpotLight(lx::vec3(0, -4, 0), lx::vec3(0, 1, 0), util::colour::white(), util::colour::white(),
			1.0, 5.0, 12.5, 30));

		// theRenderer->addPointLight(rx::PointLight(lx::vec3(8, 2, 0), util::colour::white(), util::colour::white(),
		// 	1.0, 1.0, 0.022, 0.0019));
	}


	// initialise some things
	// todo: this should go into some savefile-parsing system that does the necessary stuff
	// also should probably be more automatic than this
	gameState = new Sotv::GameState();
	gameState->playerStation = Sotv::Station::makeDefaultSpaceStation("");

	// Input::addKeyHandler(&gameState->inputState, Input::Key::Space, 0, [](Input::State* s, Input::Key k, double) -> bool {

	// 	LOG("Life Support: %s", gameState->playerStation->lifeSupportSystem->toggle() ? "on" : "off");
	// 	return true;

	// }, Input::HandlerKind::PressDown);

	input::addKeyHandler(&gameState->inputState,
		{ input::Key::W, input::Key::S, input::Key::A, input::Key::D, input::Key::ShiftL, input::Key::Space },
		0, [](input::State* s, input::Key k, double) -> bool {

		using IK = input::Key;
		auto cam = theRenderer->getCamera();

		if(k == IK::A || k == IK::D || k == IK::W || k == IK::S)
		{
			cam.position += ((k == IK::W || k == IK::S) ? cam.front() : cam.right()) * 0.005f * ((k == IK::S || k == IK::A) ? -1.0f : 1.0f);
		}
		else
		{
			if(k == IK::Space)
			{
				cam.position.y += 0.005;
			}
			else
			{
				cam.position.y -= 0.005;
			}
		}

		theRenderer->updateCamera(cam);
		return true;

	}, input::HandlerKind::WhileDown);



	auto model = rx::loadModelFromAsset(AssetLoader::Load("models/test/test.obj"), 1.0 / 20000.0);
	// rx::Model* cube = rx::Model::getUnitCube();
	// cube = model;

	auto box = new rx::Texture("textures/box.png");
	auto box_spec = new rx::Texture("textures/box_spec.png");
	auto cubeRO = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(util::colour::white(), box, box_spec, 32));


	// auto col = util::colour(0.83, 0.20, 0.22);
	// auto col1 = util::colour(0.24725, 0.1995, 0.0745);
	// auto col2 = util::colour(0.75164, 0.60648, 0.22648);
	// auto col3 = util::colour(0.628281, 0.555802, 0.366065);
	// auto cubeModel = rx::Model::fromMesh(rx::Mesh::getUnitCube(), rx::Material(col, col, util::colour::white(), 1024.0));
	// auto cubeModel = rx::Model::fromMesh(rx::Mesh::getUnitCube(), rx::Material(col1, col2, col3, 0.4 * 128));


	// Main loop
	bool done = false;
	while(!done)
	{
		// get and process events
		auto events = platform::getEvents(platformData.second);
		for(auto event : events)
		{
			if(event.ignore)
				continue;

			if(event.type == input::Event::Type::WindowResize)
				theRenderer->updateWindowSize(event.windowWidth, event.windowHeight);

			done = input::processEvent(&gameState->inputState, event);
			if(done) break;
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
					auto md = input::getMouseChange(&gameState->inputState);
					auto cam = theRenderer->getCamera();

					// fprintf(stderr, "delta = (%.0f, %.0f)\n", md.x, md.y);
					cam.pitch = lx::clamp(cam.pitch + md.y * sensitivity * (invert ? -1 : 1), -89.4, +89.4);
					cam.yaw += md.x * sensitivity;

					theRenderer->updateCamera(cam);

					theRenderer->spotLights.back().position = cam.position;
					theRenderer->spotLights.back().direction = cam.front();

					input::Update(&gameState->inputState, theRenderer->window, fixedDeltaTimeNs * timeSpeedupFactor);
				}
			}
		}



		rx::PreFrame(theRenderer);
		rx::BeginFrame(theRenderer);



		Sotv::Render(*gameState, renderDelta, theRenderer);

		// theRenderer->renderMesh(rx::Mesh::getUnitCube(), glm::translate(glm::mat4(), lx::vec3(0, -2, 0)), lx::vec4(0.24, 0.59, 0.77, 1.0));

		// theRenderer->renderModel(model, glm::translate(glm::mat4(), lx::vec3(0, 0, 0)), util::colour(0.83, 0.20, 0.22));
		// theRenderer->renderMesh(rx::Mesh::getUnitCube(), glm::scale(glm::mat4(), lx::vec3(0.5)), lx::vec4(0.24, 0.59, 0.77, 1.0));
		theRenderer->renderObject(cubeRO, lx::mat4());
		theRenderer->renderObject(cubeRO, lx::mat4().translate(lx::vec3(0, 4, 0)));

		// theRenderer->renderModel(cube, glm::translate(glm::mat4(), lx::vec3(0, 0, 2)), lx::vec4(0.24, 0.59, 0.77, 1.0));
		// theRenderer->renderModel(cube, glm::translate(glm::scale(glm::mat4(), lx::vec3(0.1)), lx::vec3(0, 20, 0)), util::colour::white());




		if((true))
		{
			std::string fpsstr = tfm::format("%.2f fps / [%.1f, %.1f, %.1f] / [%.0f, %.0f] / (y: %.0f, p: %.0f)", currentFps,
				theRenderer->getCamera().position.x, theRenderer->getCamera().position.y, theRenderer->getCamera().position.z,
				input::getMousePos(&gameState->inputState).x, input::getMousePos(&gameState->inputState).y,
				theRenderer->getCamera().yaw, theRenderer->getCamera().pitch);

			theRenderer->renderStringInScreenSpace(fpsstr, primaryFont, 12.0, lx::vec2(5, 5), util::colour::white());

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

			theRenderer->renderStringInScreenSpace(str, primaryFont, 14, lx::vec2(5, 5), util::colour::white(),
				rx::TextAlignment::RightAligned);


			size_t ofs = 5;

			for(auto batt : psys->storage)
			{
				double cur = Units::convertJoulesToWattHours(batt->getEnergyInJoules());
				double cap = Units::convertJoulesToWattHours(batt->getCapacityInJoules());

				auto str = tfm::format("%s / %s (%.1f%%)", Units::formatWithUnits(cur, 2, "Wh"),
					Units::formatWithUnits(cap, 2, "Wh"), 100.0 * ((double) cur / (double) cap));

				theRenderer->renderStringInScreenSpace(str, primaryFont, 14, lx::vec2(5, ofs += 15), util::colour::white(),
					rx::TextAlignment::RightAligned);
			}

			str = tfm::format("%s / %s", Units::formatWithUnits(lss->getAtmospherePressure(), 2, "Pa"),
				Units::formatWithUnits(Units::convertKelvinToCelsius(lss->getAtmosphereTemperature()), 1, "°C"));

			theRenderer->renderStringInScreenSpace(str, primaryFont, 14, lx::vec2(5, ofs += 15), util::colour::white(),
				rx::TextAlignment::RightAligned);
		}

		rx::EndFrame(theRenderer);




















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

	// cleanup
	platform::Uninitialise(theRenderer->window, platformData.second);

	return 0;
}


















