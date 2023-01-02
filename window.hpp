#pragma once
#include "kisslib/serialization.hpp"
#include "kisslib/kissmath.hpp"
#include "input.hpp"

struct Game;

struct IApp {
	virtual ~IApp () {};

	virtual void json_load () = 0;
	virtual void json_save () = 0;
	
	virtual void imgui (Window& window) = 0;
	virtual void frame (Window& window) = 0;
	
	enum ShouldClose {
		CLOSE_CANCEL=0,
		CLOSE_PENDING,
		CLOSE_NOW,
	};
	// called during imgui execution when app wants to close
	// return CLOSE_PENDING when you want to open a popup until the user decides if they really want to close
	//   -> close_confirmation will be called again the next frame
	// return CLOSE_CANCEL = when the user wanted to cancel closing
	// return CLOSE_NOW = when the user wants to close
	// don't implement this to get normal immediate closing behavior
	virtual ShouldClose close_confirmation () {
		return CLOSE_NOW;
	}
};
int run_game (IApp* make_game(Window&), const char* window_title);


struct Rect {
	int2	 pos;
	int2	 dim;
};

struct GLFWwindow;
struct GLFWcursor;

struct Window {
	SERIALIZE(Window, input)

	GLFWwindow* window;
	Input input = {};

	DirectoyChangeNotifier file_changes;
	IApp* _app; // only needed to support drawing on window resize
	
	// close down game after current frame
	void close ();
	
	bool fullscreen = false; // readonly
	bool borderless_fullscreen = true; // readonly, use borderless fullscreen as long as the cpu usage (gpu driver?) bug happens on my dev desktop
	Rect window_pos;

	bool switch_fullscreen (bool fullscreen, bool borderless_fullscreen);
	bool toggle_fullscreen ();

	Timing_Histogram fps_display;
	
	bool vsync = true;
	void set_vsync (bool vsync);

	void draw_imgui ();

	bool imgui_enabled = true;
#if IMGUI_DEMO
	bool imgui_show_demo_window = false;
#endif

	bool screenshot_hud = false;
	bool trigger_screenshot = false;

	enum CursorMode {
		CURSOR_NORMAL=0,
		CURSOR_FINGER,

		_CURSORS_COUNT,
	};
	GLFWcursor* _cursors[Window::_CURSORS_COUNT] = {};

	void set_cursor (CursorMode mode);
};
