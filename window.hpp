#pragma once
#include "kisslib/kissmath.hpp"
#include "input.hpp"

struct IApp {
	virtual ~IApp () {};

	virtual void frame (Window& window) = 0;

	virtual void json_load () = 0;
	virtual void json_save () = 0;
};
int run_game (IApp* make_game(Window&), const char* window_title);


struct Rect {
	int2	 pos;
	int2	 dim;
};

struct GLFWwindow;

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

	bool imgui_enabled = true;
	bool imgui_show_demo_window = false;

	bool screenshot_hud = false;
	bool trigger_screenshot = false;
};
