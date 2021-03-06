// imgui_dock.h

#include "imgui.h"
#include "imgui_internal.h"

#include <new>

namespace ImGui
{
	struct DockContext
	{
		enum Slot_
		{
			Slot_Left,
			Slot_Right,
			Slot_Top,
			Slot_Bottom,
			Slot_Tab,
			Slot_Float,
			Slot_None
		};

		enum EndAction_
		{
			EndAction_None,
			EndAction_Panel,
			EndAction_End,

			EndAction_EndChild
		};
		enum Status_
		{
			Status_Docked,
			Status_Float,
			Status_Dragged
		};


		struct Dock
		{
			Dock()
				: label(nullptr)
				, id(0)
				, next_tab(nullptr)
				, prev_tab(nullptr)
				, parent(nullptr)
				, active(true)
				, pos(0, 0)
				, size(-1, -1)
				, status(Status_Float)
			{
				children[0] = children[1] = nullptr;
			}


			~Dock()
			{
				MemFree(label);
			}


			void setParent(Dock* dock)
			{
				parent = dock;
				for (Dock* tmp = prev_tab; tmp; tmp = tmp->prev_tab) tmp->parent = dock;
				for (Dock* tmp = next_tab; tmp; tmp = tmp->next_tab) tmp->parent = dock;
			}


			Dock& getSibling()
			{
				IM_ASSERT(parent);
				if (parent->children[0] == &getFirstTab()) return *parent->children[1];
				return *parent->children[0];
			}


			Dock& getFirstTab()
			{
				Dock* tmp = this;
				while (tmp->prev_tab) tmp = tmp->prev_tab;
				return *tmp;
			}


			void setActive()
			{
				active = true;
				for (Dock* tmp = prev_tab; tmp; tmp = tmp->prev_tab) tmp->active = false;
				for (Dock* tmp = next_tab; tmp; tmp = tmp->next_tab) tmp->active = false;
			}


			bool isContainer() const { return children[0] != nullptr; }


			void setChildrenPosSize(const ImVec2& _pos, const ImVec2& _size)
			{
				if (children[0]->pos.x < children[1]->pos.x)
				{
					ImVec2 s = children[0]->size;
					s.y = _size.y;
					s.x = (float)int(_size.x * children[0]->size.x / (children[0]->size.x + children[1]->size.x));
					children[0]->setPosSize(_pos, s);

					s.x = _size.x - children[0]->size.x;
					ImVec2 p = _pos;
					p.x += children[0]->size.x;
					children[1]->setPosSize(p, s);
				}
				else if (children[0]->pos.x > children[1]->pos.x)
				{
					ImVec2 s = children[1]->size;
					s.y = _size.y;
					s.x = (float)int(_size.x * children[1]->size.x / (children[0]->size.x + children[1]->size.x));
					children[1]->setPosSize(_pos, s);

					s.x = _size.x - children[1]->size.x;
					ImVec2 p = _pos;
					p.x += children[1]->size.x;
					children[0]->setPosSize(p, s);
				}
				else if (children[0]->pos.y < children[1]->pos.y)
				{
					ImVec2 s = children[0]->size;
					s.x = _size.x;
					s.y = (float)int(_size.y * children[0]->size.y / (children[0]->size.y + children[1]->size.y));
					children[0]->setPosSize(_pos, s);

					s.y = _size.y - children[0]->size.y;
					ImVec2 p = _pos;
					p.y += children[0]->size.y;
					children[1]->setPosSize(p, s);
				}
				else
				{
					ImVec2 s = children[1]->size;
					s.x = _size.x;
					s.y = (float)int(_size.y * children[1]->size.y / (children[0]->size.y + children[1]->size.y));
					children[1]->setPosSize(_pos, s);

					s.y = _size.y - children[1]->size.y;
					ImVec2 p = _pos;
					p.y += children[1]->size.y;
					children[0]->setPosSize(p, s);
				}
			}


			void setPosSize(const ImVec2& _pos, const ImVec2& _size)
			{
				size = _size;
				pos = _pos;
				for (Dock* tmp = prev_tab; tmp; tmp = tmp->prev_tab)
				{
					tmp->size = _size;
					tmp->pos = _pos;
				}
				for (Dock* tmp = next_tab; tmp; tmp = tmp->next_tab)
				{
					tmp->size = _size;
					tmp->pos = _pos;
				}

				if (!isContainer()) return;
				setChildrenPosSize(_pos, _size);
			}


			char*	label;
			ImU32	id;
			Dock*	next_tab;
			Dock*	prev_tab;
			Dock*	children[2];
			Dock*	parent;
			bool	active;
			ImVec2	pos;
			ImVec2	size;
			Status_	status;
		};


		ImVector<Dock*>		m_docks;
		ImVec2				m_drag_offset;
		Dock*				m_current = nullptr;
		int					m_last_frame = 0;
		EndAction_			m_end_action;


		~DockContext()
		{
		}


		Dock& getDock(const char* label, bool opened)
		{
			ImU32 id = ImHash(label, 0);
			for (int i = 0; i < m_docks.size(); ++i)
			{
				if (m_docks[i]->id == id) return *m_docks[i];
			}

			Dock* new_dock = (Dock*)MemAlloc(sizeof(Dock));
			new (new_dock) Dock();
			m_docks.push_back(new_dock);
			static Dock* q = nullptr; // TODO
			new_dock->label = ImStrdup(label);
			new_dock->id = id;
			new_dock->setActive();
			if (opened)
			{
				if (q)
				{
					q->prev_tab = new_dock;
					new_dock->next_tab = q;
					new_dock->setPosSize(q->pos, q->size);
				}
				new_dock->status = Status_Docked;
				q = new_dock;
			}
			else
			{
				new_dock->status = Status_Float;
			}
			return *new_dock;
		}


		void putInBackground()
		{
			ImGuiWindow* win = GetCurrentWindow();
			ImGuiState& g = *GImGui;
			if (g.Windows[0] == win) return;

			for (int i = 0; i < g.Windows.Size; i++)
			{
				if (g.Windows[i] == win)
				{
					for (int j = i - 1; j >= 0; --j)
					{
						g.Windows[j + 1] = g.Windows[j];
					}
					g.Windows[0] = win;
					break;
				}
			}
		}


		void drawSplits()
		{
			if (GetFrameCount() == m_last_frame) return;
			m_last_frame = GetFrameCount();

			putInBackground();

			ImU32 color = GetColorU32(ImGuiCol_Button);
			ImU32 color_hovered = GetColorU32(ImGuiCol_ButtonHovered);
			ImDrawList* draw_list = GetWindowDrawList();
			ImGuiIO& io = GetIO();
			for (int i = 0; i < m_docks.size(); ++i)
			{
				Dock& dock = *m_docks[i];
				if (!dock.isContainer()) continue;

				PushID(i);
				if (!IsMouseDown(0)) dock.status = Status_Docked;

				ImVec2 p0 = dock.children[0]->pos;
				ImVec2 p1 = dock.children[1]->pos;
				ImVec2 size = dock.children[0]->size;
				if (p0.x < p1.x)
				{
					SetCursorScreenPos(p1);
					InvisibleButton("split", ImVec2(3, size.y));
					if (dock.status == Status_Dragged)
					{
						dock.children[0]->size.x += io.MouseDelta.x;
						dock.children[1]->size.x -= io.MouseDelta.x;
						dock.children[1]->pos.x += io.MouseDelta.x;
					}
				}
				else if (p0.x > p1.x)
				{
					SetCursorScreenPos(p0);
					InvisibleButton("split", ImVec2(3, size.y));
					if (dock.status == Status_Dragged)
					{
						dock.children[1]->size.x += io.MouseDelta.x;
						dock.children[0]->size.x -= io.MouseDelta.x;
						dock.children[0]->pos.x += io.MouseDelta.x;
					}
				}
				else if (p0.y < p1.y)
				{
					SetCursorScreenPos(p1);
					InvisibleButton("split", ImVec2(size.x, 3));
					if (dock.status == Status_Dragged)
					{
						dock.children[0]->size.y += io.MouseDelta.y;
						dock.children[1]->size.y -= io.MouseDelta.y;
						dock.children[1]->pos.y += io.MouseDelta.y;
					}
				}
				else
				{
					SetCursorScreenPos(p0);
					InvisibleButton("split", ImVec2(size.x, 3));
					if (dock.status == Status_Dragged)
					{
						dock.children[1]->size.y += io.MouseDelta.y;
						dock.children[0]->size.y -= io.MouseDelta.y;
						dock.children[0]->pos.y += io.MouseDelta.y;
					}
				}

				if (IsItemHoveredRect() && IsMouseClicked(0))
				{
					dock.status = Status_Dragged;
				}
				if (dock.status == Status_Dragged)
				{
					dock.children[0]->setPosSize(dock.children[0]->pos, dock.children[0]->size);
					dock.children[1]->setPosSize(dock.children[1]->pos, dock.children[1]->size);
				}

				draw_list->AddRectFilled(GetItemRectMin(),
					GetItemRectMax(),
					IsItemHoveredRect() ? color_hovered : color);
				PopID();
			}
		}


		void beginPanel()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
									 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
									 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
									 ImGuiWindowFlags_NoScrollWithMouse;
			ImVec2 pos(0, GetTextLineHeightWithSpacing());
			SetNextWindowPos(pos);
			SetNextWindowSize(GetIO().DisplaySize - pos);
			Begin("###DockPanel", nullptr, flags);
			drawSplits();
		}


		void endPanel() { End(); }


		Dock* getDockAt(const ImVec2& pos) const
		{
			for (int i = 0; i < m_docks.size(); ++i)
			{
				Dock& dock = *m_docks[i];
				if (dock.isContainer()) continue;
				if (dock.status != Status_Docked) continue;
				if (IsMouseHoveringRect(dock.pos, dock.pos + dock.size, false))
				{
					return &dock;
				}
			}

			return nullptr;
		}


		static ImRect getDockedRect(const ImRect& rect, Slot_ dock_slot)
		{
			ImVec2 half_size = rect.GetSize() * 0.5f;
			switch (dock_slot)
			{
				default: return rect;
				case Slot_Top: return ImRect(rect.Min, rect.Min + ImVec2(rect.Max.x, half_size.y));
				case Slot_Right:
					return ImRect(rect.Min + ImVec2(half_size.x, 0), rect.Max);
				case Slot_Bottom:
					return ImRect(rect.Min + ImVec2(0, half_size.y), rect.Max);
				case Slot_Left: return ImRect(rect.Min, rect.Min + ImVec2(half_size.x, rect.Max.y));
			}
		}


		static ImRect getSlotRect(ImRect parent_rect, Slot_ dock_slot)
		{
			ImVec2 size = parent_rect.Max - parent_rect.Min;
			ImVec2 center = parent_rect.Min + size * 0.5f;
			switch (dock_slot)
			{
				default: return ImRect(center - ImVec2(20, 20), center + ImVec2(20, 20));
				case Slot_Top: return ImRect(center + ImVec2(-20, -50), center + ImVec2(20, -30));
				case Slot_Right: return ImRect(center + ImVec2(30, -20), center + ImVec2(50, 20));
				case Slot_Bottom: return ImRect(center + ImVec2(-20, +30), center + ImVec2(20, 50));
				case Slot_Left: return ImRect(center + ImVec2(-50, -20), center + ImVec2(-30, 20));
			}
		}


		static ImRect getSlotRectOnBorder(ImRect parent_rect, Slot_ dock_slot)
		{
			ImVec2 size = parent_rect.Max - parent_rect.Min;
			ImVec2 center = parent_rect.Min + size * 0.5f;
			switch (dock_slot)
			{
				case Slot_Top:
					return ImRect(ImVec2(center.x - 20, parent_rect.Min.y + 10),
						ImVec2(center.x + 20, parent_rect.Min.y + 30));
				case Slot_Left:
					return ImRect(ImVec2(parent_rect.Min.x + 10, center.y - 20),
						ImVec2(parent_rect.Min.x + 30, center.y + 20));
				case Slot_Bottom:
					return ImRect(ImVec2(center.x - 20, parent_rect.Max.y - 30),
						ImVec2(center.x + 20, parent_rect.Max.y - 10));
				case Slot_Right:
					return ImRect(ImVec2(parent_rect.Max.x - 30, center.y - 20),
						ImVec2(parent_rect.Max.x - 10, center.y + 20));
			}
			IM_ASSERT(false);
			return ImRect();
		}


		Dock* getRootDock()
		{
			for (int i = 0; i < m_docks.size(); ++i)
			{
				if (!m_docks[i]->parent && m_docks[i]->status == Status_Docked)
				{
					return m_docks[i];
				}
			}
			return nullptr;
		}


		bool dockSlots(Dock& dock, Dock* dest_dock, const ImRect& rect, bool on_border)
		{
			ImDrawList* canvas = GetWindowDrawList();
			ImU32 text_color = GetColorU32(ImGuiCol_Text);
			ImU32 color = GetColorU32(ImGuiCol_Button);
			ImU32 color_hovered = GetColorU32(ImGuiCol_ButtonHovered);
			ImVec2 mouse_pos = GetIO().MousePos;
			for (int i = 0; i < (on_border ? 4 : 5); ++i)
			{
				ImRect r = on_border ? getSlotRectOnBorder(rect, (Slot_)i) : getSlotRect(rect, (Slot_)i);
				bool hovered = r.Contains(mouse_pos);
				canvas->AddRectFilled(r.Min, r.Max, hovered ? color_hovered : color);
				if (!hovered) continue;

				if (!IsMouseDown(0))
				{
					doDock(dock, dest_dock ? dest_dock : getRootDock(), (Slot_)i);
					return true;
				}
				ImRect docked_rect = getDockedRect(rect, (Slot_)i);
				canvas->AddRectFilled(docked_rect.Min, docked_rect.Max, GetColorU32(ImGuiCol_TitleBg));
			}
			return false;
		}


		void handleDrag(Dock& dock)
		{
			Dock* dest_dock = getDockAt(GetIO().MousePos);

			Begin("##Overlay",
				NULL,
				ImVec2(0, 0),
				0.f,
				ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_AlwaysAutoResize);
			ImDrawList* canvas = GetWindowDrawList();

			canvas->PushClipRectFullScreen();

			ImU32 docked_color = GetColorU32(ImGuiCol_FrameBg);
			dock.pos = GetIO().MousePos - m_drag_offset;
			if (dest_dock)
			{
				if (dockSlots(dock, dest_dock, ImRect(dest_dock->pos, dest_dock->pos + dest_dock->size), false))
				{
					canvas->PopClipRect();
					End();
					return;
				}
			}
			if(dockSlots(dock, nullptr, ImRect(ImVec2(0, 0), GetIO().DisplaySize), true))
			{
				canvas->PopClipRect();
				End();
				return;
			}
			canvas->AddRectFilled(dock.pos, dock.pos + dock.size, docked_color);
			canvas->PopClipRect();

			if (!IsMouseDown(0))
			{
				dock.status = Status_Float;
				dock.setActive();
			}

			End();
		}


		void doUndock(Dock& dock)
		{
			if (dock.prev_tab) dock.prev_tab->setActive();
			else if (dock.next_tab) dock.next_tab->setActive();
			else dock.active = false;
			Dock* container = dock.parent;

			if (container)
			{
				Dock& sibling = dock.getSibling();
				if (container->children[0] == &dock)
				{
					container->children[0] = dock.next_tab;
				}
				else if (container->children[1] == &dock)
				{
					container->children[1] = dock.next_tab;
				}

				bool remove_container = !container->children[0] || !container->children[1];
				if (remove_container)
				{
					if (container->parent)
					{
						Dock*& child = container->parent->children[0] == container
							? container->parent->children[0]
							: container->parent->children[1];
						child = &sibling;
						child->setPosSize(container->pos, container->size);
						child->setParent(container->parent);
					}
					else
					{
						if (container->children[0])
						{
							container->children[0]->setParent(nullptr);
							container->children[0]->setPosSize(container->pos, container->size);
						}
						if (container->children[1])
						{
							container->children[1]->setParent(nullptr);
							container->children[1]->setPosSize(container->pos, container->size);
						}
					}
					for (int i = 0; i < m_docks.size(); ++i)
					{
						if (m_docks[i] == container)
						{
							m_docks.erase(m_docks.begin() + i);
							break;
						}
					}
					container->~Dock();
					MemFree(container);
				}
			}
			if (dock.prev_tab) dock.prev_tab->next_tab = dock.next_tab;
			if (dock.next_tab) dock.next_tab->prev_tab = dock.prev_tab;
			dock.parent = nullptr;
			dock.prev_tab = dock.next_tab = nullptr;
		}


		void drawTabbarListButton(Dock& dock)
		{
			if (!dock.next_tab) return;

			ImDrawList* draw_list = GetWindowDrawList();
			if (InvisibleButton("list", ImVec2(16, 16)))
			{
				OpenPopup("tab_list_popup");
			}
			if (BeginPopup("tab_list_popup"))
			{
				Dock* tmp = &dock;
				while (tmp)
				{
					bool dummy = false;
					if (Selectable(tmp->label, &dummy))
					{
						tmp->setActive();
					}
					tmp = tmp->next_tab;
				}
				EndPopup();
			}

			bool hovered = IsItemHovered();
			ImVec2 min = GetItemRectMin();
			ImVec2 max = GetItemRectMax();
			ImVec2 center = (min + max) * 0.5f;
			ImU32 text_color = GetColorU32(ImGuiCol_Text);
			ImU32 color_active = GetColorU32(ImGuiCol_FrameBgActive);
			draw_list->AddRectFilled(ImVec2(center.x - 4, min.y + 3),
				ImVec2(center.x + 4, min.y + 5),
				hovered ? color_active : text_color);
			draw_list->AddTriangleFilled(ImVec2(center.x - 4, min.y + 7),
				ImVec2(center.x + 4, min.y + 7),
				ImVec2(center.x, min.y + 12),
				hovered ? color_active : text_color);
		}



// Find the optional ## from which we stop displaying text.
static const char*  FindTextDisplayEnd(const char* text, const char* text_end = 0)
{
	const char* text_display_end = text;
	if (!text_end)
		text_end = (const char*)-1;

	ImGuiState& g = *GImGui;
	if (g.DisableHideTextAfterDoubleHash > 0)
	{
		while (text_display_end < text_end && *text_display_end != '\0')
			text_display_end++;
	}
	else
	{
		while (text_display_end < text_end && *text_display_end != '\0' && (text_display_end[0] != '#' || text_display_end[1] != '#'))
			text_display_end++;
	}
	return text_display_end;
}


		bool tabbar(Dock& dock, bool close_button)
		{
			float tabbar_height = 2 * GetTextLineHeightWithSpacing();
			ImVec2 size(dock.size.x, tabbar_height);
			bool tab_closed = false;

			SetCursorScreenPos(dock.pos);
			char tmp[20];
			ImFormatString(tmp, IM_ARRAYSIZE(tmp), "tabs%d", (int)dock.id);
			if (BeginChild(tmp, size, true))
			{
				Dock* dock_tab = &dock;

				ImDrawList* draw_list = GetWindowDrawList();
				ImU32 color = GetColorU32(ImGuiCol_FrameBg);
				ImU32 color_active = GetColorU32(ImGuiCol_FrameBgActive);
				ImU32 color_hovered = GetColorU32(ImGuiCol_FrameBgHovered);
				ImU32 text_color = GetColorU32(ImGuiCol_Text);
				float line_height = GetTextLineHeightWithSpacing();
				float tab_base;

				drawTabbarListButton(dock);

				while (dock_tab)
				{
					SameLine(0, 15);

					const char* text_end = FindTextDisplayEnd(dock_tab->label);
					ImVec2 size(CalcTextSize(dock_tab->label, text_end).x, line_height);
					if (InvisibleButton(dock_tab->label, size))
					{
						dock_tab->setActive();
					}

					if (IsItemActive() && IsMouseDragging())
					{
						m_drag_offset = GetMousePos() - dock_tab->pos;
						doUndock(*dock_tab);
						dock_tab->status = Status_Dragged;
					}

					bool hovered = IsItemHovered();
					ImVec2 pos = GetItemRectMin();
					if (dock_tab->active && close_button)
					{
						size.x += 16 + GetStyle().ItemSpacing.x;
						SameLine();
						tab_closed = InvisibleButton("close", ImVec2(16, 16));
						ImVec2 center = (GetItemRectMin() + GetItemRectMax()) * 0.5f;
						draw_list->AddLine(center + ImVec2(-3.5f, -3.5f), center + ImVec2( 3.5f, 3.5f), text_color);
						draw_list->AddLine(center + ImVec2( 3.5f, -3.5f), center + ImVec2(-3.5f, 3.5f), text_color);
					}
					tab_base = pos.y;
					draw_list->PathClear();
					draw_list->PathLineTo(pos + ImVec2(-15, size.y));
					draw_list->PathBezierCurveTo(
						pos + ImVec2(-10, size.y), pos + ImVec2(-5, 0), pos + ImVec2(0, 0), 10);
					draw_list->PathLineTo(pos + ImVec2(size.x, 0));
					draw_list->PathBezierCurveTo(pos + ImVec2(size.x + 5, 0),
						pos + ImVec2(size.x + 10, size.y),
						pos + ImVec2(size.x + 15, size.y),
						10);
					draw_list->PathFill(
						hovered ? color_hovered : (dock_tab->active ? color_active : color));
					draw_list->AddText(pos, text_color, dock_tab->label, text_end);

					dock_tab = dock_tab->next_tab;
				}
				ImVec2 cp(dock.pos.x, tab_base + line_height);
				draw_list->AddLine(cp, cp + ImVec2(dock.size.x, 0), color);
			}
			EndChild();
			return tab_closed;
		}


		static void setDockPosSize(Dock& dest, Dock& dock, Slot_ dock_slot, Dock& container)
		{
			IM_ASSERT(!dock.prev_tab && !dock.next_tab && !dock.children[0] && !dock.children[1]);

			dest.pos = container.pos;
			dest.size = container.size;
			dock.pos = container.pos;
			dock.size = container.size;

			switch (dock_slot)
			{
				case Slot_Bottom:
					dest.size.y *= 0.5f;
					dock.size.y *= 0.5f;
					dock.pos.y += dest.size.y;
					break;
				case Slot_Right:
					dest.size.x *= 0.5f;
					dock.size.x *= 0.5f;
					dock.pos.x += dest.size.x;
					break;
				case Slot_Left:
					dest.size.x *= 0.5f;
					dock.size.x *= 0.5f;
					dest.pos.x += dock.size.x;
					break;
				case Slot_Top:
					dest.size.y *= 0.5f;
					dock.size.y *= 0.5f;
					dest.pos.y += dock.size.y;
					break;
				default: IM_ASSERT(false); break;
			}
			dest.setPosSize(dest.pos, dest.size);
		}


		void doDock(Dock& dock, Dock* dest, Slot_ dock_slot)
		{
			IM_ASSERT(!dock.parent);
			if (!dest)
			{
				dock.status = Status_Docked;
				ImVec2 pos = ImVec2(0, GetTextLineHeightWithSpacing());
				dock.setPosSize(pos, GetIO().DisplaySize - pos);
			}
			else if (dock_slot == Slot_Tab)
			{
				Dock* tmp = dest;
				while (tmp->next_tab)
				{
					tmp = tmp->next_tab;
				}

				tmp->next_tab = &dock;
				dock.prev_tab = tmp;
				dock.size = tmp->size;
				dock.pos = tmp->pos;
				dock.parent = dest->parent;
				dock.status = Status_Docked;
			}
			else if (dock_slot == Slot_None)
			{
				dock.status = Status_Float;
			}
			else
			{
				Dock* container = (Dock*)MemAlloc(sizeof(Dock));
				new (container) Dock();
				m_docks.push_back(container);
				container->children[0] = &dest->getFirstTab();
				container->children[1] = &dock;
				container->next_tab = nullptr;
				container->prev_tab = nullptr;
				container->parent = dest->parent;
				container->size = dest->size;
				container->pos = dest->pos;
				container->status = Status_Docked;

				if (!dest->parent)
				{
				}
				else if (&dest->getFirstTab() == dest->parent->children[0])
				{
					dest->parent->children[0] = container;
				}
				else
				{
					dest->parent->children[1] = container;
				}

				dest->setParent(container);
				dock.parent = container;
				dock.status = Status_Docked;

				setDockPosSize(*dest, dock, dock_slot, *container);
			}
			dock.setActive();
		}


		bool begin(const char* label, bool* opened, ImGuiWindowFlags extra_flags)
		{
			Dock& dock = getDock(label, !opened || *opened);
			m_end_action = EndAction_None;

			if (opened && !*opened)
			{
				if (dock.status != Status_Float)
				{
					doUndock(dock);
					dock.status = Status_Float;
				}
				return false;
			}

			m_end_action = EndAction_Panel;
			beginPanel();

			m_current = &dock;
			if (dock.status == Status_Dragged) handleDrag(dock);

			bool is_float = dock.status == Status_Float;

			if (!dock.parent && dock.size.x < 0 && dock.status != Status_Dragged)
			{
				dock.pos = ImVec2(0, GetTextLineHeightWithSpacing() + 4);
				dock.size = GetIO().DisplaySize;
				dock.size.y -= dock.pos.y;
			}

			if (is_float)
			{
				SetNextWindowPos(dock.pos);
				SetNextWindowSize(dock.size);
				bool ret = Begin(
					label, opened, dock.size, -1.0f, ImGuiWindowFlags_NoCollapse | extra_flags);
				m_end_action = EndAction_End;
				dock.pos = GetWindowPos();
				dock.size = GetWindowSize();

				ImGuiState& g = *GImGui;

				if (g.ActiveId == GetCurrentWindow()->MoveID && g.IO.MouseDown[0])
				{
					m_drag_offset = GetMousePos() - dock.pos;
					doUndock(dock);
					dock.status = Status_Dragged;
				}
				return ret;
			}

			if (!dock.active && dock.status != Status_Dragged) return false;
			m_end_action = EndAction_EndChild;

			PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			float tabbar_height = GetTextLineHeightWithSpacing();
			if (tabbar(dock.getFirstTab(), opened != nullptr))
			{
				*opened = false;
			}
			ImVec2 pos = dock.pos;
			ImVec2 size = dock.size;
			pos.y += tabbar_height + GetStyle().WindowPadding.y;
			size.y -= tabbar_height + GetStyle().WindowPadding.y;

			SetCursorScreenPos(pos);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
									 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
									 ImGuiWindowFlags_NoSavedSettings | extra_flags;
			bool ret = BeginChild(label, size, true, flags);
			ImDrawList* draw_list = GetWindowDrawList();
			return ret;
		}


		void end()
		{
			if (m_end_action == EndAction_End)
			{
				End();
			}
			else if (m_end_action == EndAction_EndChild)
			{
				EndChild();
				PopStyleColor();
			}
			m_current = nullptr;
			if (m_end_action > EndAction_None) endPanel();
		}


		int getDockIndex(Dock* dock)
		{
			if (!dock) return -1;

			for (int i = 0; i < m_docks.size(); ++i)
			{
				if (dock == m_docks[i]) return i;
			}

			IM_ASSERT(false);
			return -1;
		}
	};



	static DockContext g_dock;


	void ShutdownDock()
	{
		for (int i = 0; i < g_dock.m_docks.size(); ++i)
		{
			g_dock.m_docks[i]->~Dock();
			MemFree(g_dock.m_docks[i]);
		}
	}


	bool BeginDock(const char* label, bool* opened, ImGuiWindowFlags extra_flags)
	{
		return g_dock.begin(label, opened, extra_flags);
	}


	void EndDock()
	{
		g_dock.end();
	}

} // namespace ImGui






