// scene.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <deque>

struct Controller;

namespace Rx { struct Renderer; }
namespace Gravity
{
	struct Scene
	{
		virtual ~Scene() { }
		virtual void renderScene(Controller& c, Rx::Renderer* r) = 0;
		virtual void updateScene(Controller& c, double delta) = 0;
		virtual bool hidesBehind() = 0;
	};




	// scenes behave kind of like windows, actually.
	// little widget things. but scenes themselves contain a world of entities and stuff.
	struct CodeWindowScene : Scene
	{
		virtual void renderScene(Controller& c, Rx::Renderer* r) override;
		virtual void updateScene(Controller& c, double delta) override;
		virtual bool hidesBehind() override;

		CodeWindowScene(size_t x, size_t y, size_t w, size_t h);

		size_t xpos;
		size_t ypos;
		size_t width;
		size_t height;
	};






































































	struct SceneController
	{
		SceneController(Controller& c);

		void pushScene(Scene* s);
		void popScene();

		Scene* getCurrentScene();
		size_t getSceneCount();


		void renderCurrentScene(Rx::Renderer* r);
		void updateCurrentScene(double delta);


		/// replaces the top of the stack
		void setCurrentScene(Scene* s);


		private:
		Controller& controller;
		std::deque<Scene*> sceneStack;
	};
}








