// main.cpp
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <glbinding/gl/gl.h>

// linear algebra
#include "lx.h"

// physics (rigidbodies)
#include "px.h"

// renderer
#include "rx.h"
#include "rx/model.h"

#include "config.h"
#include "platform.h"
#include "tinyformat.h"

#include <unistd.h>
#include <array>

static const double deltaTimeMultiplier	= 500000.0;
static const double fixedDeltaTimeNs	= 1.0 * 1000.0 * 1000.0;

static const double targetFramerate		= 61.0;
static const double targetFrameTimeNs	= S_TO_NS(1.0) / targetFramerate;

static rx::Renderer* theRenderer = 0;
static input::State* inputState = 0;

namespace rx
{
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
	Config::setResX(1240);
	Config::setResY(720);


	// Setup the platform
	auto platformData = platform::Initialise();

	auto primaryFont = rx::getFont("menlo", 64, ' ', 0xFF - ' ', 2, 2);

	double accumulator = 0.0;
	double frameTime = S_TO_NS(0.01667);
	double currentFps = 0.0;

	double prevTimestamp = util::Time::ns();
	double renderDelta = 0;

	inputState = new input::State();

	// setup the shader... this is more involved than i'd like, but it's more flexible.
	// sidenote: designated initialisers are amazing, fuck c++ for not having it.
	// constructors do not replace this, especially since it doesn't have named arguments
	// .stupid.

	// 1. shader for forward rendering
	auto forwardProg = rx::ShaderProgram("forwardShader", rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/forwardShader.vs",
		.fragmentShaderPath = "shaders/forwardShader.fs",
	});

	// 2. text shader
	auto textProg = rx::ShaderProgram("textShader", rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/textShader.vs",
		.fragmentShaderPath = "shaders/textShader.fs",
	});

	// 5. screen-filling quad shader
	auto screenQuadProg = rx::ShaderProgram("screenQuadShader", rx::ShaderSource {

		.glslVersion = "330 core",
		.vertexShaderPath = "shaders/deferred/screenquad.vs",
		.fragmentShaderPath = "shaders/deferred/screenquad.fs",
	});


	rx::ShaderPipeline pipeline {

		.forwardShader = forwardProg,
		.textShader = textProg,

		.screenQuadShader = screenQuadProg
	};



	// camera matrix: camera at [ 70, 30, 70 ], looking at [ 0, 0, 0 ], rotated right-side up
	{
		rx::Camera cam;
		cam.position = lx::vec3(0, 600000000, 0);
		cam.yaw = -90;
		cam.pitch = -85;

		// setup the renderer. there's many parameters here...
		theRenderer = new rx::Renderer(
			platformData.first,					// the window
			util::colour(0.01, 0.01, 0.01),		// clear colour
			cam,								// camera
			pipeline,							// shaders for forward rendering, deferred rendering, and text rendering.
			lx::toRadians(70.0f),				// FOV, 70 degrees
			0.001, 100000000000000000			// near plane, far plane
		);

		// position, colour, intensity
		theRenderer->setAmbientLighting(util::colour::white(), 0.4);
		// theRenderer->addPointLight(rx::PointLight(lx::fvec3(0, 10, 10), util::colour::white(), util::colour::white(), 0.7, 15.0));

		// theRenderer->addSpotLight(rx::SpotLight(lx::fvec3(0, -4, 0), lx::fvec3(0, 1, 0), util::colour::white(), util::colour::white(),
		// 	0.3, 2.0, 12.5, 30));
	}


	input::addKeyHandler(inputState,
		{ input::Key::W, input::Key::S, input::Key::A, input::Key::D, input::Key::ShiftL, input::Key::Space },
		0, [](input::State* s, input::Key k, double) -> bool {

		using IK = input::Key;
		auto cam = theRenderer->getCamera();

		double speed = 100000.01;

		if(k == IK::A || k == IK::D || k == IK::W || k == IK::S)
		{
			cam.position += ((k == IK::W || k == IK::S) ? cam.front() : cam.right()) * speed * ((k == IK::S || k == IK::A) ? -1.0f : 1.0f);
		}
		else
		{
			if(k == IK::Space)
			{
				cam.position.y += speed;
			}
			else
			{
				cam.position.y -= speed;
			}
		}

		theRenderer->updateCamera(cam);
		return true;

	}, input::HandlerKind::WhileDown);



	auto model = rx::loadModelFromAsset(AssetLoader::Load("models/test/test.obj"), 1.0 / 20000.0);

	auto col1 = util::colour(0.247, 0.199, 0.075);
	auto col2 = util::colour(0.752, 0.606, 0.226);
	auto col3 = util::colour(0.628, 0.556, 0.366);
	auto col4 = util::colour(0.200, 0.830, 0.220);	// red-dish
	auto col5 = util::colour(0.992, 0.992, 0.588);	// yellow-ish

	// auto box = new rx::Texture("textures/box.png");
	// auto box_spec = new rx::Texture("textures/box_spec.png");
	// auto cubeRO = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(util::colour::white(), box, box_spec, 32));

	auto sun = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(util::colour::white(), col5, col5, 32));
	auto earth = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(util::colour::white(), col4, col4, 32));



	px::World world;
	{
		// sun
		// world.bodies.push_back(px::RigidBody(5000, lx::vec3(0, 0, 0), lx::vec3(0, 0, 0)));

		// earth
		// world.bodies.push_back(px::RigidBody(5, lx::vec3(25, 0, 0), lx::vec3(0, 0, -30)));


		// earth
		world.bodies.push_back(px::RigidBody(5.972e24, lx::vec3(0, 0, 0), lx::vec3(0, 0, 0)));

		// moon
		world.bodies.push_back(px::RigidBody(7.34767309e22, lx::vec3(3.844e8, 0, 0), lx::vec3(0, 0, -1022)));
	}


	double avgFrameTime = 0;

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

			done = input::processEvent(inputState, event);
			if(done) break;
		}


		double frameBegin = util::Time::ns();
		std::tie(currentFps, renderDelta) = determineCurrentFPS(prevTimestamp, frameBegin, frameTime);


		// do updates
		{
			accumulator += frameTime;

			while(accumulator >= fixedDeltaTimeNs)
			{
				accumulator -= fixedDeltaTimeNs;

				px::simulateWorld(world, NS_TO_S(fixedDeltaTimeNs * deltaTimeMultiplier));

				// update the camera based on the mouse, for now.
				{
					bool invert = false;

					double sensitivity = 0.5;

					auto md = input::getMouseChange(inputState);
					auto cam = theRenderer->getCamera();

					cam.pitch = lx::clamp(cam.pitch + md.y * sensitivity * (invert ? -1 : 1), -89.4, +89.4);
					cam.yaw += md.x * sensitivity * -1;

					theRenderer->updateCamera(cam);

					if(theRenderer->spotLights.size() > 0)
					{
						theRenderer->spotLights.back().position = tof(cam.position);
						theRenderer->spotLights.back().direction = tof(cam.front());
					}

					input::Update(inputState, theRenderer->window, fixedDeltaTimeNs);
				}
			}
		}



		rx::PreFrame(theRenderer);
		rx::BeginFrame(theRenderer);


		theRenderer->renderObject(sun, lx::mat4().translate(world.bodies[0].position()).scale(lx::vec3(6371000)));
		theRenderer->renderObject(earth, lx::mat4().translate(world.bodies[1].position()).scale(lx::vec3(1737000)));
		// theRenderer->renderObject(earth, lx::mat4().scale(lx::vec3(1737000)).translate(world.bodies[1].position()));

		// for(const auto& ro : rx::RenderObject::fromModel(model))
		// 	theRenderer->renderObject(ro, lx::mat4());


		if((true))
		{
			std::string fpsstr = tfm::format("%.2f fps (%.1f ms) / [%.1f, %.1f, %.1f] / [%.0f, %.0f] / (y: %.0f, p: %.0f)",
				std::min(currentFps, targetFramerate), avgFrameTime / (1000.0 * 1000.0), theRenderer->getCamera().position.x,
				theRenderer->getCamera().position.y, theRenderer->getCamera().position.z, input::getMousePos(inputState).x,
				input::getMousePos(inputState).y, theRenderer->getCamera().yaw, theRenderer->getCamera().pitch);

			theRenderer->renderStringInScreenSpace(fpsstr, primaryFont, 12.0, lx::fvec2(5, 5), util::colour::white());
		}

		rx::EndFrame(theRenderer);




















		// more fps computation
		{
			double end = util::Time::ns();
			frameTime = end - frameBegin;

			static const double _alpha = 0.2;
			avgFrameTime = _alpha * frameTime + (1 - _alpha) * avgFrameTime;

			prevTimestamp = frameBegin;
		}
	}

	// cleanup
	platform::Uninitialise(theRenderer->window, platformData.second);

	return 0;
}


















