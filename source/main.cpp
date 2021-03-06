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
			}, rx::SHADER_SUPPORTS_MATERIALS | rx::SHADER_SUPPORTS_CAMERA_POSITION
			| rx::SHADER_SUPPORTS_TRANS_INV_MODELMAT),

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
		NOTE: world axis follows opengl coord system
		Z-toward you
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
		// cam.position = lx::vec3(0, 6, 4.5); // lx::vec3(0, 700000000, 0);

		cam.position = lx::vec3(1, 0.5, 0); // lx::vec3(0, 700000000, 0); //
		cam.yaw = -180; // cam.yaw = -110;
		cam.pitch = -10;

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
		theRenderer->setAmbientLighting(util::colour::white(), 0.35);
		theRenderer->addSpotLight(rx::SpotLight(lx::fvec3(0, -4, 0), lx::fvec3(0, 1, 0), util::colour::white(), util::colour::white(),
			0.1, 1.0, 0.5, 30));
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
		double prev_rot_error = 0;
		double accum_rot_error = 0;

		const double rot_Kp = 1.5000;
		const double rot_Ki = 0.0000;
		const double rot_Kd = 0.0040;

		double prev_lin_error = 0;
		double accum_lin_error = 0;

		const double lin_Kp = 0.1000;
		const double lin_Ki = 0.0000;
		const double lin_Kd = 0.4000;

		double prev_pit_error = 0;
		double accum_pit_error = 0;

		const double pit_Kp = 80.000;
		const double pit_Ki = 0.0000;
		const double pit_Kd = 5.0000;

		double prev_dep_error = 0;
		double accum_dep_error = 0;

		const double dep_Kp = 20.000;
		const double dep_Ki = 0.0000;
		const double dep_Kd = 4.0000;

		lx::vec3 set_pos;
		lx::vec3 set_vel;

		lx::vec3 prev_vel_error;
		lx::vec3 accum_vel_error;
		const double vel_Kp = 0.1000;
		const double vel_Ki = 0.0000;
		const double vel_Kd = 0.4000;

		double start_time = 0;
		double cur_time = 0;
		double end_time = 0;

		const double target_vel = 0.25;
		void set(const px::RigidBody& rb, const lx::vec3& pos)
		{
			auto dist = (pos - rb.position()).magnitude();
			end_time = dist / target_vel;
		}

		auto update(const px::RigidBody& rb, double dt) -> decltype(thruster_control)
		{
			cur_time += dt;

			// current algo is to ensure rotation is good before we start moving.
			decltype(thruster_control) ret;

			auto makepid = [&dt](auto e, auto p_e, auto& a_e, double kp, double ki, double kd) -> auto {
				auto d_error = e - p_e;

				auto p = (kp * e);
				auto i = (ki * (a_e += (dt * e)));
				auto d = (kd * (d_error / dt));

				return p + i + d;
			};

			// TODO: superposition of thruster value: scale everything by the max value

			// linear control

			#if 0
			{
				// ok we can start to move now.
				auto error = set_pos.xz().magnitude() - rb.position().xz().magnitude();
				auto res = makepid(error, prev_lin_error, accum_lin_error, lin_Kp, lin_Ki, lin_Kd);

				// if the target is "in front" of us, then we will always want to thrust forward.
				// only thrust backwards if we overshot, but it's still "behind us" -- ie the rotation
				// controller didn't make us do a 180.
				if(lx::abs(prev_rot_error) < (lx::PI / 2))
					res = lx::abs(res);

				auto thrust = lx::clamp(res, -0.75, 0.75);

				if(lx::abs(prev_rot_error) < (lx::PI / 3) || error < 1.5)
				{
					// once we approach the target (2m), we want to give the rotation control slightly more leverage.
					auto bound = lx::lerp(0.25, 0.75, lx::min(lx::abs(error), 2) / 2.0);

					ret.fwd_l = lx::clamp(ret.fwd_l + thrust, -bound, bound);
					ret.fwd_r = lx::clamp(ret.fwd_r + thrust, -bound, bound);
				}

				prev_lin_error = error;
			}
			#else
			{
				// wtf? how does this work

				auto cur_vel = rb.velocity();

				auto error = set_vel - cur_vel;
				makepid(error, prev_vel_error, accum_vel_error, vel_Kp, vel_Ki, vel_Kd);
			}
			#endif


			// yaw control
			{
				auto set_angle = lx::atan2(-(set_pos.x - rb.position().x), -(set_pos.z - rb.position().z));

				auto error = (set_angle - rb.rotation().toEulerRads().y);
				if(error > lx::PI)
					error = error - lx::TAU;

				auto res = makepid(error, prev_rot_error, accum_rot_error, rot_Kp, rot_Ki, rot_Kd);

				auto thrust = res;


				auto rot_err_deg = lx::toDegrees(prev_rot_error);

				// don't abs! if not we'll do a 180 if we overshoot ):
				// alternatively if we're far away then sure, do a 180.
				if(prev_lin_error > 0.05 || lx::abs(prev_lin_error) > 3.0
					|| (!(rot_err_deg > -185 && rot_err_deg < -175)))
				{
					ret.fwd_l = lx::clamp(ret.fwd_l - thrust, -0.75, 0.75);
					ret.fwd_r = lx::clamp(ret.fwd_r + thrust, -0.75, 0.75);
				}

				prev_rot_error = error;
			}

			// pitch control
			{
				auto set_angle = 0;
				auto error = (set_angle - rb.rotation().toEulerRads().x);

				auto res = makepid(error, prev_pit_error, accum_pit_error, pit_Kp, pit_Ki, pit_Kd);
				auto thrust = res;

				// only use the front thruster... i guess?
				ret.vert_front = lx::clamp(ret.vert_front - thrust, -0.75, 0.75);
				prev_pit_error = error;

				// printf("thrust: %.3f\n", thrust);
			}


			// depth control
			{
				auto set_depth = 1.5;
				auto error = (set_depth - rb.position().y);

				auto res = makepid(error, prev_dep_error, accum_dep_error, dep_Kp, dep_Ki, dep_Kd);
				auto thrust = res;

				// only use the rear thruster...? let the pitch thruster maintain us.
				ret.vert_back = lx::clamp(ret.vert_back - thrust, -0.75, 1.0);
				prev_dep_error = error;

				// printf("thrust: %.3f\n", thrust);
			}

			return ret;
		}
	};


	auto world = px::World();
	bool bouyancy = false;

	input::addKeyHandler(inputState, {
		input::Key::Comma, input::Key::Period, input::Key::X, input::Key::O, input::Key::B
	}, 0, [&thruster_control, &upd, &bouyancy](input::State* s, input::Key k, double) -> bool {

		using IK = input::Key;
		if(k == IK::Comma)
		{
			deltaTimeMultiplier /= 2.0;
		}
		else if(k == IK::Period)
		{
			deltaTimeMultiplier *= 2.0;
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
		else if(k == IK::B)
		{
			bouyancy = !bouyancy;
		}

		return true;
	}, input::HandlerKind::PressDown);

	input::addKeyHandler(inputState,
		{
			input::Key::W, input::Key::S, input::Key::A, input::Key::D,
			input::Key::ShiftL, input::Key::Space, input::Key::MouseL, input::Key::R,

			input::Key::H, input::Key::N, input::Key::I, input::Key::J, input::Key::K, input::Key::L,
		},
		0, [&thruster_control, &world](input::State* s, input::Key k, double) -> bool {

		using IK = input::Key;
		auto cam = theRenderer->getCamera();

		// double speed = 100000.01;
		double speed = 0.002;


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
		else if(k == IK::R)
		{
			world.bodies[0]._pos = lx::vec3(0, 0.45, 0);
			world.bodies[0]._rot = lx::quat();

			world.bodies[0]._linearMtm = lx::vec3(0);
			world.bodies[0]._angularMtm = lx::vec3(0);
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
			thruster_control.vert_front = lx::clamp(thruster_control.vert_front - thrust_rate, -0.75, 1);
			thruster_control.vert_back = lx::clamp(thruster_control.vert_back - thrust_rate, -0.75, 1);
		}
		else if(k == IK::I)
		{
			thruster_control.vert_front = lx::clamp(thruster_control.vert_front + thrust_rate, -0.75, 1);
			thruster_control.vert_back = lx::clamp(thruster_control.vert_back + thrust_rate, -0.75, 1);
		}
		else if(k == IK::J)
		{
			thruster_control.fwd_l = lx::clamp(thruster_control.fwd_l - thrust_rate, -0.75, 0.75);
			thruster_control.fwd_r = lx::clamp(thruster_control.fwd_r + thrust_rate, -0.75, 0.75);
		}
		else if(k == IK::L)
		{
			thruster_control.fwd_l = lx::clamp(thruster_control.fwd_l + thrust_rate, -0.75, 0.75);
			thruster_control.fwd_r = lx::clamp(thruster_control.fwd_r - thrust_rate, -0.75, 0.75);
		}



		theRenderer->updateCamera(cam);
		return true;

	}, input::HandlerKind::WhileDown);



	auto model = rx::loadModelFromAsset(AssetLoader::Load("models/hornet/hornet.obj"), 1.0);

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
	// auto earth = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(), rx::Material(col2, col2, col2, 32));

	auto axis_x = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::red(), util::colour::red(), util::colour::red(), 32));

	auto axis_y = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::green(), util::colour::green(), util::colour::green(), 32));

	auto axis_z = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::blue(), util::colour::blue(), util::colour::blue(), 32));

	auto axis_w = rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(),
		rx::Material(util::colour::red(), util::colour::red(), util::colour::red(), 32));


	std::vector<rx::RenderObject*> auvParts;
	rx::SpotLight* auvLight = 0;
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
		auto colour = util::colour::fromHexRGB(0xFBA7A7);

		auto dims = lx::vec3(0.5, 0.45, 0.7);
		// auvParts = { rx::RenderObject::fromMesh(rx::Mesh::getUnitCube(dims),
		// 	rx::Material(colour, colour, colour, 32)) };

		auvParts = rx::RenderObject::fromModel(model);

		world.bodies.push_back(px::RigidBody(
			/* mass: */     30,
			/* position: */ lx::vec3(0, 0.45, 0),
			/* velocity: */ lx::vec3(0),
			/* rotation: */ lx::quat::fromEulerDegs(lx::vec3(0, 90, 30)),
			/* inertia:  */ px::getInertiaMomentOfCuboid(/* non uniform inertias are BROKEN! */ lx::vec3(0.9)),
			/* collider: */ rx::Mesh::getUnitCube(dims)
		));

		auvLight = &theRenderer->addSpotLight(rx::SpotLight(lx::fvec3(0), lx::fvec3(0),
			util::colour::white(), util::colour::white(), 0.02, 30.0, 0.0, 70));

		world.bodies[0].surfaceArea = 0.9*0.8;
		world.bodies[0].dragCoefficient = 0.75;

		deltaTimeMultiplier = 1;
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
			util::colour::white(),
			util::colour::white(),
			util::colour::white(),
			util::colour::white(),
			util::colour::white(),
			util::colour::white()
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


	auto groundplane = rx::RenderObject::fromColouredVertices(
		lx::tof(rx::triangulateQuadFace(rx::Face {
			.vertices = {
				lx::vec3(-10, 0, -10),
				lx::vec3(-10, 0, +10),
				lx::vec3(+10, 0, +10),
				lx::vec3(+10, 0, -10),
			}
		}).vertices),
		{
			util::colour::fromHexRGB(0x0E5183),
			util::colour::fromHexRGB(0x0E5183),
			util::colour::fromHexRGB(0x0E5183),
			util::colour::fromHexRGB(0x0E5183),
			util::colour::fromHexRGB(0x0E5183),
			util::colour::fromHexRGB(0x0E5183),
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
	pidController.set_pos = lx::vec3(-15, 0, 10);
	theRenderer->addPointLight(rx::PointLight(lx::tof(pidController.set_pos + lx::vec3(0, 1, 0)),
		util::colour::red(), util::colour::red(), 1.0, 5.0));


	double avgFrameTime = 0;

	const auto fwd_l_pos = lx::vec3(-0.18125, 0.0685, 0.075);
	const auto fwd_r_pos = lx::vec3(+0.18125, 0.0685, 0.075);
	const auto vert_f_pos = lx::vec3(0, -0.075, -0.405);
	const auto vert_b_pos = lx::vec3(0, -0.075, +0.095);



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



		// fucks up if we get too many rotational things going on ):
		{
			auto max = 200;
			auto& body = world.bodies[0];

			body.addRelForceAt(fwd_l_pos, max * lx::vec3(0, 0, -thruster_control.fwd_l));
			body.addRelForceAt(fwd_r_pos, max * lx::vec3(0, 0, -thruster_control.fwd_r));
			body.addRelForceAt(vert_f_pos, max * lx::vec3(0, -thruster_control.vert_front, 0));
			body.addRelForceAt(vert_b_pos, max * lx::vec3(0, -thruster_control.vert_back, 0));

			body.addRelForceAt(lx::vec3(0, 0, +0.6), max * lx::vec3(thruster_control.yaw, 0, 0));

			if(bouyancy)
				body.addRelForceAt(lx::vec3(0, 0.3, -0.2), body.rotation().inversed() * lx::vec3(0, 50.0, 0));

			auto transform = lx::mat4()
				.translated(world.bodies[0].position())
				.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis());

			auto relpos = lx::vec3(0, 0.7, -0.5);
			auto reldir = lx::vec3(0, -0.1, -2.5).normalised();

			auvLight->position = lx::tof(transform * lx::vec4(relpos, 1)).xyz();
			auvLight->direction = lx::tof(transform.upper3x3() * reldir).xyz();
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
						theRenderer->spotLights.front().position = lx::tof(cam.position);
						theRenderer->spotLights.front().direction = lx::tof(cam.front());
					}
				}


				{
					auto tc = pidController.update(world.bodies[0],
						NS_TO_S(fixedDeltaTimeNs * deltaTimeMultiplier));

					if(upd)
						thruster_control = tc;
				}

				input::Update(inputState, theRenderer->window, fixedDeltaTimeNs);
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
			// theRenderer->renderObject(gridlines, lx::mat4().scaled(50));
			theRenderer->renderObject(groundplane, lx::mat4().translated(lx::vec3(0, -0.1, 0)).scaled(50));

			for(const auto& part : auvParts)
			{
				theRenderer->renderObject(part, lx::mat4()
					.translated(world.bodies[0].position())
					.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
				);
			}

			theRenderer->renderObject(axis_x, lx::mat4()
				.translated(world.bodies[0].position())
				.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
				.scaled(lx::vec3(1.2, 0.015, 0.015))
			);

			theRenderer->renderObject(axis_y, lx::mat4()
				.translated(world.bodies[0].position())
				.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
				.scaled(lx::vec3(0.015, 2, 0.015))
			);

			theRenderer->renderObject(axis_z, lx::mat4()
				.translated(world.bodies[0].position())
				.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
				.scaled(lx::vec3(0.015, 0.015, 2))
			);


			theRenderer->renderObject(axis_x, lx::mat4()
				.translated(world.bodies[0].position())
				.scaled(lx::vec3(1.2, 0.015, 0.015))
			);

			theRenderer->renderObject(axis_y, lx::mat4()
				.translated(world.bodies[0].position())
				.scaled(lx::vec3(0.015, 2, 0.015))
			);

			theRenderer->renderObject(axis_z, lx::mat4()
				.translated(world.bodies[0].position())
				.scaled(lx::vec3(0.015, 0.015, 2))
			);



			if(lx::abs(thruster_control.vert_front) > 0)
			{
				theRenderer->renderObject(axis_w, lx::mat4()
					.translated(world.bodies[0].position())
					.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
					.translated(vert_f_pos + lx::vec3(0, thruster_control.vert_front / 4, 0))
					.scaled(lx::vec3(0.025, lx::abs(thruster_control.vert_front) / 2, 0.025))
				);
			}

			if(lx::abs(thruster_control.vert_back) > 0)
			{
				theRenderer->renderObject(axis_w, lx::mat4()
					.translated(world.bodies[0].position())
					.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
					.translated(vert_b_pos + lx::vec3(0, thruster_control.vert_back / 4, 0))
					.scaled(lx::vec3(0.025, lx::abs(thruster_control.vert_back) / 2, 0.025))
				);
			}

			if(lx::abs(thruster_control.fwd_l) > 0)
			{
				theRenderer->renderObject(axis_w, lx::mat4()
					.translated(world.bodies[0].position())
					.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
					.translated(fwd_l_pos + lx::vec3(0, 0, thruster_control.fwd_l / 4))
					.scaled(lx::vec3(0.025, 0.025, lx::abs(thruster_control.fwd_l) / 2))
				);
			}

			if(lx::abs(thruster_control.fwd_r) > 0)
			{
				theRenderer->renderObject(axis_w, lx::mat4()
					.translated(world.bodies[0].position())
					.rotated(world.bodies[0].rotation().angle(), world.bodies[0].rotation().axis())
					.translated(fwd_r_pos + lx::vec3(0, 0, thruster_control.fwd_r / 4))
					.scaled(lx::vec3(0.025, 0.025, lx::abs(thruster_control.fwd_r) / 2))
				);
			}



			theRenderer->renderObject(axis_y, lx::mat4()
				.translated(pidController.set_pos)
				.scaled(lx::vec3(0.02, 10, 0.02))
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

				timestr = tfm::format("m: %.2fx", deltaTimeMultiplier);
				theRenderer->renderStringInScreenSpace(timestr, primaryFont, 12.0, lx::fvec2(5, 20), util::colour::white(),
					rx::TextAlignment::RightAligned);
			}

			{
				auto velstr = tfm::format("vel: %s (%.1f) m/s | ang. mtm: %s (%.1f) Nms",
					world.bodies[0].velocity(), world.bodies[0].velocity().magnitude(),
					world.bodies[0].angularMomentum(), world.bodies[0].angularMomentum().magnitude());

				theRenderer->renderStringInScreenSpace(velstr, primaryFont, 12.0, lx::fvec2(5, 20), util::colour::white(),
					rx::TextAlignment::LeftAligned);
			}

			{
				auto pos = world.bodies[0].position();
				auto rot = world.bodies[0].rotation().toEulerDegs();

				auto thrstr = tfm::format("pos: (%.1f, %.1f, %.1f) | roll: [%.2f] / pitch[%.2f] / yaw[%.2f]",
					pos.x, pos.y, pos.z, rot.z, rot.x, rot.y);

				theRenderer->renderStringInScreenSpace(thrstr, primaryFont, 12.0, lx::fvec2(5, 35), util::colour::white(),
					rx::TextAlignment::LeftAligned);
			}

			{
				auto thrstr = tfm::format("thrusters: fwd[%.2f/%.2f] / vert[%.2f/%.2f] / yaw[%.2f]",
					thruster_control.fwd_l, thruster_control.fwd_r, thruster_control.vert_front, thruster_control.vert_back,
					thruster_control.yaw);

				theRenderer->renderStringInScreenSpace(thrstr, primaryFont, 12.0, lx::fvec2(5, 50), util::colour::white(),
					rx::TextAlignment::LeftAligned);
			}

			{
				auto pidstr = tfm::format("pid_errs: rot[%.2f deg] / lin[%.2f m] / pit[%.2f deg] / dep[%.2f m]",
					lx::toDegrees(pidController.prev_rot_error), pidController.prev_lin_error,
					lx::toDegrees(pidController.prev_pit_error), pidController.prev_dep_error);

				theRenderer->renderStringInScreenSpace(pidstr, primaryFont, 12.0, lx::fvec2(5, 65), util::colour::white(),
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


















