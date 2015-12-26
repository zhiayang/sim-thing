// textedit_impl.cpp

// #include "imgui.h"

// #define IMGUI_DEFINE_MATH_OPERATORS
// #include "imgui_internal.h"

// #include <ctype.h>      // toupper, isprint


// // Clang warnings with -Weverything
// #ifdef __clang__
// #pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
// #pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
// #pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
// #pragma clang diagnostic ignored "-Wexit-time-destructors"  // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. ImGui coding style welcomes static/globals.
// #pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference it.
// #pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
// #pragma clang diagnostic ignored "-Wmissing-noreturn"       // warning : function xx could be declared with attribute 'noreturn' warning    // GetDefaultFontData() asserts which some implementation makes it never return.
// #pragma clang diagnostic ignored "-Wdeprecated-declarations"// warning : 'xx' is deprecated: The POSIX name for this item.. // for strdup used in demo code (so user can copy & paste the code)
// #pragma clang diagnostic ignored "-Wint-to-void-pointer-cast" // warning : cast to 'void *' from smaller integer type 'int'
// #endif
// #ifdef __GNUC__
// #pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
// #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
// #endif






// // todo: copied.
// // find neater solution to this.
// // need to remember to unstatic these in imgui.cpp too.
// void LogRenderedText(const ImVec2& ref_pos, const char* text, const char* text_end = NULL);
// int InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end);
// ImVec2 InputTextCalcTextSizeW(const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false);

// bool InputTextFilterCharacter(unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data);
// bool IsKeyPressedMap(ImGuiKey key, bool repeat = true);
// void SetWindowScrollY(ImGuiWindow* window, float new_scroll_y);





// // Wrapper for stb_textedit.h to edit text (our wrapper is for: statically sized buffer, single-line, wchar characters. InputText converts between UTF-8 and wchar)
// namespace ImGuiStb
// {

// static int     STB_TEXTEDIT_STRINGLEN(const STB_TEXTEDIT_STRING* obj)                             { return obj->CurLenW; }
// static ImWchar STB_TEXTEDIT_GETCHAR(const STB_TEXTEDIT_STRING* obj, int idx)                      { return obj->Text[idx]; }
// static float   STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING* obj, int line_start_idx, int char_idx)  { ImWchar c = obj->Text[line_start_idx+char_idx]; if(c == '\n') return STB_TEXTEDIT_GETWIDTH_NEWLINE; return GImGui->Font->GetCharAdvance(c) * (GImGui->FontSize / GImGui->Font->FontSize); }
// static int     STB_TEXTEDIT_KEYTOTEXT(int key)                                                    { return key >= 0x10000 ? 0 : key; }
// static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
// static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, STB_TEXTEDIT_STRING* obj, int line_start_idx)
// {
// 	const ImWchar* text = obj->Text.Data;
// 	const ImWchar* text_remaining = NULL;
// 	const ImVec2 size = InputTextCalcTextSizeW(text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
// 	r->x0 = 0.0f;
// 	r->x1 = size.x;
// 	r->baseline_y_delta = size.y;
// 	r->ymin = 0.0f;
// 	r->ymax = size.y;
// 	r->num_chars = (int)(text_remaining - (text + line_start_idx));
// }

// static bool is_separator(unsigned int c)                                                          { return c==',' || c==';' || c=='(' || c==')' || c=='{' || c=='}' || c=='[' || c==']' || c=='|'; }
// #define STB_TEXTEDIT_IS_SPACE(CH)                                                                 ( ImCharIsSpace((unsigned int)CH) || is_separator((unsigned int)CH) )
// static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING* obj, int pos, int n)
// {
// 	ImWchar* dst = obj->Text.Data + pos;

// 	// We maintain our buffer length in both UTF-8 and wchar formats
// 	obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
// 	obj->CurLenW -= n;

// 	// Offset remaining text
// 	const ImWchar* src = obj->Text.Data + pos + n;
// 	while(ImWchar c = *src++)
// 		*dst++ = c;
// 	*dst = '\0';
// }

// static bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING* obj, int pos, const ImWchar* new_text, int new_text_len)
// {
// 	const int text_len = obj->CurLenW;
// 	if(new_text_len + text_len + 1 > obj->Text.Size)
// 		return false;

// 	const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
// 	if(new_text_len_utf8 + obj->CurLenA + 1 > obj->BufSizeA)
// 		return false;

// 	ImWchar* text = obj->Text.Data;
// 	if(pos != text_len)
// 		memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
// 	memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

// 	obj->CurLenW += new_text_len;
// 	obj->CurLenA += new_text_len_utf8;
// 	obj->Text[obj->CurLenW] = '\0';

// 	return true;
// }

// // We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
// #define STB_TEXTEDIT_K_LEFT         0x10000 // keyboard input to move cursor left
// #define STB_TEXTEDIT_K_RIGHT        0x10001 // keyboard input to move cursor right
// #define STB_TEXTEDIT_K_UP           0x10002 // keyboard input to move cursor up
// #define STB_TEXTEDIT_K_DOWN         0x10003 // keyboard input to move cursor down
// #define STB_TEXTEDIT_K_LINESTART    0x10004 // keyboard input to move cursor to start of line
// #define STB_TEXTEDIT_K_LINEEND      0x10005 // keyboard input to move cursor to end of line
// #define STB_TEXTEDIT_K_TEXTSTART    0x10006 // keyboard input to move cursor to start of text
// #define STB_TEXTEDIT_K_TEXTEND      0x10007 // keyboard input to move cursor to end of text
// #define STB_TEXTEDIT_K_DELETE       0x10008 // keyboard input to delete selection or character under cursor
// #define STB_TEXTEDIT_K_BACKSPACE    0x10009 // keyboard input to delete selection or character left of cursor
// #define STB_TEXTEDIT_K_UNDO         0x1000A // keyboard input to perform undo
// #define STB_TEXTEDIT_K_REDO         0x1000B // keyboard input to perform redo
// #define STB_TEXTEDIT_K_WORDLEFT     0x1000C // keyboard input to move cursor left one word
// #define STB_TEXTEDIT_K_WORDRIGHT    0x1000D // keyboard input to move cursor right one word
// #define STB_TEXTEDIT_K_SHIFT        0x20000

// #define STB_TEXTEDIT_IMPLEMENTATION
// #include "stb_textedit.h"

// }




// // todo: copied.
// namespace ImGui
// {
// 	void SetActiveID(ImGuiID id, ImGuiWindow* window = NULL);
// }












// Edit a string of text
// FIXME: Rather messy function partly because we are doing UTF8 > u16 > UTF8 conversions on the go to more easily handle stb_textedit calls. Ideally we should stay in UTF-8 all the time. See https://github.com/nothings/stb/issues/188
bool ImGui::InputTextEx(const char* label, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
{
	ImGuiWindow* window = GetCurrentWindow();
	if(window->SkipItems)
		return false;

	IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline))); // Can't use both together (they both use up/down keys)
	IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

	ImGuiState& g = *GImGui;
	const ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	const ImGuiID id = window->GetID(label);
	const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;
	const bool is_editable = (flags & ImGuiInputTextFlags_ReadOnly) == 0;
	const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;

	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), is_multiline ? ImGui::GetTextLineHeight() * 8.0f : label_size.y); // Arbitrary default of 8 lines high for multi-line
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size + style.FramePadding*2.0f);
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? (style.ItemInnerSpacing.x + label_size.x) : 0.0f, 0.0f));

	ImGuiWindow* draw_window = window;
	if(is_multiline)
	{
		ImGui::BeginGroup();
		if(!ImGui::BeginChildFrame(id, frame_bb.GetSize()))
		{
			ImGui::EndChildFrame();
			ImGui::EndGroup();
			return false;
		}
		draw_window = GetCurrentWindow();
		draw_window->DC.CursorPos += style.FramePadding;
		size.x -= draw_window->ScrollbarSizes.x;
	}
	else
	{
		ItemSize(total_bb, style.FramePadding.y);
		if(!ItemAdd(total_bb, &id))
			return false;
	}

	// Password pushes a temporary font with only a fallback glyph
	if(is_password)
	{
		const ImFont::Glyph* glyph = g.Font->FindGlyph('*');
		ImFont* password_font = &g.InputTextPasswordFont;
		password_font->FontSize = g.Font->FontSize;
		password_font->Scale = g.Font->Scale;
		password_font->DisplayOffset = g.Font->DisplayOffset;
		password_font->Ascent = g.Font->Ascent;
		password_font->Descent = g.Font->Descent;
		password_font->ContainerAtlas = g.Font->ContainerAtlas;
		password_font->FallbackGlyph = glyph;
		password_font->FallbackXAdvance = glyph->XAdvance;
		IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexXAdvance.empty() && password_font->IndexLookup.empty());
		ImGui::PushFont(password_font);
	}

	// NB: we are only allowed to access 'edit_state' if we are the active widget.
	ImGuiTextEditState& edit_state = g.InputTextState;

	const bool is_ctrl_down = io.KeyCtrl;
	const bool is_shift_down = io.KeyShift;
	const bool is_alt_down = io.KeyAlt;


	#if ENABLE_OSX_TEXT_EDITING
		const bool is_cmd_down = io.KeysDown[ImGuiKey_LeftCmd] || io.KeysDown[ImGuiKey_RightCmd];
	#endif


	const bool focus_requested = FocusableItemRegister(window, g.ActiveId == id, (flags & (ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_AllowTabInput)) == 0);    // Using completion callback disable keyboard tabbing
	const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
	const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

	const bool hovered = IsHovered(frame_bb, id);
	if(hovered)
	{
		SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}
	const bool user_clicked = hovered && io.MouseClicked[0];
	const bool user_scrolled = is_multiline && g.ActiveId == 0 && edit_state.Id == id && g.ActiveIdPreviousFrame == draw_window->GetID("#SCROLLY");

	bool select_all = (g.ActiveId != id) && (flags & ImGuiInputTextFlags_AutoSelectAll) != 0;
	if(focus_requested || user_clicked || user_scrolled)
	{
		if(g.ActiveId != id)
		{
			// Start edition
			// Take a copy of the initial buffer value (both in original UTF-8 format and converted to wchar)
			// From the moment we focused we are ignoring the content of 'buf' (unless we are in read-only mode)
			const int prev_len_w = edit_state.CurLenW;
			edit_state.Text.resize(buf_size+1);        // wchar count <= utf-8 count. we use +1 to make sure that .Data isn't NULL so it doesn't crash.
			edit_state.InitialText.resize(buf_size+1); // utf-8. we use +1 to make sure that .Data isn't NULL so it doesn't crash.
			ImFormatString(edit_state.InitialText.Data, edit_state.InitialText.Size, "%s", buf);
			const char* buf_end = NULL;
			edit_state.CurLenW = ImTextStrFromUtf8(edit_state.Text.Data, edit_state.Text.Size, buf, NULL, &buf_end);
			edit_state.CurLenA = (int)(buf_end - buf); // We can't get the result from ImFormatString() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.
			edit_state.CursorAnimReset();

			// Preserve cursor position and undo/redo stack if we come back to same widget
			// FIXME: We should probably compare the whole buffer to be on the safety side. Comparing buf (utf8) and edit_state.Text (wchar).
			const bool recycle_state = (edit_state.Id == id) && (prev_len_w == edit_state.CurLenW);
			if(recycle_state)
			{
				// Recycle existing cursor/selection/undo stack but clamp position
				// Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
				edit_state.CursorClamp();
			}
			else
			{
				edit_state.Id = id;
				edit_state.ScrollX = 0.0f;
				stb_textedit_initialize_state(&edit_state.StbState, !is_multiline);
				if(!is_multiline && focus_requested_by_code)
					select_all = true;
			}
			if(flags & ImGuiInputTextFlags_AlwaysInsertMode)
				edit_state.StbState.insert_mode = true;
			if(!is_multiline && (focus_requested_by_tab || (user_clicked && is_ctrl_down)))
				select_all = true;
		}
		SetActiveID(id, window);
		FocusWindow(window);
	}
	else if(io.MouseClicked[0])
	{
		// Release focus when we click outside
		if(g.ActiveId == id)
			SetActiveID(0);
	}

	bool value_changed = false;
	bool cancel_edit = false;
	bool enter_pressed = false;

	if(g.ActiveId == id)
	{
		if(!is_editable && !g.ActiveIdIsJustActivated)
		{
			// When read-only we always use the live data passed to the function
			edit_state.Text.resize(buf_size+1);
			const char* buf_end = NULL;
			edit_state.CurLenW = ImTextStrFromUtf8(edit_state.Text.Data, edit_state.Text.Size, buf, NULL, &buf_end);
			edit_state.CurLenA = (int)(buf_end - buf);
			edit_state.CursorClamp();
		}

		edit_state.BufSizeA = buf_size;

		// Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
		// Down the line we should have a cleaner library-wide concept of Selected vs Active.
		g.ActiveIdAllowOverlap = !io.MouseDown[0];

		// Edit in progress
		const float mouse_x = (g.IO.MousePos.x - frame_bb.Min.x - style.FramePadding.x) + edit_state.ScrollX;
		const float mouse_y = (is_multiline ? (g.IO.MousePos.y - draw_window->DC.CursorPos.y - style.FramePadding.y) : (g.FontSize*0.5f));

		if(select_all || (hovered && io.MouseDoubleClicked[0]))
		{
			edit_state.SelectAll();
			edit_state.SelectedAllMouseLock = true;
		}
		else if(io.MouseClicked[0] && !edit_state.SelectedAllMouseLock)
		{
			stb_textedit_click(&edit_state, &edit_state.StbState, mouse_x, mouse_y);
			edit_state.CursorAnimReset();
		}
		else if(io.MouseDown[0] && !edit_state.SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
		{
			stb_textedit_drag(&edit_state, &edit_state.StbState, mouse_x, mouse_y);
			edit_state.CursorAnimReset();
			edit_state.CursorFollow = true;
		}
		if(edit_state.SelectedAllMouseLock && !io.MouseDown[0])
			edit_state.SelectedAllMouseLock = false;

		if(g.IO.InputCharacters[0])
		{
			// Process text input (before we check for Return because using some IME will effectively send a Return?)
			// We ignore CTRL inputs, but need to allow CTRL+ALT as some keyboards (e.g. German) use AltGR - which is Alt+Ctrl - to input certain characters.
			if(!(is_ctrl_down && !is_alt_down) && is_editable)
			{
				for(int n = 0; n < IM_ARRAYSIZE(g.IO.InputCharacters) && g.IO.InputCharacters[n]; n++)
					if(unsigned int c = (unsigned int)g.IO.InputCharacters[n])
					{
						// Insert character if they pass filtering
						if(!InputTextFilterCharacter(&c, flags, callback, user_data))
							continue;
						edit_state.OnKeyPressed((int)c);
					}
			}

			// Consume characters
			memset(g.IO.InputCharacters, 0, sizeof(g.IO.InputCharacters));
		}

		// Handle various key-presses
		const int k_mask = (is_shift_down ? STB_TEXTEDIT_K_SHIFT : 0);
		const bool is_ctrl_only = is_ctrl_down && !is_alt_down && !is_shift_down;

		bool shortcut_modifier_down = is_ctrl_only;

		#if ENABLE_OSX_TEXT_EDITING
			const bool is_cmd_only = is_cmd_down && !is_ctrl_down && !is_alt_down && !is_shift_down;
			const bool is_alt_only = is_alt_down && !is_ctrl_down && !is_cmd_down && !is_shift_down;

			shortcut_modifier_down = is_cmd_only;
		#endif



		if(IsKeyPressedMap(ImGuiKey_LeftArrow))
		{
			#if ENABLE_OSX_TEXT_EDITING
				edit_state.OnKeyPressed(is_alt_only ? STB_TEXTEDIT_K_WORDLEFT | k_mask : STB_TEXTEDIT_K_LEFT | k_mask);
			#else
				edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_WORDLEFT | k_mask : STB_TEXTEDIT_K_LEFT | k_mask);
			#endif
		}
		else if(IsKeyPressedMap(ImGuiKey_RightArrow))
		{
			#if ENABLE_OSX_TEXT_EDITING
				edit_state.OnKeyPressed(is_alt_only ? STB_TEXTEDIT_K_WORDRIGHT | k_mask : STB_TEXTEDIT_K_RIGHT | k_mask);
			#else
				edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_WORDRIGHT | k_mask : STB_TEXTEDIT_K_RIGHT | k_mask);
			#endif
		}
		else if(is_multiline && IsKeyPressedMap(ImGuiKey_UpArrow))
		{
			if(is_ctrl_down)
				SetWindowScrollY(draw_window, draw_window->Scroll.y - g.FontSize);

			else
				edit_state.OnKeyPressed(STB_TEXTEDIT_K_UP | k_mask);
		}
		else if(is_multiline && IsKeyPressedMap(ImGuiKey_DownArrow))
		{
			if(is_ctrl_down)
				SetWindowScrollY(draw_window, draw_window->Scroll.y + g.FontSize);

			else
				edit_state.OnKeyPressed(STB_TEXTEDIT_K_DOWN| k_mask);
		}
		else if(IsKeyPressedMap(ImGuiKey_Home))
		{
			edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask);
		}
		else if(IsKeyPressedMap(ImGuiKey_End))
		{
			edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask);
		}
		else if(IsKeyPressedMap(ImGuiKey_Delete) && is_editable)
		{
			edit_state.OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask);
		}
		else if(IsKeyPressedMap(ImGuiKey_Backspace) && is_editable)
		{
			edit_state.OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
		}
		else if(IsKeyPressedMap(ImGuiKey_Enter))
		{
			bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
			if(!is_multiline || (ctrl_enter_for_new_line && !is_ctrl_down) || (!ctrl_enter_for_new_line && is_ctrl_down))
			{
				SetActiveID(0);
				enter_pressed = true;
			}
			else if(is_editable)
			{
				unsigned int c = '\n'; // Insert new line
				if(InputTextFilterCharacter(&c, flags, callback, user_data))
					edit_state.OnKeyPressed((int)c);
			}
		}
		else if((flags & ImGuiInputTextFlags_AllowTabInput) && IsKeyPressedMap(ImGuiKey_Tab) && !is_ctrl_down && !is_shift_down && !is_alt_down && is_editable)
		{
			unsigned int c = '\t'; // Insert TAB
			if(InputTextFilterCharacter(&c, flags, callback, user_data))
				edit_state.OnKeyPressed((int)c);
		}
		else if(IsKeyPressedMap(ImGuiKey_Escape))
		{
			SetActiveID(0); cancel_edit = true;
		}
		else if(shortcut_modifier_down && IsKeyPressedMap(ImGuiKey_Z) && is_editable)
		{
			edit_state.OnKeyPressed(STB_TEXTEDIT_K_UNDO); edit_state.ClearSelection();
		}
		else if(shortcut_modifier_down && IsKeyPressedMap(ImGuiKey_Y) && is_editable)
		{
			edit_state.OnKeyPressed(STB_TEXTEDIT_K_REDO); edit_state.ClearSelection();
		}
		else if(shortcut_modifier_down && IsKeyPressedMap(ImGuiKey_A))
		{
			edit_state.SelectAll(); edit_state.CursorFollow = true;
		}
		else if(shortcut_modifier_down && !is_password && ((IsKeyPressedMap(ImGuiKey_X) && is_editable) || IsKeyPressedMap(ImGuiKey_C))
			&& (!is_multiline || edit_state.HasSelection()))
		{
			// Cut, Copy
			const bool cut = IsKeyPressedMap(ImGuiKey_X);
			if(cut && !edit_state.HasSelection())
				edit_state.SelectAll();

			if(g.IO.SetClipboardTextFn)
			{
				const int ib = edit_state.HasSelection() ? ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end) : 0;
				const int ie = edit_state.HasSelection() ? ImMax(edit_state.StbState.select_start, edit_state.StbState.select_end) : edit_state.CurLenW;
				edit_state.TempTextBuffer.resize((ie-ib) * 4 + 1);
				ImTextStrToUtf8(edit_state.TempTextBuffer.Data, edit_state.TempTextBuffer.Size, edit_state.Text.Data+ib, edit_state.Text.Data+ie);
				g.IO.SetClipboardTextFn(edit_state.TempTextBuffer.Data);
			}

			if(cut)
			{
				edit_state.CursorFollow = true;
				stb_textedit_cut(&edit_state, &edit_state.StbState);
			}
		}
		else if(shortcut_modifier_down && IsKeyPressedMap(ImGuiKey_V) && is_editable)
		{
			// Paste
			if(g.IO.GetClipboardTextFn)
			{
				if(const char* clipboard = g.IO.GetClipboardTextFn())
				{
					// Remove new-line from pasted buffer
					const int clipboard_len = (int)strlen(clipboard);
					ImWchar* clipboard_filtered = (ImWchar*)ImGui::MemAlloc((clipboard_len+1) * sizeof(ImWchar));
					int clipboard_filtered_len = 0;
					for(const char* s = clipboard; *s; )
					{
						unsigned int c;
						s += ImTextCharFromUtf8(&c, s, NULL);
						if(c == 0)
							break;
						if(c >= 0x10000 || !InputTextFilterCharacter(&c, flags, callback, user_data))
							continue;
						clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
					}
					clipboard_filtered[clipboard_filtered_len] = 0;
					if(clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
					{
						stb_textedit_paste(&edit_state, &edit_state.StbState, clipboard_filtered, clipboard_filtered_len);
						edit_state.CursorFollow = true;
					}
					ImGui::MemFree(clipboard_filtered);
				}
			}
		}

		if(cancel_edit)
		{
			// Restore initial value
			if(is_editable)
			{
				ImFormatString(buf, buf_size, "%s", edit_state.InitialText.Data);
				value_changed = true;
			}
		}
		else
		{
			// Apply new value immediately - copy modified buffer back
			// Note that as soon as the input box is active, the in-widget value gets priority over any underlying modification of the input buffer
			// FIXME: We actually always render 'buf' when calling DrawList->AddText, making the comment above incorrect.
			// FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks.
			if(is_editable)
			{
				edit_state.TempTextBuffer.resize(edit_state.Text.Size * 4);
				ImTextStrToUtf8(edit_state.TempTextBuffer.Data, edit_state.TempTextBuffer.Size, edit_state.Text.Data, NULL);
			}

			// User callback
			if((flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackAlways)) != 0)
			{
				IM_ASSERT(callback != NULL);

				// The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
				ImGuiInputTextFlags event_flag = 0;
				ImGuiKey event_key = ImGuiKey_COUNT;
				if((flags & ImGuiInputTextFlags_CallbackCompletion) != 0 && IsKeyPressedMap(ImGuiKey_Tab))
				{
					event_flag = ImGuiInputTextFlags_CallbackCompletion;
					event_key = ImGuiKey_Tab;
				}
				else if((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_UpArrow))
				{
					event_flag = ImGuiInputTextFlags_CallbackHistory;
					event_key = ImGuiKey_UpArrow;
				}
				else if((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_DownArrow))
				{
					event_flag = ImGuiInputTextFlags_CallbackHistory;
					event_key = ImGuiKey_DownArrow;
				}

				if(event_key != ImGuiKey_COUNT || (flags & ImGuiInputTextFlags_CallbackAlways) != 0)
				{
					ImGuiTextEditCallbackData callback_data;
					callback_data.EventFlag = event_flag;
					callback_data.Flags = flags;
					callback_data.UserData = user_data;
					callback_data.ReadOnly = !is_editable;

					callback_data.EventKey = event_key;
					callback_data.Buf = edit_state.TempTextBuffer.Data;
					callback_data.BufSize = edit_state.BufSizeA;
					callback_data.BufDirty = false;

					// We have to convert from position from wchar to UTF-8 positions
					ImWchar* text = edit_state.Text.Data;
					const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.cursor);
					const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.select_start);
					const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.select_end);

					// Call user code
					callback(&callback_data);

					// Read back what user may have modified
					IM_ASSERT(callback_data.Buf == edit_state.TempTextBuffer.Data);  // Invalid to modify those fields
					IM_ASSERT(callback_data.BufSize == edit_state.BufSizeA);
					IM_ASSERT(callback_data.Flags == flags);
					if(callback_data.CursorPos != utf8_cursor_pos)            edit_state.StbState.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos);
					if(callback_data.SelectionStart != utf8_selection_start)  edit_state.StbState.select_start = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart);
					if(callback_data.SelectionEnd != utf8_selection_end)      edit_state.StbState.select_end = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd);
					if(callback_data.BufDirty)
					{
						edit_state.CurLenW = ImTextStrFromUtf8(text, edit_state.Text.Size, edit_state.TempTextBuffer.Data, NULL);
						edit_state.CurLenA = (int)strlen(edit_state.TempTextBuffer.Data);
						edit_state.CursorAnimReset();
					}
				}
			}

			// Copy back to user buffer
			if(is_editable && strcmp(edit_state.TempTextBuffer.Data, buf) != 0)
			{
				ImFormatString(buf, buf_size, "%s", edit_state.TempTextBuffer.Data);
				value_changed = true;
			}
		}
	}

	if(!is_multiline)
		RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

	// Render
	const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x + style.FramePadding.x*2.0f, frame_bb.Min.y + size.y + style.FramePadding.y*2.0f);
	ImVec2 render_pos = is_multiline ? draw_window->DC.CursorPos : frame_bb.Min + style.FramePadding;
	ImVec2 text_size(0.f, 0.f);
	if(g.ActiveId == id || (edit_state.Id == id && is_multiline && g.ActiveId == draw_window->GetID("#SCROLLY")))
	{
		edit_state.CursorAnim += g.IO.DeltaTime;

		// We need to:
		// - Display the text (this can be more easily clipped)
		// - Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
		// - Measure text height (for scrollbar)
		// We are attempting to do most of that in **one main pass** to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
		const ImWchar* text_begin = edit_state.Text.Data;
		ImVec2 cursor_offset, select_start_offset;

		{
			// Count lines + find lines numbers straddling 'cursor' and 'select_start' position.
			const ImWchar* searches_input_ptr[2];
			searches_input_ptr[0] = text_begin + edit_state.StbState.cursor;
			searches_input_ptr[1] = NULL;
			int searches_remaining = 1;
			int searches_result_line_number[2] = { -1, -999 };
			if(edit_state.StbState.select_start != edit_state.StbState.select_end)
			{
				searches_input_ptr[1] = text_begin + ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end);
				searches_result_line_number[1] = -1;
				searches_remaining++;
			}

			// Iterate all lines to find our line numbers
			// In multi-line mode, we never exit the loop until all lines are counted, so add one extra to the searches_remaining counter.
			searches_remaining += is_multiline ? 1 : 0;
			int line_count = 0;
			for(const ImWchar* s = text_begin; *s != 0; s++)
				if(*s == '\n')
				{
					line_count++;
					if(searches_result_line_number[0] == -1 && s >= searches_input_ptr[0]) { searches_result_line_number[0] = line_count; if(--searches_remaining <= 0) break; }
					if(searches_result_line_number[1] == -1 && s >= searches_input_ptr[1]) { searches_result_line_number[1] = line_count; if(--searches_remaining <= 0) break; }
				}
			line_count++;
			if(searches_result_line_number[0] == -1) searches_result_line_number[0] = line_count;
			if(searches_result_line_number[1] == -1) searches_result_line_number[1] = line_count;

			// Calculate 2d position by finding the beginning of the line and measuring distance
			cursor_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[0], text_begin), searches_input_ptr[0]).x;
			cursor_offset.y = searches_result_line_number[0] * g.FontSize;
			if(searches_result_line_number[1] >= 0)
			{
				select_start_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[1], text_begin), searches_input_ptr[1]).x;
				select_start_offset.y = searches_result_line_number[1] * g.FontSize;
			}

			// Calculate text height
			if(is_multiline)
				text_size = ImVec2(size.x, line_count * g.FontSize);
		}

		// Scroll
		if(edit_state.CursorFollow)
		{
			// Horizontal scroll in chunks of quarter width
			if(!(flags & ImGuiInputTextFlags_NoHorizontalScroll))
			{
				const float scroll_increment_x = size.x * 0.25f;
				if(cursor_offset.x < edit_state.ScrollX)
					edit_state.ScrollX = (float)(int)ImMax(0.0f, cursor_offset.x - scroll_increment_x);
				else if(cursor_offset.x - size.x >= edit_state.ScrollX)
					edit_state.ScrollX = (float)(int)(cursor_offset.x - size.x + scroll_increment_x);
			}
			else
			{
				edit_state.ScrollX = 0.0f;
			}

			// Vertical scroll
			if(is_multiline)
			{
				float scroll_y = draw_window->Scroll.y;
				if(cursor_offset.y - g.FontSize < scroll_y)
					scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
				else if(cursor_offset.y - size.y >= scroll_y)
					scroll_y = cursor_offset.y - size.y;
				draw_window->DC.CursorPos.y += (draw_window->Scroll.y - scroll_y);   // To avoid a frame of lag
				draw_window->Scroll.y = scroll_y;
				render_pos.y = draw_window->DC.CursorPos.y;
			}
		}
		edit_state.CursorFollow = false;
		const ImVec2 render_scroll = ImVec2(edit_state.ScrollX, 0.0f);

		// Draw selection
		if(edit_state.StbState.select_start != edit_state.StbState.select_end)
		{
			const ImWchar* text_selected_begin = text_begin + ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end);
			const ImWchar* text_selected_end = text_begin + ImMax(edit_state.StbState.select_start, edit_state.StbState.select_end);

			float bg_offy_up = is_multiline ? 0.0f : -1.0f;    // FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
			float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
			ImU32 bg_color = GetColorU32(ImGuiCol_TextSelectedBg);
			ImVec2 rect_pos = render_pos + select_start_offset - render_scroll;
			for(const ImWchar* p = text_selected_begin; p < text_selected_end; )
			{
				if(rect_pos.y > clip_rect.w + g.FontSize)
					break;
				if(rect_pos.y < clip_rect.y)
				{
					while(p < text_selected_end)
						if(*p++ == '\n')
							break;
				}
				else
				{
					ImVec2 rect_size = InputTextCalcTextSizeW(p, text_selected_end, &p, NULL, true);
					if(rect_size.x <= 0.0f) rect_size.x = (float)(int)(g.Font->GetCharAdvance((unsigned short)' ') * 0.50f); // So we can see selected empty lines
					ImRect rect(rect_pos + ImVec2(0.0f, bg_offy_up - g.FontSize), rect_pos +ImVec2(rect_size.x, bg_offy_dn));
					rect.Clip(clip_rect);
					if(rect.Overlaps(clip_rect))
						draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, bg_color);
				}
				rect_pos.x = render_pos.x - render_scroll.x;
				rect_pos.y += g.FontSize;
			}
		}

		draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos - render_scroll, GetColorU32(ImGuiCol_Text), buf, buf+edit_state.CurLenA, 0.0f, is_multiline ? NULL : &clip_rect);

		// Draw blinking cursor
		ImVec2 cursor_screen_pos = render_pos + cursor_offset - render_scroll;
		bool cursor_is_visible = (g.InputTextState.CursorAnim <= 0.0f) || fmodf(g.InputTextState.CursorAnim, 1.20f) <= 0.80f;
		if(cursor_is_visible)
			draw_window->DrawList->AddLine(cursor_screen_pos + ImVec2(0.0f,-g.FontSize+0.5f), cursor_screen_pos + ImVec2(0.0f,-1.5f), GetColorU32(ImGuiCol_Text));

		// Notify OS of text input position for advanced IME (-1 x offset so that Windows IME can cover our cursor. Bit of an extra nicety.)
		if(is_editable)
			g.OsImePosRequest = ImVec2(cursor_screen_pos.x - 1, cursor_screen_pos.y - g.FontSize);
	}
	else
	{
		// Render text only
		const char* buf_end = NULL;
		if(is_multiline)
			text_size = ImVec2(size.x, InputTextCalcTextLenAndLineCount(buf, &buf_end) * g.FontSize); // We don't need width
		draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos, GetColorU32(ImGuiCol_Text), buf, buf_end, 0.0f, is_multiline ? NULL : &clip_rect);
	}

	if(is_multiline)
	{
		ImGui::Dummy(text_size + ImVec2(0.0f, g.FontSize)); // Always add room to scroll an extra line
		ImGui::EndChildFrame();
		ImGui::EndGroup();
	}

	if(is_password)
		ImGui::PopFont();

	// Log as text
	if(g.LogEnabled && !is_password)
		LogRenderedText(render_pos, buf, NULL);

	if(label_size.x > 0)
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

	if((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
		return enter_pressed;
	else
		return value_changed;
}









