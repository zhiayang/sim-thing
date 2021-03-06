// ImGui SDL2 binding with OpenGL
// You can copy and use unmodified imgui_impl_* files in your project.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// See main.cpp for an example of using this.
// https://github.com/ocornut/imgui

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <glbinding/gl/gl.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"

#include "backend_abstraction.h"

// custom stuff.
#include "rx.h"

using namespace gl;


// Data
static double	g_Time = 0.0f;
static bool		g_MousePressed[3] = { false, false, false };

static float	g_MouseWheelV = 0.0f;
static float	g_MouseWheelH = 0.0f;

static GLuint	g_FontTexture = 0;







static const char* ImGui_ImplSdl_GetClipboardText()
{
	return SDL_GetClipboardText();
}

static void ImGui_ImplSdl_SetClipboardText(const char* text)
{
	SDL_SetClipboardText(text);
}

bool ImGui_ImplSdl_ProcessEvent(SDL_Event* event)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (event->type)
	{
		case SDL_MOUSEWHEEL:
		{
			// if(event->wheel.y > 20)
			// 	g_MouseWheelV = +9;

			// else if(event->wheel.y > 10)
			// 	g_MouseWheelV = +4;

			// else if(event->wheel.y > 0)
			// 	g_MouseWheelV = +1;

			// else if(event->wheel.y < 20)
			// 	g_MouseWheelV = -9;

			// else if(event->wheel.y < 10)
			// 	g_MouseWheelV = -4;

			// else if(event->wheel.y < 0)
			// 	g_MouseWheelV = -1;





			// if(event->wheel.x > 20)
			// 	g_MouseWheelH = +9;

			// else if(event->wheel.x > 10)
			// 	g_MouseWheelH = +4;

			// else if(event->wheel.x > 0)
			// 	g_MouseWheelH = +1;

			// else if(event->wheel.x < 20)
			// 	g_MouseWheelH = -9;

			// else if(event->wheel.x < 10)
			// 	g_MouseWheelH = -4;

			// else if(event->wheel.x < 0)
			// 	g_MouseWheelH = -1;


			if(event->wheel.y > 0)
				g_MouseWheelV = +1;
			else if(event->wheel.y < 0)
				g_MouseWheelV = -1;

			if(event->wheel.x > 0)
				g_MouseWheelH = +1;
			else if(event->wheel.x < 0)
				g_MouseWheelH = -1;


			return true;
		}

		case SDL_MOUSEBUTTONDOWN:
		{
			if(event->button.button == SDL_BUTTON_LEFT) g_MousePressed[0] = true;
			if(event->button.button == SDL_BUTTON_RIGHT) g_MousePressed[1] = true;
			if(event->button.button == SDL_BUTTON_MIDDLE) g_MousePressed[2] = true;
			return true;
		}

		case SDL_TEXTINPUT:
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddInputCharactersUTF8(event->text.text);
			return true;
		}

		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
			io.KeysDown[key] = (event->type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
			io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			return true;
		}
	}
	return false;
}
bool ImGui_ImplSdl_CreateDeviceObjects()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void*) (uintptr_t) g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

void ImGui_ImplSdl_InvalidateDeviceObjects()
{
	if(g_FontTexture)
	{
		glDeleteTextures(1, &g_FontTexture);
		ImGui::GetIO().Fonts->TexID = 0;
		g_FontTexture = 0;
	}
}

bool ImGui_ImplSdl_Init(SDL_Window *window)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;		// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDLK_a;
	io.KeyMap[ImGuiKey_C] = SDLK_c;
	io.KeyMap[ImGuiKey_V] = SDLK_v;
	io.KeyMap[ImGuiKey_X] = SDLK_x;
	io.KeyMap[ImGuiKey_Y] = SDLK_y;
	io.KeyMap[ImGuiKey_Z] = SDLK_z;


	// io.RenderDrawListsFn = ImGui_ImplSdl_RenderDrawLists;	// Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.

	// yes, we set it to null here.
	io.RenderDrawListsFn = 0;

	io.SetClipboardTextFn = ImGui_ImplSdl_SetClipboardText;
	io.GetClipboardTextFn = ImGui_ImplSdl_GetClipboardText;

#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	io.ImeWindowHandle = wmInfo.info.win.window;
#endif

	int fb_w = 0;
	int fb_h = 0;

	SDL_GL_GetDrawableSize(window, &fb_w, &fb_h);
	ImGui::GetIO().DisplayFramebufferScale.x = fb_w / ImGui::GetIO().DisplaySize.x;
	ImGui::GetIO().DisplayFramebufferScale.y = fb_h / ImGui::GetIO().DisplaySize.y;

	return true;
}

void ImGui_ImplSdl_Shutdown()
{
	ImGui_ImplSdl_InvalidateDeviceObjects();
	ImGui::Shutdown();
}

void ImGui_ImplSdl_NewFrame(SDL_Window *window)
{
	if(!g_FontTexture)
		ImGui_ImplSdl_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	io.DisplaySize = ImVec2((float) w, (float) h);

	// Setup time step
	Uint32	time = SDL_GetTicks();
	double current_time = time / 1000.0;
	io.DeltaTime = g_Time > 0.0 ? (float) (current_time - g_Time) : (float) (1.0f/60.0f);
	g_Time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
	int mx, my;
	Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
	if(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)
		io.MousePos = ImVec2((float) mx, (float) my);	// Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
	else
		io.MousePos = ImVec2(-1, -1);

	io.MouseDown[0] = g_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = g_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	io.MouseDown[2] = g_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
	g_MousePressed[0] = g_MousePressed[1] = g_MousePressed[2] = false;

	io.MouseWheel = g_MouseWheelV;
	// io.MouseWheelX = g_MouseWheelH;

	g_MouseWheelV = 0.0f;
	g_MouseWheelH = 0.0f;

	// change the thing accordingly, because imgui does weird stupid shit
	switch(ImGui::GetCurrentContext()->MouseCursor)
	{
		case ImGuiMouseCursor_Arrow:
			ImGuiBackend::SetCursor(ImGuiBackend::CursorType::Arrow);
			break;

		case ImGuiMouseCursor_TextInput:
			ImGuiBackend::SetCursor(ImGuiBackend::CursorType::IBeam);
			break;

		case ImGuiMouseCursor_Move:
			ImGuiBackend::SetCursor(ImGuiBackend::CursorType::Move);
			break;

		case ImGuiMouseCursor_ResizeNS:
			ImGuiBackend::SetCursor(ImGuiBackend::CursorType::ResizeNS);
			break;

		case ImGuiMouseCursor_ResizeEW:
			ImGuiBackend::SetCursor(ImGuiBackend::CursorType::ResizeEW);
			break;

		case ImGuiMouseCursor_ResizeNESW:
			ImGuiBackend::SetCursor(ImGuiBackend::CursorType::ResizeNESW);
			break;

		case ImGuiMouseCursor_ResizeNWSE:
			ImGuiBackend::SetCursor(ImGuiBackend::CursorType::ResizeNWSE);
			break;

	}


	// Hide OS mouse cursor if ImGui is drawing it
	SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

	// Start the frame
	ImGui::NewFrame();
}
