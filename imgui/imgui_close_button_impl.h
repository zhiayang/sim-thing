

#ifdef CUSTOMISED_SHIT

// Upper-right button to close a window.
// repurpose this to draw the traffic light situation.
static bool CloseWindowButton(bool* p_opened)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	bool didclose = false;

	const float size = window->TitleBarHeight() - 6.0f;
	ImU32 lineColour = ImGui::GetColorU32(ImVec4(0.2, 0.2, 0.2, 1.0));







	// draw the close button
	if(p_opened != nullptr)
	{
		const ImGuiID cid = window->GetID("#CLOSE");

		const ImRect bb(window->Rect().GetTL() + ImVec2(3.0f, 3.0f), window->Rect().GetTL() + ImVec2(3.0f + size, 3.0f + size));

		bool held = false;
		bool hovered = false;
		bool pressed = ImGui::ButtonBehavior(bb, cid, &hovered, &held);

		// Render
		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_CloseButtonActive :
			(hovered ? ImGuiCol_CloseButtonHovered : ImGuiCol_CloseButton));

		ImVec2 center = bb.GetCenter();
		window->DrawList->AddCircleFilled(center, ImMax(3.0f, size * 0.4f), col, 16);

		center -= ImVec2(0.5f, 0.5f);

		const float cross_extent = (size * 0.45f * 0.7071f) - 2.0f;
		if(hovered)
		{
			window->DrawList->AddLine(center + ImVec2(+cross_extent, +cross_extent),
				center + ImVec2(-cross_extent, -cross_extent), lineColour);

			window->DrawList->AddLine(center + ImVec2(+cross_extent, -cross_extent),
				center + ImVec2(-cross_extent, +cross_extent), lineColour);
		}

		if(p_opened != NULL && pressed)
			*p_opened = !*p_opened;

		didclose = pressed;
	}




	// draw the minimise button
	{
		const ImGuiID mid = window->GetID("#MINMAX");

		ImRect bb;

		// if we have a pointer, we have the close button.
		if(p_opened != nullptr)
		{
			bb = ImRect(window->Rect().GetTL() + ImVec2(5.0f + size, 3.0f), window->Rect().GetTL()
				+ ImVec2(5.0f + 2 * size, 3.0f + size));
		}
		else
		{
			// put the minmax button at the position of the would-be close button
			bb = ImRect(window->Rect().GetTL() + ImVec2(3.0f, 3.0f), window->Rect().GetTL() + ImVec2(3.0f + size, 3.0f + size));
		}


		bool held = false;
		bool hovered = false;
		bool pressed = ImGui::ButtonBehavior(bb, mid, &hovered, &held);

		// Render
		ImU32 col;
		if(window->Collapsed)
		{
			col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_MaxButtonActive :
				(hovered ? ImGuiCol_MaxButtonHovered : ImGuiCol_MaxButton));
		}
		else
		{
			col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_MinButtonActive :
				(hovered ? ImGuiCol_MinButtonHovered : ImGuiCol_MinButton));
		}

		ImVec2 center = bb.GetCenter();
		window->DrawList->AddCircleFilled(center, ImMax(3.0f, size * 0.4f), col, 16);

		center -= ImVec2(0.5f, 0.5f);

		const float cross_extent = (size * 0.45f * 0.7071f) - 2.0f;
		if(hovered)
		{
			if(window->Collapsed)
			{
				// draw a plus
				window->DrawList->AddLine(center + ImVec2(0, +cross_extent),
					center + ImVec2(0, -cross_extent), lineColour);

				window->DrawList->AddLine(center + ImVec2(+cross_extent, 0),
					center + ImVec2(-cross_extent, 0), lineColour);
			}
			else
			{
				// just a minus
				window->DrawList->AddLine(center + ImVec2(+cross_extent, 0),
					center + ImVec2(-cross_extent, 0), lineColour);
			}
		}

		if(pressed)
		{
			window->Collapsed = !window->Collapsed;
			if(!(window->Flags & ImGuiWindowFlags_NoSavedSettings))
				MarkSettingsDirty();
		}
	}


	return didclose;
}
#endif


