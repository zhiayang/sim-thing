// Terminal.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "imgui.h"
#include "graphicswrapper.h"

#include "connect/connect.h"
#include "connect/apps.h"
#include "connect/gui.h"

namespace Connect
{
	TerminalWindow::TerminalWindow()
	{
		this->tstate = new TerminalState();
		this->tstate->ref();
	}

	TerminalWindow::TerminalWindow(TerminalState* ts)
	{
		this->tstate = ts;
		this->tstate->ref();
	}


	TerminalWindow::~TerminalWindow()
	{
		this->tstate->deref();
	}

	void TerminalWindow::Render(Connect::GameState& gs, float delta, Rx::Renderer* r)
	{
		if(this->isWindowOpen)
		{
			ImGui::Begin("Terminal", &this->isWindowOpen);
			ImGui::BeginChild("Tabs", ImVec2(0, ImGui::GetItemsLineHeightWithSpacing()));

			size_t num_tabs = this->tstate->tabs.size();

			// this formula discovered through trial and error.
			// todo: do not like the '6' magic number. it's appearing everywhere.
			float availspace = ImGui::GetWindowContentRegionWidth() - ((2 * (float) num_tabs - 6)
				* ImGui::GetStyle().FramePadding.x) - (2 * ImGui::GetStyle().WindowPadding.x);

			float per_tab = availspace / (float) num_tabs;


			for(size_t i = 0; i < num_tabs; i++)
			{
				ImGui::Button(this->tstate->tabs[i].title.c_str(), ImVec2(std::max(per_tab, 0.0f), 0));
				ImGui::SameLine();
			}

			// ImGui::Spacing();
			ImGui::EndChild();

			ImGui::Separator();

			ImGui::Text("Hello, world");
			ImGui::End();
		}
		else
		{
			// must be the last thing, since we get deleted in Close().
			this->Close(gs);
		}
	}

	void TerminalWindow::Update(Connect::GameState& gs, float delta)
	{
	}
















	TerminalState::TerminalState()
	{
		TabState ts;
		ts.title = "shell";

		this->tabs.push_back(ts);
	}

	// state
	void TerminalState::deref()
	{
		// TODO(synchro): lock this maybe?
		// do we want to? idk.

		if(this->_refcount == 0)
			ERROR("cannot dereference deleted object");

		this->_refcount--;

		if(this->_refcount == 0)
			delete this;
	}

	void TerminalState::ref()
	{
		this->_refcount++;
	}
}












