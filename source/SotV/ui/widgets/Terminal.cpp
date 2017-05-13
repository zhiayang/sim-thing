// Terminal.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#if 0

#include "imgui.h"
#include "graphicswrapper.h"

#include "sotv/sotv.h"
#include "sotv/widgets.h"
#include "sotv/gui.h"


namespace Sotv
{
	TerminalWindow::TerminalWindow()
	{
		this->tstate = new TerminalState();
	}

	TerminalWindow::TerminalWindow(TerminalTabState* ts)
	{
		this->tstate = new TerminalState();
		this->tstate->tabs.push_back(ts);
		ts->ref();
	}


	TerminalWindow::~TerminalWindow()
	{
		for(auto tab : this->tstate->tabs)
			tab->deref();

		delete this->tstate;
	}

	void TerminalWindow::Render(Connect::GameState& gs, double delta, Rx::Renderer* r)
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
				float dx = 0;
				float dy = 0;
				if(ImGui::IsItemActive() && ImGui::IsMouseDragging())
				{
					printf("drag %zu\n", i);
					dx = ImGui::GetMouseDragDelta().x;
					dy = ImGui::GetMouseDragDelta().y;
				}

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + dx);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + dy);

				if(ImGui::Button((this->tstate->tabs[i]->title + "##" + std::to_string(i)).c_str(),
					ImVec2(std::max(per_tab, 0.0f), 0)))
				{
					printf("clicked\n");
				}

				ImGui::SameLine();
			}



			ImGui::EndChild();
			ImGui::Separator();

			ImGui::Text("Hello, world");
			ImGui::Text("FooBar");







			ImGui::End();
		}
		else
		{
			// must be the last thing, since we get deleted in Close().
			this->Close(gs);
		}
	}

	void TerminalWindow::Update(Connect::GameState& gs, double delta)
	{
	}















	namespace Widgets
	{
		TerminalState::TerminalState()
		{
			TerminalTabState* ts = new TerminalTabState();
			ts->title = "shell";

			this->tabs.push_back(ts->ref());
			this->tabs.push_back(ts->ref());
			this->tabs.push_back(ts->ref());
			this->tabs.push_back(ts->ref());
		}

		// state
		#define CMD_BUFFER_MAX_SIZE	512
		TerminalTabState::TerminalTabState()
		{
			this->_refcount = 1;
			this->curCmd = new char[CMD_BUFFER_MAX_SIZE];
		}

		TerminalTabState::~TerminalTabState()
		{
			// should only be called when refcount = 0.
			assert(this->_refcount == 0 && "cannot delete object when it is still referenced");
			delete[] this->curCmd;
		}

		void TerminalTabState::deref()
		{
			// TODO(synchro): lock this maybe?
			// do we want to? idk.

			if(this->_refcount == 0)
				ERROR("cannot dereference deleted object");

			this->_refcount--;

			if(this->_refcount == 0)
				delete this;
		}

		TerminalTabState* TerminalTabState::ref()
		{
			this->_refcount++;
			return this;
		}
	}
}



#endif







