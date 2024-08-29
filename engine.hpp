#pragma once
#include "common.hpp"
#include "kisslib/kissmath.hpp"
#include "input.hpp"

struct GLFWwindow;
struct GLFWcursor;

class Engine {
public:

	Engine (const char* window_title);
	virtual ~Engine ();

	GLFWwindow* window;
	Input input = {};

	DirectoyChangeNotifier file_changes = DirectoyChangeNotifier("./", true);
	
	// close down game after current frame
	void close ();

	
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

	int2 window_pos = INT_MIN;
	int2 window_size = int2(1280, 720);

	bool fullscreen = false;
	bool borderless_fullscreen = true;

	bool set_fullscreen (bool fullscreen, bool borderless_fullscreen);
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
	GLFWcursor* _cursors[_CURSORS_COUNT] = {};

	void set_cursor (CursorMode mode);

	virtual void imgui () = 0;
	virtual void frame () = 0;

	virtual bool update_files_changed (kiss::ChangedFiles& changed_files) = 0;

	virtual void json_load () = 0;
	virtual void json_save () = 0;

	int main_loop ();
};
