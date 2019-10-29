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
		NOTE: world axis follows right-hand-rule!
		Z-front
		X-left
		Y-up

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





















	// camera matrix
	{
		rx::Camera cam;
		cam.position = lx::vec3(0, 6, 4.5); // lx::vec3(0, 700000000, 0); //
		cam.yaw = -90; // cam.yaw = -110;
		cam.pitch = -50;

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


	struct {
		double fwd_l = 0;
		double fwd_r = 0;

		double vert_front = 0;
		double vert_back = 0;

		double yaw = 0;

	} thruster_control;


	bool upd = false;

	struct pid_controller
	{
		lx::vec3 set_pos;

		double prev_rot_error = 0;
		double accum_rot_error = 0;

		const double rot_Kp = 1.5000;
		const double rot_Ki = 0.0005;
		const double rot_Kd = 0.0050;

		double prev_lin_error = 0;
		double accum_lin_error = 0;

		const double lin_Kp = 0.1500;
		const double lin_Ki = 0.0005;
		const double lin_Kd = 0.0200;

		auto update(const px::RigidBody& rb, double dt) -> decltype(thruster_control)
		{
			// current algo is to ensure rotation is good before we start moving.
			decltype(thruster_control) ret;

			// this is still fucked up if we need to turn > 90 degrees ):
			auto set_angle = -1 * lx::atan2(set_pos.x - rb.position().x, -(set_pos.z - rb.position().z));

			if(lx::abs(prev_lin_error) > 0.5)
			{
				auto error = (set_angle - rb.rotation().toEulerRads().y);
				auto d_error = error - prev_rot_error;

				auto p = (rot_Kp * error);
				auto i = (rot_Ki * (accum_rot_error += (dt * error)));
				auto d = (rot_Kd * (d_error / dt));

				auto thrust = (p + i + d);
				// ret.yaw = lx::clamp(thrust, -0.75, 0.75);

				ret.fwd_l = lx::clamp(-thrust, -0.75, 0.75);
				ret.fwd_r = lx::clamp(thrust, -0.75, 0.75);

				prev_rot_error = error;
			}


			if(lx::abs(prev_rot_error) < 0.5)
			{
				// ok we can start to move now.
				auto error = (set_pos.xz() - rb.position().xz()).magnitude();
				if(rb.position().xz().magnitudeSquared() > set_pos.xz().magnitudeSquared())
					error *= -1;

				auto d_error = error - prev_lin_error;

				auto p = (lin_Kp * error);
				auto i = (lin_Ki * (accum_lin_error += (dt * error)));
				auto d = (lin_Kd * (d_error / dt));

				auto thrust = lx::clamp(p + i + d, -0.75, 0.75);
				ret.fwd_l = lx::clamp(ret.fwd_l + thrust, -0.75, 1);
				ret.fwd_r = lx::clamp(ret.fwd_r + thrust, -0.75, 1);

				prev_lin_error = error;
			}

			return ret;
		}
	};


	auto world = px::World();

	input::addKeyHandler(inputState,
		{
			input::Key::W, input::Key::S, input::Key::A, input::Key::D,
			input::Key::ShiftL, input::Key::Space, input::Key::MouseL,

			input::Key::X, input::Key::H, input::Key::N, input::Key::I, input::Key::J, input::Key::K, input::Key::L,
			input::Key::U, input::Key::O, input::Key::G, input::Key::B
		},
		0, [&world, &thruster_control, &upd](input::State* s, input::Key k, double) -> bool {

		using IK = input::Key;
		auto cam = theRenderer->getCamera();

		// double speed = 100000.01;
		double speed = 0.007;


		constexpr double thrust_rate = 0.0004;
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
			// auto f = 600 * cam.front();
			// f.y = 0;

			// auto& b = world.bodies[0];
			// b.addRelForceAt(lx::vec3(-.3, 0, 0), lx::vec3(0, 0, -60));
			// b.addRelForceAt(lx::vec3(+.3, 0, 0), lx::vec3(0, 0, -60));
			// world.bodies[0].addTorque(world.bodies[0].mass * 2 * lx::vec3(0, 1, 0));
		}
		else if(k == IK::H)
		{
			thruster_control.fwd_l = lx::clamp(thruster_control.fwd_l + thrust_rate, -0.75, 1);
			thruster_control.fwd_r = lx::clamp(thruster_control.fwd_r + thrust_rate, -0.75, 1);
		}
		else if(k == IK::N)
		{
			thruster_control.fwd_l = lx::clamp(thruster_control.fwd_l - thrust_rate, -0.75, 1);
			thruster_control.fwd_r = lx::clamp(thruster_control.fwd_r - thrust_rate, -0.75, 1);
		}
		else if(k == IK::K)
		{
			thruster_control.vert_front = lx::clamp(thruster_control.vert_front + thrust_rate, -0.75, 1);
			thruster_control.vert_back = lx::clamp(thruster_control.vert_back + thrust_rate, -0.75, 1);
		}
		else if(k == IK::I)
		{
			thruster_control.vert_front = lx::clamp(thruster_control.vert_front - thrust_rate, -0.75, 1);
			thruster_control.vert_back = lx::clamp(thruster_control.vert_back - thrust_rate, -0.75, 1);
		}
		else if(k == IK::J)
		{
			thruster_control.yaw = lx::clamp(thruster_control.yaw + thrust_rate, -0.75, 0.75);
		}
		else if(k == IK::L)
		{
			thruster_control.yaw = lx::clamp(thruster_control.yaw - thrust_rate, -0.75, 0.75);
		}
		else if(k == IK::O)
		{
			upd = true;
		}
		else if(k == IK::X)
		{
			thruster_control = { 0 };
			upd = false;
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

	// auto sun = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(col5, col5, col5, 32));
	auto earth = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(col5, col5, col5, 32));

	auto axis_x = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::red(), util::colour::red(), util::colour::red(), 32));

	auto axis_y = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::blue(), util::colour::blue(), util::colour::blue(), 32));

	auto axis_z = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::green(), util::colour::green(), util::colour::green(), 32));


	if constexpr ((false))
	{
		// earth and moon.
		world.bodies.push_back(px::RigidBody(EARTH_MASS, lx::vec3(0, 0, 0), lx::vec3(0, 0, 0), lx::quat(),
			px::getInertiaMomentOfSphere(EARTH_RADIUS), rx::Mesh::getUnitCube(2 * EARTH_RADIUS)));

		world.bodies.push_back(px::RigidBody(MOON_MASS, lx::vec3(3.844e8, 0, 0), lx::vec3(0, 0, -1022), lx::quat(),
			px::getInertiaMomentOfSphere(MOON_RADIUS), rx::Mesh::getUnitCube(2 * MOON_RADIUS)));

		deltaTimeMultiplier = 500000;
	}
	else
	{
		auto dims = lx::vec3(0.9, 0.8, 1.4);
		earth = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(dims),
			rx::Material(col5, col5, col5, 32));

		world.bodies.push_back(px::RigidBody(
			/* mass: */     40,
			/* position: */ lx::vec3(0, 1, 0),
			/* velocity: */ lx::vec3(0),
			/* rotation: */ lx::quat::fromEulerRads(lx::vec3(0, 0, 0)),
			/* inertia:  */ px::getInertiaMomentOfCuboid(dims),
			/* collider: */ rx::Mesh::getUnitCube()
		));

		world.bodies[0].surfaceArea = 0.9*0.8;
		world.bodies[0].dragCoefficient = 1.05;
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





	auto pidController = pid_controller();
	pidController.set_pos = lx::vec3(-15, 0, -10);



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



		{
			double max_thrust = 66;
			// double max_thrust = 200;
			auto max = max_thrust;
			// auto rot = world.bodies[0].rotation().toRotationMatrix();

			world.bodies[0].addRelForceAt(lx::vec3(-0.4, 0, +0.6), max * lx::vec3(0, 0, -thruster_control.fwd_l));
			world.bodies[0].addRelForceAt(lx::vec3(+0.4, 0, +0.6), max * lx::vec3(0, 0, -thruster_control.fwd_r));
			world.bodies[0].addRelForceAt(lx::vec3(0, 0, +0.6), max * lx::vec3(0, thruster_control.vert_front, 0));
			world.bodies[0].addRelForceAt(lx::vec3(0, 0, -0.6), max * lx::vec3(0, thruster_control.vert_back, 0));
			world.bodies[0].addRelForceAt(lx::vec3(0, 0, +0.6), max * lx::vec3(thruster_control.yaw, 0, 0));

			auto thrstr = tfm::format("thrusters: fwd[%.2f/%.2f] / vert_f[%.2f] / vert_b[%.2f] / yaw[%.2f]",
				thruster_control.fwd_l, thruster_control.fwd_r, thruster_control.vert_front, thruster_control.vert_back,
				thruster_control.yaw);

			theRenderer->renderStringInScreenSpace(thrstr, primaryFont, 12.0, lx::fvec2(5, 35), util::colour::white(),
				rx::TextAlignment::LeftAligned);


			auto pidstr = tfm::format("pid: rot_err[%.2f deg] / lin_err[%.2f m]",
				lx::toDegrees(pidController.prev_rot_error), pidController.prev_lin_error);

			theRenderer->renderStringInScreenSpace(pidstr, primaryFont, 12.0, lx::fvec2(5, 50), util::colour::white(),
				rx::TextAlignment::LeftAligned);
		}






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

				if(upd)
				thruster_control = pidController.update(world.bodies[0], NS_TO_S(fixedDeltaTimeNs * deltaTimeMultiplier));
			}
		}



		rx::PreFrame(theRenderer);
		rx::BeginFrame(theRenderer);










		if constexpr ((false))
		{
			// theRenderer->renderObject(sun, lx::mat4().translated(world.bodies[0].position()).scaled(lx::vec3(2 * EARTH_RADIUS)));
			// theRenderer->renderObject(earth, lx::mat4().translated(world.bodies[1].position()).scaled(lx::vec3(20 * MOON_RADIUS)));
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



			theRenderer->renderObject(axis_y, lx::mat4()
				.translated(pidController.set_pos)
				.scaled(lx::vec3(0.02, 10, 0.02))
			);


			// theRenderer->renderObject(axis_x, lx::mat4()
			// 	.translated(world.bodies[0].position())
			// 	.scaled(lx::vec3(5, 0.02, 0.02))
			// );

			// theRenderer->renderObject(axis_y, lx::mat4()
			// 	.translated(world.bodies[0].position())
			// 	.scaled(lx::vec3(0.02, 5, 0.02))
			// );

			// theRenderer->renderObject(axis_z, lx::mat4()
			// 	.translated(world.bodies[0].position())
			// 	.scaled(lx::vec3(0.02, 0.02, 5))
			// );
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


















