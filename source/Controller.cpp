// Controller.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <deque>

#include "config.h"
#include "controller.h"




// static const double fixedDeltaTimeNs	= 20.0 * 1000.0 * 1000.0;
// static const double targetFramerate		= 60.0;
// static const double targetFrameTimeNs	= S_TO_NS(1.0) / targetFramerate;

// void Controller::UpdateLoop()
// {
	// using namespace std::chrono;
	// // SDL::Texture* text = new SDL::Texture("aqua.png", this->renderer);
	// // delete text;

	// double accumulator = 0.0;
	// double currentTime = this->timeInfo.ns();

	// // no point checking for this->run here, since we will exit out if its false first thing.
	// while(this->run)
	// {
	// 	double newTime = this->timeInfo.ns();
	// 	double frameTime = newTime - currentTime;
	// 	currentTime = newTime;

	// 	accumulator += frameTime;

	// 	while(this->run && accumulator >= fixedDeltaTimeNs)
	// 	{
	// 		this->sceneController->updateCurrentScene(NS_TO_S(fixedDeltaTimeNs));
	// 		accumulator -= fixedDeltaTimeNs;
	// 	}
	// }
// }

// void Controller::RenderLoop()
// {
	// double frameTime = 1;

	// std::vector<double> prevFps;
	// // while(this->run && this->checkSDLEventQueue())
	// {
	// 	double begin = this->timeInfo.ns();
	// 	this->renderer->Clear();

	// 	this->sceneController->renderCurrentScene(this->renderer);


	// 	if(Config::getShowFps())
	// 	{
	// 		// frames per second is (1sec to ns) / 'frametime' (in ns)
	// 		double fps = S_TO_NS(1.0) / frameTime;

	// 		// smooth fps
	// 		#if 1
	// 		{
	// 			prevFps.push_back(fps);

	// 			if(prevFps.size() > 500)
	// 				prevFps.erase(prevFps.begin());

	// 			double totalfps = 0;
	// 			for(auto f : prevFps)
	// 				totalfps += f;

	// 			totalfps /= prevFps.size();
	// 			fps = totalfps;
	// 		}
	// 		#endif


	// 		auto c = this->renderer->GetColour();
	// 		this->renderer->SetColour(Util::Colour::white());
	// 		this->renderer->RenderText(std::string("FPS: ") + (std::to_string((int) fps)), this->debugFont, Math::Vector2(10, 4));

	// 		this->renderer->SetColour(c);
	// 	}













	// 	this->renderer->Flush();

	// 	double end = this->timeInfo.ns();
	// 	frameTime = end - begin;

	// 	// don't kill the CPU
	// 	{
	// 		double toWait = targetFrameTimeNs - frameTime;
	// 		if(toWait >= 0)
	// 		{
	// 			usleep(NS_TO_US(toWait));
	// 		}
	// 		else
	// 		{
	// 			// todo: we missed our framerate.
	// 		}
	// 	}
// 	// }
// }







// void Controller::start()
// {
	// // init the game state first
	// this->sceneController = new Gravity::SceneController(*this);
	// this->sceneController->pushScene(new Gravity::CodeWindowScene(0, 0, this->window->width / 2, this->window->height));

	// // curiously, SDL's event processing *must* be done on the main thread
	// // therefore start the gameloop in a separate thread

	// // also curiously, SDL's rendering uses some thread-unsafe code from Cocoa (WHY, APPLE)
	// // therefore, all rendering must also be done in the main thread.

	// this->updateLoop = std::thread(&Controller::UpdateLoop, this);
	// this->RenderLoop();

	// this->updateLoop.join();


	// // clean up the scenes
	// for(size_t i = 0; i < this->sceneController->getSceneCount(); i++)
	// {
	// 	Gravity::Scene* s = this->sceneController->getCurrentScene();
	// 	this->sceneController->popScene();

	// 	delete s;
	// }

	// SDL_Quit();
// }

















