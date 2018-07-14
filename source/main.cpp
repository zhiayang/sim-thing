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




static double deltaTimeMultiplier		= 1.0;
static const double fixedDeltaTimeNs	= MS_TO_NS(1.0);

static const double targetFramerate		= 61.0;
static const double targetFrameTimeNs	= S_TO_NS(1.0) / targetFramerate;

static rx::Renderer* theRenderer = 0;
static input::State* inputState = 0;


#define EARTH_RADIUS		6371000
#define MOON_RADIUS			1737000

#define EARTH_MASS			5.972e24
#define MOON_MASS			7.34767309e22



int main(int argc, char** argv)
{
	Config::setResX(960);
	Config::setResY(600);


	// Setup the platform
	auto platformData = platform::Initialise();

	auto primaryFont = rx::getFont("menlo", 64, ' ', 0xFF - ' ', 2, 2);

	double accumulator = 0.0;
	double frameTime = S_TO_NS(0.01667);
	double currentFps = 0.0;

	double prevTimestamp = util::Time::ns();
	double renderDelta = 0;

	inputState = new input::State();


	rx::ShaderPipeline pipeline {

		.shaders = {

			// the main one
			rx::ShaderProgram("forwardShader", rx::ShaderSource {
				.glslVersion = "330 core",
				.vertexShaderPath = "shaders/forward.vs",
				.fragmentShaderPath = "shaders/forward.fs",
			}, rx::SHADER_SUPPORTS_MATERIALS | rx::SHADER_SUPPORTS_CAMERA_POSITION),

			// the gridline
			rx::ShaderProgram("gridlineShader", rx::ShaderSource {
				.glslVersion = "330 core",
				.vertexShaderPath = "shaders/gridline.vs",
				.fragmentShaderPath = "shaders/gridline.fs",
			}, rx::SHADER_SUPPORTS_CAMERA_POSITION)
		},

		// for on-screen text.
		.textShader = rx::ShaderProgram("textShader", rx::ShaderSource {
			.glslVersion = "330 core",
			.vertexShaderPath = "shaders/screentext.vs",
			.fragmentShaderPath = "shaders/screentext.fs",
		}, rx::SHADER_SUPPORTS_NOTHING)
	};




	/*
		TODO list:

		1. !! force applied is in body-space coordinates, including the direction. this should probably
			remain like this, but then we have an issue.

			if the body is rotated (say a yaw), then a force along the z-axis will actually move the body
			in its yawed-direction (see current behaviour with the cube)

			the position of the force should be ok, but should we rotate the force to match the body? or leave
			the rotation of the force as-is??

			if we want to let the force-applier handle the rotation, then we need to rotate the force by the reverse
			of the body's rotation -- which might feel like exposing too much detail???
	*/





















	// camera matrix: camera at [ 70, 30, 70 ], looking at [ 0, 0, 0 ], rotated right-side up
	{
		rx::Camera cam;
		cam.position = lx::vec3(0, 3, 4.5); // lx::vec3(0, 700000000, 0); //
		cam.yaw = -90; // cam.yaw = -110;
		cam.pitch = -20;

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
		theRenderer->addSpotLight(rx::SpotLight(lx::fvec3(0, -4, 0), lx::fvec3(0, 1, 0), util::colour::white(), util::colour::white(),
			0.3, 2.0, 12.5, 30));
	}


	px::World world;

	input::addKeyHandler(inputState,
		{ input::Key::W, input::Key::S, input::Key::A, input::Key::D, input::Key::ShiftL, input::Key::Space, input::Key::MouseL },
		0, [&world](input::State* s, input::Key k, double) -> bool {

		using IK = input::Key;
		auto cam = theRenderer->getCamera();

		// double speed = 100000.01;
		double speed = 0.01;

		if(k == IK::A || k == IK::D || k == IK::W || k == IK::S)
		{
			cam.position += ((k == IK::W || k == IK::S) ? cam.front() : cam.right()) * speed * ((k == IK::S || k == IK::A) ? -1.0f : 1.0f);
		}
		else if(k == IK::Space)
		{
			cam.position.y += speed;
		}
		else if(k == IK::ShiftL)
		{
			cam.position.y -= speed;
		}
		else if(k == IK::MouseL)
		{
			auto f = 600 * cam.front();
			f.y = 0;

			world.bodies[0].addRelForceAt(lx::vec3(0, 1, 0), lx::vec3(0, 0, f.z));
			// world.bodies[0].addTorque(world.bodies[0].mass * 2 * lx::vec3(0, 1, 0));
		}

		theRenderer->updateCamera(cam);
		return true;

	}, input::HandlerKind::WhileDown);



	auto model = rx::loadModelFromAsset(AssetLoader::Load("models/test/test.obj"), 1.0 / 20000.0);

	auto col1 = util::colour(0.247, 0.199, 0.075);
	auto col2 = util::colour(0.752, 0.606, 0.226);
	auto col3 = util::colour(0.628, 0.556, 0.366);
	auto col4 = util::colour(0.200, 0.830, 0.220);
	auto col5 = util::colour(0.992, 0.992, 0.588);
	auto col6 = util::colour(0.830, 0.200, 0.220);

	// auto box = new rx::Texture("textures/box.png");
	// auto box_spec = new rx::Texture("textures/box_spec.png");
	// auto cubeRO = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(util::colour::white(), box, box_spec, 32));

	auto sun = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(col5, col5, col5, 32));
	auto earth = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(col6, col6, col6, 32));

	auto axis_x = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::red(), util::colour::red(), util::colour::red(), 32));

	auto axis_y = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::blue(), util::colour::blue(), util::colour::blue(), 32));

	auto axis_z = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::green(), util::colour::green(), util::colour::green(), 32));


	if((false))
	{
		// earth and moon.
		world.bodies.push_back(px::RigidBody(EARTH_MASS, lx::vec3(0, 0, 0), lx::vec3(0, 0, 0), lx::quat(),
			px::getInertiaMomentOfSphere(EARTH_RADIUS)));

		world.bodies.push_back(px::RigidBody(MOON_MASS, lx::vec3(3.844e8, 0, 0), lx::vec3(0, 0, -1022), lx::quat(),
			px::getInertiaMomentOfSphere(MOON_RADIUS)));

		deltaTimeMultiplier = 500000;
	}
	else
	{
		world.bodies.push_back(px::RigidBody(60, lx::vec3(0, 1, 0), lx::vec3(0),
			lx::quat::fromEulerRads(lx::vec3(0)),
			px::getInertiaMomentOfCuboid(lx::vec3(1))));
	}



	auto gridlines = rx::RenderObject::fromColouredVertices(
		lx::tof(rx::triangulateQuadFace(rx::Face {
			.vertices = {
				lx::vec3(-10, 0, -10),
				lx::vec3(-10, 0, +10),
				lx::vec3(+10, 0, +10),
				lx::vec3(+10, 0, -10),
			}
		}).vertices),
		{
			util::colour::random(),
			util::colour::random(),
			util::colour::random(),
			util::colour::random(),
			util::colour::random(),
			util::colour::random()
		},
		{
			lx::fvec3(0, 1, 0),
			lx::fvec3(0, 1, 0),
			lx::fvec3(0, 1, 0),
			lx::fvec3(0, 1, 0),
			lx::fvec3(0, 1, 0),
			lx::fvec3(0, 1, 0)
		}
	);

	// use the gridline shader.
	gridlines->shaderProgramIndex = 1;







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

				px::stepSimulation(world, NS_TO_S(fixedDeltaTimeNs * deltaTimeMultiplier));

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








		if((false))
		{
			theRenderer->renderObject(sun, lx::mat4().translated(world.bodies[0].position()).scaled(lx::vec3(2 * EARTH_RADIUS)));
			theRenderer->renderObject(earth, lx::mat4().translated(world.bodies[1].position()).scaled(lx::vec3(20 * MOON_RADIUS)));
		}
		else
		{
			theRenderer->renderObject(gridlines, lx::mat4().scaled(50));
			theRenderer->renderObject(earth, lx::mat4()
				.translated(world.bodies[0].position())
				.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
			);

			theRenderer->renderObject(axis_x, lx::mat4()
				.translated(world.bodies[0].position())
				.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
				.scaled(lx::vec3(5, 0.02, 0.02))
			);

			theRenderer->renderObject(axis_y, lx::mat4()
				.translated(world.bodies[0].position())
				.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
				.scaled(lx::vec3(0.02, 5, 0.02))
			);

			theRenderer->renderObject(axis_z, lx::mat4()
				.translated(world.bodies[0].position())
				.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
				.scaled(lx::vec3(0.02, 0.02, 5))
			);



			theRenderer->renderObject(axis_x, lx::mat4()
				.translated(world.bodies[0].position())
				.scaled(lx::vec3(5, 0.02, 0.02))
			);

			theRenderer->renderObject(axis_y, lx::mat4()
				.translated(world.bodies[0].position())
				.scaled(lx::vec3(0.02, 5, 0.02))
			);

			theRenderer->renderObject(axis_z, lx::mat4()
				.translated(world.bodies[0].position())
				.scaled(lx::vec3(0.02, 0.02, 5))
			);
		}



		if((true))
		{
			{
				auto fpsstr = tfm::format("%.2f fps (%.1f ms) / [%.1f, %.1f, %.1f] / [%.0f, %.0f] / (y: %.0f, p: %.0f)",
					currentFps, NS_TO_MS(avgFrameTime), theRenderer->getCamera().position.x,
					theRenderer->getCamera().position.y, theRenderer->getCamera().position.z, input::getMousePos(inputState).x,
					input::getMousePos(inputState).y, theRenderer->getCamera().yaw, theRenderer->getCamera().pitch);

				theRenderer->renderStringInScreenSpace(fpsstr, primaryFont, 12.0, lx::fvec2(5, 5), util::colour::white());
			}

			{
				auto timestr = tfm::format("t: %.2f s", world.worldtime);
				theRenderer->renderStringInScreenSpace(timestr, primaryFont, 12.0, lx::fvec2(5, 5), util::colour::white(),
					rx::TextAlignment::RightAligned);
			}

			{
				auto velstr = tfm::format("vel: %s (%.1f) m/s | L = %s (%.1f) Nms",
					world.bodies[0].velocity(), world.bodies[0].velocity().magnitude(),
					world.bodies[0].angularMomentum(), world.bodies[0].angularMomentum().magnitude());

				theRenderer->renderStringInScreenSpace(velstr, primaryFont, 12.0, lx::fvec2(5, 20), util::colour::white(),
					rx::TextAlignment::LeftAligned);
			}
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


















