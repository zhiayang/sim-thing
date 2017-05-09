// System.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include <util.h>
#include <sotv/sotv.h>

namespace Sotv
{
	void System::Render(GameState& gs, float delta, Rx::Renderer* ren)
	{
		for(auto m : this->modules)
			m->Render(gs, delta, ren);
	}

	void System::Update(GameState& gs, float delta)
	{
		for(auto m : this->modules)
			m->Update(gs, delta);
	}

	void System::addModule(Module* mod)
	{
		assert(mod);
		if(std::find(this->modules.begin(), this->modules.end(), mod) != this->modules.end())
			ERROR("Module (ptr '%p') already exists in the system, skipping", mod);

		else
			this->modules.push_back(mod);
	}
}
