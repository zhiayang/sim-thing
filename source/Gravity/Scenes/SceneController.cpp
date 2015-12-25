// SceneController.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <assert.h>

#include "gravity/gravity.h"

namespace Gravity
{
	SceneController::SceneController(Controller& c) : controller(c)
	{
	}

	void SceneController::pushScene(Scene* s)
	{
		assert(s);
		this->sceneStack.push_back(s);
	}

	void SceneController::popScene()
	{
		assert(this->sceneStack.size() > 0);
		this->sceneStack.pop_back();
	}

	Scene* SceneController::getCurrentScene()
	{
		assert(this->sceneStack.size() > 0);
		return this->sceneStack.back();
	}

	size_t SceneController::getSceneCount()
	{
		return this->sceneStack.size();
	}

	void SceneController::renderCurrentScene(SDL::Renderer* r)
	{
		this->getCurrentScene()->renderScene(this->controller, r);
	}

	void SceneController::updateCurrentScene(double delta)
	{
		this->getCurrentScene()->updateScene(this->controller, delta);
	}

	void SceneController::setCurrentScene(Scene* s)
	{
		assert(this->sceneStack.size() > 0);

		this->sceneStack.pop_back();
		this->sceneStack.push_back(s);
	}
}



























